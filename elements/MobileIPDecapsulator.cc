#include <click/config.h>
#include "MobileIPDecapsulator.hh"

CLICK_DECLS

MobileIPDecapsulator::MobileIPDecapsulator() { };
MobileIPDecapsulator::~MobileIPDecapsulator() { };

Packet *MobileIPDecapsulator::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPDecapsulator)
