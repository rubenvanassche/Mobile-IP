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

void MobileIPPacketTransformer::push(int port, Packet *p) {
	if(this->soliciter->isHome() == false and this->soliciter->agentMAC.isZero() == false){
		WritablePacket* q = p->uniqueify();


		EtherHeader ether = processEtherHeader(q, false);
		Etherfy(q, ether.source, this->soliciter->agentMAC);

		if(ether.destination.isBroadcast()){
			output(1).push(p);
			return;
		}

		p->kill();
		output(1).push(q);
	}else{
		output(0).push(p);
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPPacketTransformer)
