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

int MobileIPNode::registerHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
		MobileIPNode* me = (MobileIPNode*) e;

		int lifetime;
		IPAddress address;
		if(Args(errh).push_back_args(conf).read_m("IP", address).read_m("LT", lifetime).complete() < 0){
				return -1;
		}

		me->reregister(address, lifetime);

		return 0;
}


void MobileIPNode::add_handlers(){
		add_write_handler("register", &registerHandler, (void *)0);
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


	if(reply.code == 66){
			click_chatter("Reply 66 : FA has too many pending registrations");
			return;
	}

	if(reply.code == 70){
			click_chatter("Reply 70 : FA poorly formed request");
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

bool MobileIPNode::reregister(IPAddress address, unsigned int lifetime){
	this->connected = false;

	if(this->homeAgentPrivateAddress == address){
		this->registerHA(lifetime);
	}else{
		this->registerFA(address, lifetime);
	}
}

bool MobileIPNode::registerLL(){
	int lifetime = 1800;

  WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, IPAddress("224.0.0.11"), 434, 1);

	RequestListItem r;
	r.destination = IPAddress("224.0.0.11");
	r.careOfAddress = this->careOfAddress;
	r.requestedLifetime = lifetime;
	r.remainingLifetime = lifetime;
	this->requests.add(r);


	output(0).push(packet);
}

bool MobileIPNode::registerFA(IPAddress FAAddress, unsigned int lifetime){
  WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, FAAddress, 434, 1);

	RequestListItem r;
	r.destination = FAAddress;
	r.careOfAddress = this->careOfAddress;
	r.requestedLifetime = lifetime;
	r.remainingLifetime = lifetime;
	this->requests.add(r);

	output(0).push(packet);
}

bool MobileIPNode::registerHA(unsigned int lifetime){
	// Set ttl to 16 because why not?
  WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, this->homeAgentPrivateAddress, 434, 16);

	RequestListItem r;
	r.destination = this->homeAgentPrivateAddress;
	r.careOfAddress = this->careOfAddress;
	r.requestedLifetime = lifetime;
	r.remainingLifetime = lifetime;
	this->requests.add(r);

	output(0).push(packet);
}

bool MobileIPNode::deregister(IPAddress FAAddress){
	int lifetime = 0;

	WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, this->homeAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, FAAddress, 434, 1);

	RequestListItem r;
	r.destination = FAAddress;
	r.careOfAddress = this->careOfAddress;
	r.requestedLifetime = lifetime;
	r.remainingLifetime = lifetime;
	this->requests.add(r);

	output(0).push(packet);
}

bool MobileIPNode::deregister(IPAddress FAAddress, IPAddress address){
	int lifetime = 0;

	WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentPublicAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, FAAddress, 434, 1);

	RequestListItem r;
	r.destination = FAAddress;
	r.careOfAddress = this->careOfAddress;
	r.requestedLifetime = lifetime;
	r.remainingLifetime = lifetime;
	this->requests.add(r);

	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
