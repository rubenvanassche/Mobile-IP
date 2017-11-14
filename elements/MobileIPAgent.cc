#include <click/config.h>
#include "MobileIPAgent.hh"

CLICK_DECLS

MobileIPAgent::MobileIPAgent() {
	this->push(0, NULL);
 };
MobileIPAgent::~MobileIPAgent() { };

void MobileIPAgent::push(int port, Packet *p) {
	std::cout << "ports " << this->name() << std::endl;
	Packet* x = buildRegistrationRequestPacket(100, IPAddress("1.2.3.4"), IPAddress("1.2.3.4"), IPAddress("1.2.3.4"));
	std::cout << x->length() << std::endl;
	this->output(0).push(x);
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPAgent)
