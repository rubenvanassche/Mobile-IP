#include <click/config.h>
#include "MobileIPSoliciter.hh"

CLICK_DECLS

MobileIPSoliciter::MobileIPSoliciter()  : lifetimeTimer(this), solicitationTimer(this) { };
MobileIPSoliciter::~MobileIPSoliciter() { };

int MobileIPSoliciter::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("LINK_ADDRESS", this->linkAddress)
		.read_m("MN",ElementCastArg("MobileIPNode"), this->MN)
		.execute() < 0){
			return -1;
		}

		return 0;
}

int MobileIPSoliciter::initialize(ErrorHandler *) {
    lifetimeTimer.initialize(this);
		solicitationTimer.initialize(this);

    return 0;
}
void MobileIPSoliciter::run_timer(Timer *timer) {
		if(timer == &lifetimeTimer){
			click_chatter("Node was moved");
			this->disconnect();
			return;
		}

		if(timer == &solicitationTimer){
			this->sendSolicitation();
			timer->reschedule_after_sec(this->solicitationInterval);
			return;
		}

}

void MobileIPSoliciter::add_handlers(){
		add_write_handler("set_solicitation_interval", &changeSolicitationIntervalHandler, (void *)0);
		add_write_handler("enable_fast_moving", &enableFastMovingHandler, (void *)0);
}


int MobileIPSoliciter::changeSolicitationIntervalHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
		MobileIPSoliciter* me = (MobileIPSoliciter*) e;

		int seconds;
		if(Args(errh).push_back_args(conf).read_mp("SECONDS", seconds).complete() < 0){
				return -1;
		}

		me->changeSolicitationInterval(seconds);

		return 0;
}

int MobileIPSoliciter::enableFastMovingHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
	MobileIPSoliciter* me = (MobileIPSoliciter*) e;

	bool enabled;
	if(Args(errh).push_back_args(conf).read_mp("VALUE", enabled).complete() < 0){
			return -1;
	}

	me->enableFastMoving = enabled;

	return 0;
}

void MobileIPSoliciter::raiseLifetime(int seconds){
	this->lifetimeTimer.schedule_after_sec(seconds);
}

void MobileIPSoliciter::disconnect(){
	this->connected = false;
	this->agentAddress = IPAddress();
	this->advertisementSequenceNumber = 0;

}

Packet *MobileIPSoliciter::simple_action(Packet *p) {
	if(getPacketType(p) != ADVERTISEMENT){
		return NULL;
	}

	//EtherHeader ether = processEtherHeader(p);

	routerAdvertisement adv;

	try{
		adv = processRouterAdvertisementMessage(p);
	}catch(ZeroChecksumException &e){
		click_chatter("Zero Checksum in Router Advertisement");
	}catch(InvalidChecksumException &e){
		click_chatter("Invalid Checksum in Router Advertisement");
	}


	if(adv.IP.source == this->MN->homeAgentPrivateAddress){
		if(this->connected == true and this->agentAddress ==  adv.IP.source){
			// Already connected with HA, we don't care about sequence numbers because
			// the reset of HA is not important
			this->advertisementSequenceNumber++;
			this->raiseLifetime(adv.lifetime);
			return NULL;
		}

		// When an Home Agent advertisement is recieved immediatly connect with it
		if(this->firstConnection == true){
			// This is the first connection, with an HA this means we do not register
			this->firstConnection = false;

			this->disconnect();
			this->connect(adv, false);
		}else{
			click_chatter("Node was moved");
			this->disconnect();
			this->connect(adv);
		}

	}else if(this->connected == false){
			// Not connected with FA so let's do that
			this->connect(adv);
	}else if(this->agentAddress == adv.IP.source){
		// connected with FA in advertisement, so check if the agent resetted itself and raise lifetimes
		if((adv.sequenceNumber - 1) != this->advertisementSequenceNumber and adv.sequenceNumber < 256){
			// agent resetted itself
			std::cout << "Agent resetted itself" << std::endl;
			this->MN->reregister(adv.IP.source, adv.careOfAddress, adv.lifetime);
			this->advertisementSequenceNumber = adv.sequenceNumber;
		}else{
			this->advertisementSequenceNumber++;
			this->raiseLifetime(adv.lifetime);
		}

	}else if(this->enableFastMoving == true){
		// advertisment from other agent, normally ignored. But fast moving is on so let's do that
		this->disconnect();

		// Let's reconnect
		this->connect(adv);

		click_chatter("Node was moved");
	}else{
		// Sinkhole
		// Is connected, is not an HA, is not current FA agent and should not fast move
	}

	return NULL;
}

void MobileIPSoliciter::connect(routerAdvertisement advertisement, bool requestRegistration){
	this->agentAddress = advertisement.IP.source;
	this->connected = true;
	this->raiseLifetime(advertisement.lifetime);
	this->advertisementSequenceNumber = advertisement.sequenceNumber;

	if(requestRegistration == true){
		this->MN->reregister(advertisement.IP.source, advertisement.careOfAddress, advertisement.lifetime);
	}
}

void MobileIPSoliciter::changeSolicitationInterval(unsigned int seconds){
	this->solicitationInterval = seconds;

	if(seconds == 0){
		this->solicitationTimer.unschedule();
	}else{
		this->solicitationTimer.schedule_now();
	}
}

bool MobileIPSoliciter::sendSolicitation(){
	this->sendSolicitation(IPAddress("255.255.255.255"));
}

bool MobileIPSoliciter::sendSolicitation(IPAddress destination){
	WritablePacket* packet = buildRouterSolicitationMessage();
	ICMPIPfy(packet, this->linkAddress, destination, 1);

	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPSoliciter)
