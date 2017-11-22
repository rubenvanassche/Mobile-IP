#include <click/config.h>
#include "MobileIPForeignAgent.hh"

CLICK_DECLS

MobileIPForeignAgent::MobileIPForeignAgent() { };
MobileIPForeignAgent::~MobileIPForeignAgent() { };

int MobileIPForeignAgent::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("PRIVATE_ADDRESS", this->privateAddress)
		.read_m("PUBLIC_ADDRESS", this->publicAddress)
		.read_m("HA_ADDRESS",this->HAAddress)
		.execute() < 0){
			return -1;
		}

		return 0;
}

int MobileIPForeignAgent::initialize(ErrorHandler *) {
		/*
    advertisementTimer.initialize(this);
    advertisementTimer.schedule_now();

    return 0;
		*/

	return 0;
}

void MobileIPForeignAgent::add_handlers(){
		//add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
}

Packet *MobileIPForeignAgent::simple_action(Packet *p) {
	std::cout << "fffff" << std::endl;

	return NULL;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
