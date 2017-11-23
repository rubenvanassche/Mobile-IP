#include <click/config.h>
#include "MobileIPForeignAgent.hh"

CLICK_DECLS

MobileIPForeignAgent::MobileIPForeignAgent() : requestsTimer(this) { };
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
    requestsTimer.initialize(this);
    requestsTimer.schedule_now();

		return 0;
}

void MobileIPForeignAgent::run_timer(Timer *timer) {
    assert(timer == &requestsTimer);
    Timestamp now = Timestamp::now_steady();

		// Decrease the lifetime by one in the pending requests table
		this->requests.decreaseLifetime();

		// Remove pending requests running longer then 7 seconds
		this->sendRequestTimedOutReply(this->requests.removeTimedOutRequests(7));

    requestsTimer.reschedule_after_sec(1);
}

void MobileIPForeignAgent::add_handlers(){
		//add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
}

void MobileIPForeignAgent::push(int port, Packet *p) {
	// Priavte network
	if(port == 0){
		if(getPacketType(p) == REGISTRATION){
			registrationRequest registration = processRegistrationRequestPacket(p);

			// Create an new entry in the pending requests list
			RequestListItem request;
			request.destination = registration.IP.destination;
			request.careOfAddress = registration.careOf;
			request.requestedLifetime = registration.lifetime;
			request.remainingLifetime = registration.lifetime;
			request.homeAddress = registration.home;
			request.homeAgentAddress = registration.homeAgent;
			request.source = registration.IP.source;
			request.sourcePort = registration.UDP.sourcePort;

			if(this->requests.size() <= this->maxPendingRegistrations){
				this->requests.add(request);
			}else{
				this->sendTooManyRegistrationsReply(request);
			}


		}

		return;
	}

	// Public network
	if(port == 1){

		return;
	}

};

void MobileIPForeignAgent::sendRequestTimedOutReply(std::vector<RequestListItem> requests){
		for(auto request : requests){
				WritablePacket* packet = buildRegistrationReplyPacket(request.requestedLifetime, 78, request.homeAddress, request.homeAgentAddress);
				UDPIPfy(packet, this->privateAddress, 434, request.destination, request.sourcePort, 1);

				output(0).push(packet);
		}
}

void MobileIPForeignAgent::sendTooManyRegistrationsReply(RequestListItem request){
		WritablePacket* packet = buildRegistrationReplyPacket(request.requestedLifetime, 66, request.homeAddress, request.homeAgentAddress);
		UDPIPfy(packet, this->privateAddress, 434, request.destination, request.sourcePort, 1);

		output(0).push(packet);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
