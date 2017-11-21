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
			//return -1;
		}
}

Packet* MobileIPNode::simple_action(Packet *p) {
	try{
		registrationReply r = processRegistrationReplyPacket(p);
		this->processReply(r);
	}catch(InvalidChecksumException &e){
		click_chatter("The registration reply contained an invalid checksum!");
		return NULL;
	}



	return NULL;
}

void MobileIPNode::processReply(registrationReply reply){
	if(reply.code == 0 or reply.code == 1){
		// TODO change routing table
	}
}

bool MobileIPNode::reregister(IPAddress address, unsigned int lifetime){
	if(this->homeAgentAddress == address){
		this->registerHA(lifetime);
	}else{
		this->registerFA(address, lifetime);
	}
}

bool MobileIPNode::registerLL(){
  WritablePacket* packet = buildRegistrationRequestPacket(1800, this->homeAddress, this->homeAgentAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, IPAddress("224.0.0.11"), 434, 1);

	output(0).push(packet);
}

bool MobileIPNode::registerFA(IPAddress FAAddress, unsigned int lifetime){
  WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, FAAddress, 434, 1);

	output(0).push(packet);
}

bool MobileIPNode::registerHA(unsigned int lifetime){
	// Set ttl to 16 because why not?
  WritablePacket* packet = buildRegistrationRequestPacket(lifetime, this->homeAddress, this->homeAgentAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, this->homeAgentAddress, 434, 16);

	output(0).push(packet);
}

bool MobileIPNode::deregister(IPAddress FAAddress){
	WritablePacket* packet = buildRegistrationRequestPacket(0, this->homeAddress, this->homeAgentAddress, this->homeAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, FAAddress, 434, 1);

	output(0).push(packet);
}

bool MobileIPNode::deregister(IPAddress FAAddress, IPAddress address){
	WritablePacket* packet = buildRegistrationRequestPacket(0, this->homeAddress, this->homeAgentAddress, this->careOfAddress);
	UDPIPfy(packet, this->homeAddress, this->sourcePort, FAAddress, 434, 1);

	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
