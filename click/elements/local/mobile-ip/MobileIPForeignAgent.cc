#include <click/config.h>
#include "MobileIPForeignAgent.hh"

CLICK_DECLS

MobileIPForeignAgent::MobileIPForeignAgent() : requestsTimer(this) { };
MobileIPForeignAgent::~MobileIPForeignAgent() { };

int MobileIPForeignAgent::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("PRIVATE_ADDRESS", this->privateAddress)
		.read_m("PUBLIC_ADDRESS", this->publicAddress)
		.read_m("HA_ADDRESS",this->HAAddress)
		.execute() < 0){
			return -1;
		}

		this->careOfAddress = this->publicAddress;

		return 0;
}

int MobileIPForeignAgent::initialize(ErrorHandler *) {
    requestsTimer.initialize(this);
    requestsTimer.schedule_now();

		return 0;
}

void MobileIPForeignAgent::run_timer(Timer *timer) {
    assert(timer == &requestsTimer);
    Timestamp now = Timestamp::now_steady();

		// Decrease the lifetime by one in the pending requests table
		this->requests.decreaseLifetime();

		// Remove pending requests running longer then 7 seconds
		this->sendRequestTimedOutReply(this->requests.removeTimedOutRequests(7));

		// remove visitors from visitorlist when remaaining lifetime is 0 and decrease the lifetime
		this->visitors.decreaseLifetime();

    requestsTimer.reschedule_after_sec(1);
}

void MobileIPForeignAgent::add_handlers(){
		//add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
}

void MobileIPForeignAgent::push(int port, Packet *p) {
	// Priavte network
	if(port == 0){
		if(getPacketType(p) == REGISTRATION){
			registrationRequest registration;
			try{
				registration = processRegistrationRequestPacket(p);
			}catch(ZeroChecksumException &e){
				click_chatter("Request recieved at FA has an IP checksum 0");
				return;
			}catch(InvalidChecksumException &e){
				click_chatter("Request recieved at FA has an invalid checksum");
				return;
			}

			// Checks if the request recieved is valid
			if(checkRegistrationValidity(registration) == false){
				return;
			}

			// Create an new entry in the pending requests list
			RequestListItem request;
			request.destination = registration.IP.destination;
			request.careOfAddress = this->careOfAddress;
			request.requestedLifetime = registration.lifetime;
			request.remainingLifetime = registration.lifetime;
			request.rr = registration;

			this->requests.add(request);

			// Now send the request to the home agent
			WritablePacket* packet = buildRegistrationRequestPacket(registration.lifetime, registration.home, registration.homeAgent, this->careOfAddress, registration.identification);
			UDPIPfy(packet, this->careOfAddress, this->sourcePort, registration.homeAgent, 434, 16);
			output(1).push(packet);


			p->kill();
		}

		return;
	}

	// Public network
	if(port == 1){
		if(getPacketType(p) == REPLY){
			registrationReply reply;
			try{
				reply = processRegistrationReplyPacket(p);
			}catch(ZeroChecksumException &e){
				click_chatter("Reply from HA recieved at FA has an IP checksum 0");
				return;
			}catch(InvalidChecksumException &e){
				click_chatter("Reply from HA recieved at FA has an invalid checksum");
				return;
			}

			this->sendReplyFromHA(reply);

			// Kill the packet because we've dealt with it
			p->kill();

		}

		return;
	}

};

void MobileIPForeignAgent::sendRequestTimedOutReply(std::vector<RequestListItem> requests){
		for(auto request : requests){
				this->sendReply(request.rr, 78);
		}
}


void MobileIPForeignAgent::sendReply(registrationRequest registration, unsigned int code){
	this->sendReply(registration, code, registration.lifetime);
}

void MobileIPForeignAgent::sendReply(registrationRequest registration, unsigned int code, unsigned int lifetime){
	IPAddress source = this->privateAddress;
	IPAddress destination = registration.IP.source;

	// RFC p55
	if(destination == IPAddress("255.255.255.255") ){
		source = IPAddress("255.255.255.255");
	}

	if(registration.home == IPAddress("0.0.0.0")){
		destination = IPAddress("255.255.255.255");
	}

	WritablePacket* packet = buildRegistrationReplyPacket(lifetime, code, registration.home, registration.homeAgent, registration.identification);
	UDPIPfy(packet, source, 434, destination, registration.UDP.sourcePort, 1);

	output(0).push(packet);
}

void MobileIPForeignAgent::sendReplyFromHA(registrationReply reply){
		if(reply.code == 0 or reply.code == 1){
				// Home agent accepted
				if(reply.lifetime == 0){
					// Remove the MN from the visitors list
					this->visitors.remove(reply.home, reply.homeAgent);
				}else{
					// Add or update visitors list
					unsigned int remainingLifetime = std::min(reply.lifetime, this->maxAcceptedLifetime);

					if(this->visitors.has(reply.home, reply.homeAgent)){
						this->visitors.setVisitorRemainingLifetime(reply.home, reply.homeAgent, remainingLifetime);
					}else{
						// Add a new visitor
						VisitorListItem visitor;
						visitor.MNhome = reply.home;
						visitor.MNhomeAgent = reply.homeAgent;
						visitor.requestedLifetime = remainingLifetime;
						visitor.remainingLifetime = remainingLifetime;

						// TODO some fields are connected to a request, we need this information
					}
				}
		}

		// TODO how to determine that this reply replies to a specified request? Now implemented with homeAddress and homeAgentAddress
		// But that doesn't seems right, better working with data givven in registration like care-of address and so
		// Needs to be investigated
		this->requests.remove(reply.home, reply.homeAgent);

		// TODO Relay the reply(checking the RFC is needed)
		// TODO set the right destination port
		WritablePacket* packet = buildRegistrationReplyPacket(reply.lifetime, reply.code, reply.home, reply.homeAgent, reply.identification);
		UDPIPfy(packet, this->privateAddress, 434, reply.home, 5584, 1);
		output(0).push(packet);
}

bool MobileIPForeignAgent::checkRegistrationValidity(registrationRequest registration){
		// TODO Check if there are other devices in the subnet that send this request

		// check if reserved fields are 0
		if(registration.r != false  or registration.x != false){
			this->sendReply(registration, 134);
			return false;
		}


		// Check if there are too many pending requests
		if(this->requests.size() >= this->maxPendingRegistrations){
			this->sendReply(registration, 66);
			return false;
		}

		// check if lifetime is accepted by this FA
		if(registration.lifetime > this->maxAcceptedLifetime){
			this->sendReply(registration, 69, this->maxAcceptedLifetime);
			return false;
		}

		// Check if client wants to use a co-located address
		if(registration.D == true){
			this->sendReply(registration, 64);
			return false;
		}

		return true;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
