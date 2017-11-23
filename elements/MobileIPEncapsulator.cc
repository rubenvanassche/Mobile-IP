#include <click/config.h>
#include "MobileIPEncapsulator.hh"

CLICK_DECLS

MobileIPEncapsulator::MobileIPEncapsulator() { };
MobileIPEncapsulator::~MobileIPEncapsulator() { };

Packet *MobileIPEncapsulator::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPEncapsulator)
