#include <click/config.h>
#include "MobileIPNode.hh"

CLICK_DECLS

MobileIPNode::MobileIPNode() {
	this->simple_action(NULL);
};
MobileIPNode::~MobileIPNode() { };

Packet *MobileIPNode::simple_action(Packet *p) {
	// TODO: fill
	std::cout << this->noutputs() << std::endl;

	return p;
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPNode)
