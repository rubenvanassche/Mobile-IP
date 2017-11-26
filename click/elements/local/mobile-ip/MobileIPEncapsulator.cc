#include <click/config.h>
#include "MobileIPEncapsulator.hh"

CLICK_DECLS

MobileIPEncapsulator::MobileIPEncapsulator() { };
MobileIPEncapsulator::~MobileIPEncapsulator() { };

int MobileIPEncapsulator::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("HA",ElementCastArg("MobileIPHomeAgent"), this->HA)
		.execute() < 0){
			return -1;
		}

		return 0;
}

Packet *MobileIPEncapsulator::simple_action(Packet *p) {
	IPHeader ip = processIPHeader(p);

	if(this->HA->mobilityBindings.isMobileBinded(ip.destination)){
		IPAddress destination = this->HA->mobilityBindings.getCareOfAddress(ip.destination);

		buildTunnelIPPacket(p, this->HA->publicAddress, destination);
	}

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPEncapsulator)
