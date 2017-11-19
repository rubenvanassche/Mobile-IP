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

}

int MobileIPAdvertiser::initialize(ErrorHandler *) {
    advertisementTimer.initialize(this);
    advertisementTimer.schedule_now();

    return 0;
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
		this->sequenceNumber = 256;
	}else{
		this->sequenceNumber++;
	}


	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPAdvertiser)
