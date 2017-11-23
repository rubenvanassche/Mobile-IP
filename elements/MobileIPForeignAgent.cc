#include <click/config.h>
#include "MobileIPForeignAgent.hh"

CLICK_DECLS

MobileIPForeignAgent::MobileIPForeignAgent() { };
MobileIPForeignAgent::~MobileIPForeignAgent() { };

int MobileIPForeignAgent::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("PRIVATE_ADDRESS", this->privateAddress)
		.read_m("PUBLIC_ADDRESS", this->publicAddress)
		.read_m("HA_ADDRESS",this->HAAddress)
		.execute() < 0){
			return -1;
		}

		return 0;
}

int MobileIPForeignAgent::initialize(ErrorHandler *) {
		/*
    advertisementTimer.initialize(this);
    advertisementTimer.schedule_now();

    return 0;
		*/

	return 0;
}

void MobileIPForeignAgent::add_handlers(){
		//add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
}

void MobileIPForeignAgent::push(int port, Packet *p) {
	// Registration
	getPacketType(p);
	output(0).push(p);

	// SOLICITATION
	WritablePacket* s = buildRouterSolicitationMessage();
	ICMPIPfy(s, IPAddress("192.1.11.1"), IPAddress("192.1.11.1"), 1);
	getPacketType(s);
	output(0).push(s);

	// ADVERTISEMENT
	WritablePacket* a = buildRouterAdvertisementMessage(10,100, IPAddress("192.1.11.1"),5,false, true);
	ICMPIPfy(a, IPAddress("192.1.11.1"), IPAddress("192.1.11.1"), 1);
	getPacketType(a);
	output(0).push(a);

	// Reply
	WritablePacket* r = buildRegistrationReplyPacket(10,1,IPAddress("192.1.11.1"), IPAddress("192.1.11.1"));
	UDPIPfy(r, IPAddress("192.1.11.1"), 434, IPAddress("192.1.11.1"), 1850, 1);
	getPacketType(r);
	output(0).push(r);

	Packet* i = buildTunnelIPPacket(r->clone(), IPAddress("192.1.11.1"), IPAddress("192.1.11.1"));
	getPacketType(i);
	output(0).push(i);


};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
