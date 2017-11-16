#include <click/config.h>
#include "MobileIPForeignAgent.hh"

CLICK_DECLS

MobileIPForeignAgent::MobileIPForeignAgent() { };
MobileIPForeignAgent::~MobileIPForeignAgent() { };

Packet *MobileIPForeignAgent::simple_action(Packet *p) {
	// TODO: fill

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
