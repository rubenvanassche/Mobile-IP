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
	this->routerAddress = IPAddress();
	this->advertisementSequenceNumber = 0;

	click_chatter("Node was moved");
}

Packet *MobileIPSoliciter::simple_action(Packet *p) {
	if(getPacketType(p) != ADVERTISEMENT){
		return NULL;
	}

	routerAdvertisement advertisement = processRouterAdvertisementMessage(p);
	if(advertisement.homeAgent == true){
		// TODO this should probably be executed but then move dection doesn't work as it should
		//return NULL;
	}

	if(this->connected == false){
		// Not connected with agent so let's do that
		this->connect(advertisement);

	}else if(this->connected == true and this->routerAddress == advertisement.IP.source){
		// connected with agent in advertisement, so check if the agent resetted itself and raise lifetimes
		if((advertisement.sequenceNumber - 1) != this->advertisementSequenceNumber and advertisement.sequenceNumber < 256){
			// agent resetted itself
			std::cout << "Agent resetted itself" << std::endl;
			this->MN->reregister(advertisement.IP.source, advertisement.careOfAddress, advertisement.lifetime);
		}else{
			this->advertisementSequenceNumber++;
			this->raiseLifetime(advertisement.lifetime);
		}

	}else if(this->connected == true and this->enableFastMoving == true){
		// advertisment from other agent, normally ignored. But fast moving is on so let's do that
		this->disconnect();

		// Let's reconnect
		this->connect(advertisement);
	}

	return NULL;
}

void MobileIPSoliciter::connect(routerAdvertisement advertisement){
	this->routerAddress = advertisement.IP.source;
	this->connected = true;
	this->raiseLifetime(advertisement.lifetime);
	this->advertisementSequenceNumber = advertisement.sequenceNumber;

	this->MN->reregister(advertisement.IP.source, advertisement.careOfAddress, advertisement.lifetime);
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
