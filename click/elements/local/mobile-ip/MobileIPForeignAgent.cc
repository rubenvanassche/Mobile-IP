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

String MobileIPForeignAgent::getVisitorsHandler(Element *e, void * thunk){
		MobileIPForeignAgent * me = (MobileIPForeignAgent *) e;
		return me->visitors.print().c_str();
}

void MobileIPForeignAgent::add_handlers(){
		add_read_handler("visitors", &getVisitorsHandler, (void *)0);
}

void MobileIPForeignAgent::push(int port, Packet *p) {
	// Priavte network
	if(port == 0 and getPacketType(p) == REGISTRATION){
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

		this->relayRequest(registration);
	}

	// Public network
	if(port == 1 and getPacketType(p) == REPLY){
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

		this->relayReply(reply);
	}

	// Kill the packet because we've dealt with it
	p->kill();
}

void MobileIPForeignAgent::sendRequestTimedOutReply(std::vector<RequestListItem> requests){
		for(auto request : requests){
				this->sendReply(request.rr, 78);
		}
}

void MobileIPForeignAgent::sendReply(registrationRequest registration, unsigned int code){
	this->sendReply(registration, code, registration.lifetime);
}

void MobileIPForeignAgent::sendReply(registrationRequest registration, unsigned int code, unsigned int lifetime){
	IPAddress source = registration.IP.destination;
	IPAddress destination = registration.IP.source;

	// RFC p55
	if(destination == IPAddress("255.255.255.255") ){
		source = this->privateAddress;
	}

	if(registration.home == IPAddress("0.0.0.0")){
		destination = IPAddress("255.255.255.255");
	}

	WritablePacket* packet = buildRegistrationReplyPacket(lifetime, code, registration.home, registration.homeAgent, registration.identification);
	UDPIPfy(packet, source, 434, destination, registration.UDP.sourcePort, 1);

	output(0).push(packet);
}

void MobileIPForeignAgent::sendReply(registrationReply registration, IPAddress destination, unsigned int destinationPort){
	IPAddress source = this->privateAddress;

	// RFC p55
	if(destination == IPAddress("255.255.255.255") ){
		source =this->privateAddress;
	}

	if(registration.home == IPAddress("0.0.0.0")){
		destination = IPAddress("255.255.255.255");
	}

	WritablePacket* packet = buildRegistrationReplyPacket(registration.lifetime, registration.code, registration.home, registration.homeAgent, registration.identification);
	UDPIPfy(packet, source, 434, destination, destinationPort, 1);
	output(0).push(packet);

}

void MobileIPForeignAgent::relayReply(registrationReply reply){
		RequestListItem request;
		try{
			request = this->requests.remove(reply.identification);
		}catch(RequestNotFoundException &e){
			// Captured an reply to an request we haven't
			click_chatter("Reply to unkwon request recieved");
			return;
		}

		if(reply.code == 0 or reply.code == 1){
				// Home agent accepted
				unsigned int remainingLifetime = std::min(reply.lifetime, this->maxAcceptedLifetime);

				if(this->visitors.has(reply.home, reply.homeAgent)){
					if(reply.lifetime == 0){
						// Remove the MN from the visitors list
						this->visitors.remove(reply.home, reply.homeAgent);
					}else{
						// Update the visitors list
						this->visitors.setVisitorRequestedLifetime(reply.home, reply.homeAgent, remainingLifetime);
					}
				}else{
					// Add to visitor list
					VisitorListItem visitor;
					visitor.MNsource = request.rr.IP.source;
					visitor.MNhome = reply.home;
					visitor.MNhomeAgent = reply.homeAgent;
					visitor.UDPSourcePort = request.rr.UDP.sourcePort;
					visitor.requestedLifetime = remainingLifetime;
					visitor.remainingLifetime = remainingLifetime;

					this->visitors.add(visitor);
				}
		}

		this->sendReply(reply, reply.home, request.rr.UDP.sourcePort);

		if(reply.lifetime != 0){
			if(reply.code == 0 or reply.code == 1){
				// Reset the lfietime in the visitor list
				this->visitors.resetVisitorRemainingLifetime(reply.home, reply.homeAgent);
			}else{
				// Something went wrong in the HA so remove the visitor
				// Remove the MN from the visitors list
				this->visitors.remove(reply.home, reply.homeAgent);
			}
		}
}

void MobileIPForeignAgent::relayRequest(registrationRequest registration){
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
			request.identification = registration.identification;
			request.rr = registration;

			this->requests.add(request);

			// Now send the request to the home agent
			WritablePacket* packet = buildRegistrationRequestPacket(registration.lifetime, registration.home, registration.homeAgent, this->careOfAddress, registration.identification);
			UDPIPfy(packet, this->careOfAddress, this->sourcePort, registration.homeAgent, 434, 16);
			output(1).push(packet);
}

bool MobileIPForeignAgent::checkRegistrationValidity(registrationRequest registration){
		// TODO Check if there are other devices in the subnet that send this request

		// check if reserved fields are 0
		if(registration.r != false  or registration.x != false){
			this->sendReply(registration, 70);
			return false;
		}

		// Check if the right care-of-address was givven
		if(registration.careOf != this->careOfAddress){
			this->sendReply(registration, 77);
			return false;
		}

		// Check the requested encapsulation
		if(registration.M == true or registration.G == true){
			this->sendReply(registration, 72);
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
		if(registration.D == true or registration.T == true){
			this->sendReply(registration, 64);
			return false;
		}

		return true;
}

bool MobileIPForeignAgent::checkRegistrationValidity(registrationReply registration){
		// check if reserved fields are 0
		/*
		if(registration.r != false  or registration.x != false){
			this->sendReply(registration, 71);
			return false;
		}
		*/

		return true;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
