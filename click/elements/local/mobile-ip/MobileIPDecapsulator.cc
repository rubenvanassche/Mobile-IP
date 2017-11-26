#include <click/config.h>
#include "MobileIPDecapsulator.hh"

CLICK_DECLS

MobileIPDecapsulator::MobileIPDecapsulator() { };
MobileIPDecapsulator::~MobileIPDecapsulator() { };

int MobileIPDecapsulator::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("FA",ElementCastArg("MobileIPForeignAgent"), this->FA)
		.execute() < 0){
			return -1;
		}

		return 0;
}

Packet *MobileIPDecapsulator::simple_action(Packet *p) {
	if(getPacketType(p) == IPINIP){
		IPHeader ip = processIPHeader(p);

	}

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPDecapsulator)
