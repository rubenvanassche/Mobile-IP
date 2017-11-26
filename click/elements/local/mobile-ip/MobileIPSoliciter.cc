#include <click/config.h>
#include "MobileIPSoliciter.hh"

CLICK_DECLS

MobileIPSoliciter::MobileIPSoliciter()  : lifetimeTimer(this), solicitationTimer(this) { };
MobileIPSoliciter::~MobileIPSoliciter() { };

int MobileIPSoliciter::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("LINK_ADDRESS", this->linkAddress)
		.read_m("MN",ElementCastArg("MobileIPNode"), this->MN)
		.execute() < 0){
			return -1;
		}

		return 0;
}

int MobileIPSoliciter::initialize(ErrorHandler *) {
    lifetimeTimer.initialize(this);
		solicitationTimer.initialize(this);

    return 0;
}
void MobileIPSoliciter::run_timer(Timer *timer) {
		if(timer == &lifetimeTimer){
			this->move();
			return;
		}

		if(timer == &solicitationTimer){
			this->sendSolicitation();
			return;
		}

}

void MobileIPSoliciter::raiseLifetime(int seconds){
	this->lifetimeTimer.schedule_after_sec(seconds);
}

void MobileIPSoliciter::move(){
	this->connected = false;
	this->routerAddress = IPAddress();

	click_chatter("Node was moved");
}

Packet *MobileIPSoliciter::simple_action(Packet *p) {
	if(getPacketType(p) != ADVERTISEMENT){
		// TODO Send broadcast packets through to the link so other systems of the router can use it
		return NULL;
	}

	routerAdvertisement advertisement = processRouterAdvertisementMessage(p);

	if(this->connected == false){
		this->routerAddress = advertisement.IP.source;
		this->connected = true;
		this->raiseLifetime(advertisement.lifetime);

		// TODO: Register using Mobile IP
		this->MN->reregister(advertisement.IP.source, advertisement.lifetime);


		return NULL;
	}

	if(this->routerAddress == advertisement.IP.source){
		this->raiseLifetime(advertisement.lifetime);
		return NULL;
	}

	return NULL;
};

bool MobileIPSoliciter::sendSolicitation(){
	this->sendSolicitation(IPAddress("255.255.255.255"));
}

bool MobileIPSoliciter::sendSolicitation(IPAddress destination){
	WritablePacket* packet = buildRouterSolicitationMessage();
	ICMPIPfy(packet, this->linkAddress, destination, 1);

	output(0).push(packet);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPSoliciter)
