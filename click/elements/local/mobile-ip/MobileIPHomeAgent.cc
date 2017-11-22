#include <click/config.h>
#include "MobileIPHomeAgent.hh"

CLICK_DECLS

MobileIPHomeAgent::MobileIPHomeAgent() { };
MobileIPHomeAgent::~MobileIPHomeAgent() { };

int MobileIPHomeAgent::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("PRIVATE_ADDRESS", this->privateAddress)
		.read_m("PUBLIC_ADDRESS", this->publicAddress)
		.read_m("FA_ADDRESS",this->FAAddress)
		.execute() < 0){
			return -1;
		}

		return 0;
}

int MobileIPHomeAgent::initialize(ErrorHandler *) {
		/*
    advertisementTimer.initialize(this);
    advertisementTimer.schedule_now();

    return 0;
		*/

	return 0;
}

void MobileIPHomeAgent::add_handlers(){
		//add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
}

Packet *MobileIPHomeAgent::simple_action(Packet *p) {


	return NULL;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPHomeAgent)
