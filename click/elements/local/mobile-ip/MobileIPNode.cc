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

		// TODO resend requests when no answer p50

		// Decrease the lifetime of the current registration if there is one
		if(this->registrationLifetime > 0){
			this->registrationLifetime -= 1;

			if(this->registrationLifetime <= this->renewLifetime){
				// TODO rergister with same FA address + lifetime
				//this->reregister();
			}
		}

    requestsTimer.reschedule_after_sec(1);
}

Packet* MobileIPNode::simple_action(Packet *p) {
	if(getPacketType(p) != REPLY){
		return NULL;
	}

	try{
		registrationReply r = processRegistrationReplyPacket(p);
		this->processReply(r);
	}catch(InvalidChecksumException &e){
		click_chatter("The registration reply contained an invalid checksum!");
		return NULL;
	}

	return NULL;
}

void MobileIPNode::processReply(registrationReply reply){
	if(reply.code == 0 or reply.code == 1){
		this->connected == true;

		if(reply.IP.source == this->homeAgentPrivateAddress){
			this->isHome = true;
			click_chatter("Connected to HA");
		}else{
			this->isHome = false;
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
			this->registerFA(reply.IP.source, this->careOfAddress, reply.lifetime);
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
}

bool MobileIPNode::reregister(IPAddress address, IPAddress careOfAddress, unsigned int lifetime){
	this->connected = false;

	if(this->homeAgentPrivateAddress == address){
		// TODO should delete registration with FA
		//this->registerHA(lifetime);
	}else{
		this->registerFA(address, careOfAddress, lifetime + 1895);
	}
}

void MobileIPNode::sendRequest(IPAddress destination, IPAddress careOfAddress, unsigned int lifetime, unsigned int ttl){
	unsigned int identification = Timestamp::now_steady().subsec();

	WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, careOfAddress, identification);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, destination, 434, ttl);

	RequestListItem r;
	r.destination = destination;
	r.careOfAddress = careOfAddress;
	r.requestedLifetime = lifetime;
	r.remainingLifetime = lifetime;
	r.identification = identification;
	this->requests.add(r);

	output(0).push(packet);
}

bool MobileIPNode::registerLL(){
	this->sendRequest(IPAddress("224.0.0.11"), this->careOfAddress, 1800);
}

bool MobileIPNode::registerFA(IPAddress FAAddress, IPAddress careOfAddress, unsigned int lifetime){
	this->sendRequest(FAAddress, careOfAddress, lifetime);
}

bool MobileIPNode::registerHA(unsigned int lifetime){
	this->sendRequest(this->homeAgentPrivateAddress, this->careOfAddress, lifetime, 16);
}

bool MobileIPNode::deregister(IPAddress FAAddress){
	this->sendRequest(FAAddress, this->careOfAddress, 0);
}

bool MobileIPNode::deregister(IPAddress FAAddress, IPAddress address){
	this->sendRequest(FAAddress, this->careOfAddress, 0);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
