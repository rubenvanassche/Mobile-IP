#include <click/config.h>
#include "MobileIPClassifier.hh"

CLICK_DECLS

MobileIPClassifier::MobileIPClassifier() { };
MobileIPClassifier::~MobileIPClassifier() { };

void MobileIPClassifier::push(int port, Packet *p){
	PacketType type = getPacketType(p);

	if(type == REGISTRATION){
		output(1).push(p);
	}else if(type == REPLY){
		output(2).push(p);
	}else if(type == IPINIP){
		output(3).push(p);
	}else if(type == SOLICITATION){
		output(4).push(p);
	}else{
		output(0).push(p);
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPClassifier)
