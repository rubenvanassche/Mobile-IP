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

		this->careOfAddress = this->publicAddress;

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
			registrationRequest registration;
			try{
				registration = processRegistrationRequestPacket(p);
			}catch(ZeroChecksumException &e){
				click_chatter("Request recieved at FA has an IP checksum 0");
				return;
			}catch(InvalidChecksumException &e){
				click_chatter("Request recieved at FA has an invalid checksum");
				return;
			}

			// Checks if the request recieved is valid
			if(checkRegistrationValidity(registration) == false){
				return;
			}

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

			this->requests.add(request);


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

void MobileIPForeignAgent::sendTooManyRegistrationsReply(registrationRequest registration){
	WritablePacket* packet = buildRegistrationReplyPacket(registration.lifetime, 66, registration.home, registration.homeAgent);
	UDPIPfy(packet, this->privateAddress, 434, registration.IP.destination, registration.UDP.sourcePort, 1);

	output(0).push(packet);
}

void MobileIPForeignAgent::sendIncorrectReservedFieldsReply(registrationRequest registration){
		WritablePacket* packet = buildRegistrationReplyPacket(registration.lifetime, 70, registration.home, registration.homeAgent);
		UDPIPfy(packet, this->privateAddress, 434, registration.IP.destination, registration.UDP.sourcePort, 1);

		output(0).push(packet);
}

void MobileIPForeignAgent::sendCareOfAddressNotProvidedReply(registrationRequest registration){
		WritablePacket* packet = buildRegistrationReplyPacket(registration.lifetime, 77, registration.home, registration.homeAgent);
		UDPIPfy(packet, this->privateAddress, 434, registration.IP.destination, registration.UDP.sourcePort, 1);

		output(0).push(packet);
}

bool MobileIPForeignAgent::checkRegistrationValidity(registrationRequest registration){
		// TODO Check if there are other devices in the subnet that send this request

		// check if reserved fields are 0
		if(registration.r != false){
			this->sendIncorrectReservedFieldsReply(registration);
			return false;
		}

		// Check if care-of address is offered by this FA
		if(registration.careOf.in_addr() != this->careOfAddress.in_addr()){
			this->sendCareOfAddressNotProvidedReply(registration);
			return false;
		}

		// Check if there are too many pending requests
		if(this->requests.size() >= this->maxPendingRegistrations){
			this->sendTooManyRegistrationsReply(registration);
			return false;
		}

		return true;
}


CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPForeignAgent)
