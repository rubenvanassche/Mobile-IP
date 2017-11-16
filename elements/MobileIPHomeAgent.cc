#include <click/config.h>
#include "MobileIPHomeAgent.hh"

CLICK_DECLS

MobileIPHomeAgent::MobileIPHomeAgent() { };
MobileIPHomeAgent::~MobileIPHomeAgent() { };

Packet *MobileIPHomeAgent::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPHomeAgent)
