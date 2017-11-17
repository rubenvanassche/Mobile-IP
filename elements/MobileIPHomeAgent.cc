#include <click/config.h>
#include "MobileIPHomeAgent.hh"

CLICK_DECLS

MobileIPHomeAgent::MobileIPHomeAgent() { };
MobileIPHomeAgent::~MobileIPHomeAgent() { };

Packet *MobileIPHomeAgent::simple_action(Packet *p) {


	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPHomeAgent)
