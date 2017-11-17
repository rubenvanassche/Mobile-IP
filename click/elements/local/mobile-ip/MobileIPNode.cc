#include <click/config.h>
#include "MobileIPNode.hh"

CLICK_DECLS

MobileIPNode::MobileIPNode() {

};
MobileIPNode::~MobileIPNode() { };

int MobileIPNode::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("HOME_ADDRESS", this->homeAddress)
		.read_m("HA_ADDRESS",this->homeAgentAddress)
		.complete() < 0) return -1;
}

Packet* MobileIPNode::simple_action(Packet *p) {
	tunnelIP r = processTunnelIPPacket(p, true);
	std::cout << "cc" << r.ttl << std::endl;

	//this->sendRegistrationRequest(IPAddress("1.1.1.1"), 225);

	return p;
};
/*
bool MobileIPNode::sendRegistrationRequest(IPAddress destination, unsigned int lifetime){
	WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, destination, 434, 1);

	output(0).push(packet);
}
*/

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
