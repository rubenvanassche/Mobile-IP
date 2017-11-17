#include <click/config.h>
#include "MobileIPAgent.hh"

CLICK_DECLS

MobileIPAgent::MobileIPAgent() {

 };
MobileIPAgent::~MobileIPAgent() { };

void MobileIPAgent::push(int port, Packet *p) {
  /*
	Packet* x = buildRegistrationRequestPacket(125, IPAddress("1.2.3.4"), IPAddress("1.2.3.4"), IPAddress("1.2.3.4"));
	this->output(0).push(x);
*/

  WritablePacket* y = buildRegistrationReplyPacket(125, 12, IPAddress("1.2.3.4"), IPAddress("1.2.3.4"));
  UDPIPfy(y, IPAddress("191.35.6.3"), 100, IPAddress("192.54.8.7"), 434, 50);
  this->output(0).push(y);
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPAgent)
