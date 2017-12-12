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

void MobileIPEncapsulator::push(int port, Packet *p) {
	IPHeader ip = processIPHeader(p);

	if(this->HA->mobilityBindings.isMobileBinded(ip.destination)){
		IPAddress destination = this->HA->mobilityBindings.getCareOfAddress(ip.destination);
		WritablePacket* packet = buildTunnelIPPacket(p, this->HA->publicAddress, destination);

		output(1).push(packet);
		p->kill();
	}else{
		output(0).push(p);
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPEncapsulator)
