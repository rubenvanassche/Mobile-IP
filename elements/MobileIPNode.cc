#include <click/config.h>
#include "MobileIPNode.hh"

CLICK_DECLS

MobileIPNode::MobileIPNode() {

};
MobileIPNode::~MobileIPNode() { };

Packet *MobileIPNode::simple_action(Packet *p) {
	tunnelIP r = processTunnelIPPacket(p, true);
	std::cout << "cc" << r.ttl << std::endl;


	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
