#include <click/config.h>
#include "MobileIPAgent.hh"

CLICK_DECLS

MobileIPAgent::MobileIPAgent() {

 };
MobileIPAgent::~MobileIPAgent() { };

void MobileIPAgent::push(int port, Packet *p) {
	Packet* x = buildRegistrationRequestPacket(100, IPAddress("1.2.3.4"), IPAddress("1.2.3.4"), IPAddress("1.2.3.4"));
	this->output(0).push(x);

  Packet* y = buildRegistrationReplyPacket(125, 10, IPAddress("1.2.3.4"), IPAddress("1.2.3.4"));
  this->output(0).push(y);

  Packet* ipip = buildTunnelIPPacket(p, IPAddress("1.2.3.4"), IPAddress("1.2.3.4"));
  this->output(0).push(ipip);
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPAgent)
