#include <click/config.h>
#include "MobileIPClassifier.hh"

CLICK_DECLS

MobileIPClassifier::MobileIPClassifier() { };
MobileIPClassifier::~MobileIPClassifier() { };

void MobileIPClassifier::push(int port, Packet *p){
	if(getPacketType(p) == REGISTRATION){
		output(1).push(p);
	}else if(getPacketType(p) == REPLY){
		output(2).push(p);
	}else if(getPacketType(p) == IPINIP){
		output(3).push(p);
	}else{
		output(0).push(p);
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPClassifier)
