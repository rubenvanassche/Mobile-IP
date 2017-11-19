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
		.execute() < 0){
			return -1;
		}
}

Packet* MobileIPNode::simple_action(Packet *p) {
	registrationReply r = processRegistrationReplyPacket(p);
	std::cout << "cc" << r.IP.source.s().c_str() << std::endl;

	/*
	WritablePacket* m = buildRouterSolicitationMessage();
	IPfy(m, IPAddress("124.58.1.1"), IPAddress("123.78.9.6"), 1);
	output(0).push(m);
	*/

	return p;
};

bool MobileIPNode::registerLL(){
	// TODO set lifetime
	return this->sendRegistrationRequest(IPAddress("224.0.0.11"), 100);
}

bool MobileIPNode::registerFA(IPAddress FAAddress, unsigned int lifetime){
	return this->sendRegistrationRequest(FAAddress , lifetime);
}

bool MobileIPNode::registerHA(){

}

bool MobileIPNode::sendRegistrationRequest(IPAddress destination, unsigned int lifetime){
	WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, destination, 434, 1);

	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
