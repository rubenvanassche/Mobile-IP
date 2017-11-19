#include <click/config.h>
#include "MobileIPSoliciter.hh"

CLICK_DECLS

MobileIPSoliciter::MobileIPSoliciter() { };
MobileIPSoliciter::~MobileIPSoliciter() { };

Packet *MobileIPSoliciter::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPSoliciter)
