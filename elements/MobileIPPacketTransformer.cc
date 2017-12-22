#include <click/config.h>
#include "MobileIPPacketTransformer.hh"

CLICK_DECLS

MobileIPPacketTransformer::MobileIPPacketTransformer() { };
MobileIPPacketTransformer::~MobileIPPacketTransformer() { };

int MobileIPPacketTransformer::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_mp("SOLICITER",ElementCastArg("MobileIPSoliciter"), this->soliciter)
		.execute() < 0){
			return -1;
		}

		return 0;
}

Packet* MobileIPPacketTransformer::simple_action(Packet *p) {
	if(this->soliciter->isHome() == false and this->soliciter->agentMAC.isZero() == false){

		WritablePacket* q = p->uniqueify();

		EtherHeader ether = processEtherHeader(q, true);
		Etherfy(q, ether.source, this->soliciter->agentMAC);

		if(ether.destination.isBroadcast()){
			return p;
		}

		p->kill();
		return q;
	}else{
		return p;
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPPacketTransformer)
