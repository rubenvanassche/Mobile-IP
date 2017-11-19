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
    // This function is called when the timer fires.
    assert(timer == &advertisementTimer);
    Timestamp now = Timestamp::now_steady();
    click_chatter("%s: %p{timestamp}: timer fired with expiry %p{timestamp}!\n",
                  declaration().c_str(), &now, &advertisementTimer.expiry_steady());
           // _timer.expiry_steady() is the steady-clock Timestamp
           // at which the timer was set to fire.
    advertisementTimer.reschedule_after_sec(5);  // Fire again 5 seconds later.
}

Packet* MobileIPAdvertiser::simple_action(Packet *p) {
	// TODO: fill

	return p;
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
	this->sequenceNumber++;

	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPAdvertiser)
