#include <click/config.h>
#include "MobileIPAdvertiser.hh"

CLICK_DECLS

MobileIPAdvertiser::MobileIPAdvertiser() : advertisementTimer(this) { };
MobileIPAdvertiser::~MobileIPAdvertiser() { };

int MobileIPAdvertiser::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("LINK_ADDRESS", this->linkAddress)
		.read("CAREOF_ADDRESS",this->careOfAddress)
		.read("FA",this->isForeignAgent)
		.read("HA",this->isHomeAgent)
		.execute() < 0){
			return -1;
		}

		if(this->isForeignAgent == this->isHomeAgent){
			click_chatter("An Advertiser should be an Home Agent or Foreign Agent!");
			return -1;
		}

		return 0;
}

int MobileIPAdvertiser::initialize(ErrorHandler *) {
    advertisementTimer.initialize(this);
    advertisementTimer.schedule_after_sec(3); // Wait some time so the network is ready

    return 0;
}

void MobileIPAdvertiser::add_handlers(){
		add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
}


int MobileIPAdvertiser::changeLifetimeHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh){
		MobileIPAdvertiser* me = (MobileIPAdvertiser*) e;

		int seconds;
		if(Args(errh).push_back_args(conf).read("SECONDS", seconds).complete() < 0){
				return -1;
		}

		if(seconds < 3){
			click_chatter("Seconds should be higher then 3");
			return -1;
		}

		me->resetAdvertisementTimer(seconds);

		return 0;
}

void MobileIPAdvertiser::resetAdvertisementTimer(int seconds){
		this->lifetime = seconds;

		int next = ceil(this->lifetime/3)*1000 - (rand() % 100);
		advertisementTimer.schedule_after_msec(next);
}

void MobileIPAdvertiser::run_timer(Timer *timer) {
    assert(timer == &advertisementTimer);
    Timestamp now = Timestamp::now_steady();

		this->sendAdvertisement();

		int next = ceil(this->lifetime/3)*1000 - (rand() % 100);
    advertisementTimer.reschedule_after_msec(next);
}

Packet* MobileIPAdvertiser::simple_action(Packet *p) {
	try{
		routerSolicitation r = processRouterSolicitationMessage(p);
		this->sendAdvertisement(r.IP.source);
	}catch(ZeroChecksumException &e){
		click_chatter("Zero Checksum in Router Solicitation");
	}catch(InvalidChecksumException &e){
		click_chatter("Invalid Checksum in Router Solicitation");
	}


	return NULL;
};

bool MobileIPAdvertiser::sendAdvertisement(){
	return this->sendAdvertisement(IPAddress("255.255.255.255"));
}

bool MobileIPAdvertiser::sendAdvertisement(IPAddress destination){

	WritablePacket* packet = buildRouterAdvertisementMessage(
		this->lifetime,
		this->registrationLifetime,
		this->careOfAddress,
		this->sequenceNumber,
		this->isHomeAgent,
		this->isForeignAgent
	);

	ICMPIPfy(packet, this->linkAddress, destination, 1);

	// Raise the sequenceNumber
	if(this->sequenceNumber == 65535){
		// When sequence number is 0xffff -> return 256
		this->sequenceNumber = 256;
	}else{
		this->sequenceNumber++;
	}


	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPAdvertiser)
