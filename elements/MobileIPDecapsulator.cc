#include <click/config.h>
#include "MobileIPDecapsulator.hh"

CLICK_DECLS

MobileIPDecapsulator::MobileIPDecapsulator() { };
MobileIPDecapsulator::~MobileIPDecapsulator() { };

int MobileIPDecapsulator::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("FA",ElementCastArg("MobileIPForeignAgent"), this->FA)
		.execute() < 0){
			return -1;
		}

		return 0;
}

void MobileIPDecapsulator::push(int port, Packet *p) {
	if(getPacketType(p) == IPINIP){
		tunnelIP ipinip = processTunnelIPPacket(p, true);

		if(this->FA->visitors.has(ipinip.originalIP.destination, ipinip.source)){
			output(1).push(p);
		}

		// If not in visitor list, ignore
	}else{
		output(0).push(p);
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPDecapsulator)
