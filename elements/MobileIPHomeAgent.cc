#include <click/config.h>
#include "MobileIPHomeAgent.hh"

CLICK_DECLS

MobileIPHomeAgent::MobileIPHomeAgent() : mobilityBindingListTimer(this) { };
MobileIPHomeAgent::~MobileIPHomeAgent() { };

int MobileIPHomeAgent::configure(Vector<String> &conf, ErrorHandler *errh) {
	if(Args(conf, this, errh)
		.read_m("PRIVATE_ADDRESS", this->privateAddress)
		.read_m("PUBLIC_ADDRESS", this->publicAddress)
		.read_m("FA_ADDRESS",this->FAAddress)
		.execute() < 0){
			return -1;
		}

		return 0;
}

int MobileIPHomeAgent::initialize(ErrorHandler *) {
  mobilityBindingListTimer.initialize(this);
  mobilityBindingListTimer.schedule_now();

	return 0;
}

void MobileIPHomeAgent::run_timer(Timer *timer) {
    assert(timer == &mobilityBindingListTimer);
    Timestamp now = Timestamp::now_steady();

		// Decrease the lifetime by one in the pending requests table
		this->mobilityBindings.decreaseLifetime();

    mobilityBindingListTimer.reschedule_after_sec(1);
}

String MobileIPHomeAgent::getMobilityBindingsHandler(Element *e, void * thunk){
		MobileIPHomeAgent * me = (MobileIPHomeAgent *) e;
		return me->mobilityBindings.print().c_str();
}

void MobileIPHomeAgent::add_handlers(){
		add_read_handler("mobility_bindings", &getMobilityBindingsHandler, (void *)0);
		add_data_handlers("max_lifetime", Handler::OP_READ | Handler::OP_WRITE, &maxAcceptedLifetime);
}

void MobileIPHomeAgent::push(int port, Packet *p) {
	if(getPacketType(p) == REGISTRATION){
		registrationRequest registration;

		try{
			registration = processRegistrationRequestPacket(p);
		}catch(ZeroChecksumException &e){
			click_chatter("Request recieved at HA has an IP checksum 0");
			return;
		}catch(InvalidChecksumException &e){
			click_chatter("Request recieved at HA has an invalid checksum");
			return;
		}

		// Checks if the request recieved is valid
		if(checkRegistrationValidity(registration, port) == false){
			return;
		}

		// Check if the home agent in the request recieved is valid
		if(checkForUnkwownHomeAgent(registration, port) == false){
			return;
		}

		if(registration.lifetime == 0){
			if(registration.careOf == registration.home){
				// Remove all bindings
				this->mobilityBindings.remove(registration.home);
			}else{

				// Remove binding for this careOf address
				this->mobilityBindings.remove(registration.home, registration.careOf);
			}

			// Send a reply to indicate that the request was accpeted to delete
			this->sendReply(registration, 0, port, 0);
		}else{
			// Now let's check the lifetime
			unsigned int lifetime = registration.lifetime;
			if(registration.lifetime > this->maxAcceptedLifetime){
				lifetime = this->maxAcceptedLifetime;
			}

			// Add and send reply
			this->mobilityBindings.add(registration.home, registration.careOf, lifetime);
			this->sendReply(registration, 0, port, lifetime);
		}
	}
};

void MobileIPHomeAgent::sendReply(registrationRequest registration, unsigned int code, unsigned int port){
	this->sendReply(registration, code, port, registration.lifetime);
}

void MobileIPHomeAgent::sendReply(registrationRequest registration, unsigned int code, unsigned int port, unsigned int lifetime){
	IPAddress source = registration.IP.destination;
	IPAddress destination = registration.IP.source;

	// RFC p64
	if(source == IPAddress("255.255.255.255") ){
		if(port == 0){
			source = this->privateAddress;
		}else if(port == 1){
			source == this->publicAddress;
		}
	}

	// If lifetime is 0 -> send directly to private network because MN is over there
	if(lifetime == 0){
		port = 0;
	}

	WritablePacket* packet = buildRegistrationReplyPacket(lifetime, code, registration.home, registration.homeAgent, registration.identification);
	UDPIPfy(packet, source, 434, destination, registration.UDP.sourcePort, 1);

	output(port).push(packet);
}


bool MobileIPHomeAgent::checkForUnkwownHomeAgent(registrationRequest registration, unsigned int port){
	// Get the address on which this home agent advertises itself(depening on the port)
	IPAddress goToAddress;
	if(port == 0){
		goToAddress = this->privateAddress;
	}else if(port == 1){
		goToAddress == this->publicAddress;
	}

	// Check if we've got the correct home agent address
	if(registration.homeAgent == this->publicAddress and (registration.IP.destination == this->privateAddress or registration.IP.destination == this->publicAddress)){
		return true;
	}

	IPAddress source = registration.IP.destination;
	IPAddress destination = registration.IP.source;

	// RFC p64
	if(destination == IPAddress("255.255.255.255") ){
		source = goToAddress;
	}

	WritablePacket* packet = buildRegistrationReplyPacket(this->maxAcceptedLifetime, 136, registration.home, this->publicAddress, registration.identification);
	UDPIPfy(packet, source, 434, destination, registration.UDP.sourcePort, 1);

	output(port).push(packet);

	return false;
}

bool MobileIPHomeAgent::checkRegistrationValidity(registrationRequest registration, unsigned int port){
		// TODO check if send subnet-directed broadcast address and reply p61

		// Check if resuest was recieved on correct port
		if(registration.UDP.destinationPort != 434){
			click_chatter("Reply send to wrong port!");
			return false;
		}

		// check if reserved fields are 0
		if(registration.r != false or registration.x != false){
			this->sendReply(registration, 134, port);
			return false;
		}

		// Check if options in the registration are set that aren't supported
		if(registration.D == true or
				registration.M == true or
				registration.G == true or
				registration.T == true){
					this->sendReply(registration, 128, port);
					return false;
				}

		return true;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPHomeAgent)
