#include <click/config.h>
#include "MobileIPNode.hh"

CLICK_DECLS

MobileIPNode::MobileIPNode() : requestsTimer(this) {

};
MobileIPNode::~MobileIPNode() { };

int MobileIPNode::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("HOME_ADDRESS", this->homeAddress)
		.read_m("HA_PRIVATE_ADDRESS",this->homeAgentPrivateAddress)
		.read_m("HA_PUBLIC_ADDRESS",this->homeAgentPublicAddress)
		.complete() < 0){
			return -1;
		}

		return 0;
}

int MobileIPNode::initialize(ErrorHandler *) {
    requestsTimer.initialize(this);
    requestsTimer.schedule_now();

    return 0;
}


void MobileIPNode::add_handlers(){

}

void MobileIPNode::run_timer(Timer *timer) {
    assert(timer == &requestsTimer);
    Timestamp now = Timestamp::now_steady();

		// Decrease the lifetime by one in the pending requests table
		this->requests.decreaseLifetime();

		// resend requests when no answer p50
		std::vector<RequestListItem> timedOutRequests = this->requests.removeTimedOutRequests(this->requestTimeout);
		for(auto request : timedOutRequests){
				this->sendRequest(request.destination, request.careOfAddress, request.requestedLifetime, request.ttl);
		}

		// Decrease the lifetime of the current registration if there is one
		// TODO: this is stilll quite a mess, should be cleaned up
		if(this->connection.lifetime != 65535){
			// ^- check if lifetime is infinity
			if(this->connection.remainingLifetime > 0){
				this->connection.remainingLifetime -= 1;

				if(this->connection.remainingLifetime <= ((double)this->connection.lifetime/2.0)){
					if(this->connection.isHome == false){
						// TODO check if request was send so we don't send requests every second
						this->registerFA(this->connection.agentAddress, this->connection.careOfAddress, this->connection.lifetime);
					}
				}
			}else{
				// TODO deconnect and try to reconenct
			}
		}

    requestsTimer.reschedule_after_sec(1);
}

void MobileIPNode::push(int port, Packet *p){
	if(getPacketType(p) != REPLY){
		return;
	}

	registrationReply reply;
	try{
		reply = processRegistrationReplyPacket(p);
	}catch(InvalidChecksumException &e){
		click_chatter("The registration reply contained an invalid checksum!");
		return;
	}

	if(this->checkReplyValidity(reply) == true){
		this->processReply(reply);
	}
}

void MobileIPNode::processReply(registrationReply reply){
	// Get the request for this REPLY
	RequestListItem request;
	try{
		request = this->requests.remove(reply.identification);
	}catch(RequestNotFoundException &e){
		// Captured an reply to an request we haven't
		click_chatter("Reply to unkwon request recieved");
		return;
	}


	if(reply.code == 0 or reply.code == 1){
		unsigned int decreaseLifetime = abs(reply.lifetime - request.requestedLifetime);

		this->connection.connected == true;
		this->connection.remainingLifetime = request.requestedLifetime - decreaseLifetime; // Set the lifetime according to p49

		if(reply.IP.source == this->homeAgentPrivateAddress){
			this->connection.isHome = true;
			click_chatter("Connected to HA");
		}else{
			this->connection.isHome = false;
			click_chatter("Connected to FA");
		}
	}

	if(reply.code == 64){
			click_chatter("Reply 64 : FA unspecied reason");
			return;
	}

	if(reply.code == 66){
			click_chatter("Reply 66 : FA has too many pending registrations");
			return;
	}

	if(reply.code == 69){
			click_chatter("Reply 69 : FA requested lifetime too long");
			click_chatter("Retry request with lower lifetime");

			// Reregister again
			this->connection.lifetime = reply.lifetime;
			this->registerFA(reply.IP.source, request.careOfAddress, reply.lifetime);
			return;
	}

	if(reply.code == 70){
			click_chatter("Reply 70 : FA poorly formed request");
			return;
	}

	if(reply.code == 72){
			click_chatter("Reply 72 : FA requested encapsulation unavailable");
			return;
	}

	if(reply.code == 77){
			click_chatter("Reply 77 : FA has not the same care of address as request");
			return;
	}

	if(reply.code == 78){
			click_chatter("Reply 78 : FA registration time-out");
			return;
	}

	if(reply.code == 128){
			click_chatter("Reply 128 : HA reason unspecified");
			return;
	}

	if(reply.code == 134){
			click_chatter("Reply 134 : HA poorly formed request");
			return;
	}

	if(reply.code == 136){
			click_chatter("Reply 136 : HA Unkown Home Agent Address");
			// TODO: resend registration
			// TODO set home agent field to the correct one
			return;
	}
}

bool MobileIPNode::checkReplyValidity(registrationReply reply){
	if(reply.identification != this->requests.getLastRequest().identification){
		click_chatter("Reply's identification was not equal to last request send");
		return false;
	}

	if(reply.UDP.destinationPort != this->sourcePort){
		click_chatter("Reply send to wrong port!");
		return false;
	}

	return true;
}

bool MobileIPNode::reregister(IPAddress address, IPAddress careOfAddress, unsigned int lifetime){
	if(this->homeAgentPrivateAddress == address){
		this->deregister(this->connection.careOfAddress);
	}else{
		this->registerFA(address, careOfAddress, lifetime);
	}

	this->connection.connected = false;
	this->connection.agentAddress = address;
	this->connection.careOfAddress = careOfAddress;
	this->connection.lifetime = lifetime;
	this->connection.remainingLifetime = lifetime;
}

void MobileIPNode::sendRequest(IPAddress destination, IPAddress careOfAddress, unsigned int lifetime, unsigned int ttl){
	unsigned int identification = Timestamp::now_steady().subsec();

	WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, careOfAddress, identification);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, destination, 434, ttl);

	RequestListItem r;
	r.destination = destination;
	r.careOfAddress = careOfAddress;
	r.requestedLifetime = lifetime;
	r.identification = identification;
	r.ttl = ttl;
	this->requests.add(r);

	output(0).push(packet);
}

bool MobileIPNode::registerLL(){
	this->sendRequest(IPAddress("255.255.255.255"), this->connection.careOfAddress, 1800);
}

bool MobileIPNode::registerFA(IPAddress FAAddress, IPAddress careOfAddress, unsigned int lifetime){
	this->sendRequest(FAAddress, careOfAddress, lifetime);
}

bool MobileIPNode::deregister(){
	this->sendRequest(this->homeAgentPrivateAddress, this->homeAddress, 0);
}

bool MobileIPNode::deregister(IPAddress address){
	this->sendRequest(this->homeAgentPrivateAddress, address, 0);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
