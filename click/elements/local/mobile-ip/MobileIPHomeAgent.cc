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
		//std::cout << this->mobilityBindings.print() << std::endl;


    mobilityBindingListTimer.reschedule_after_sec(1);
}

void MobileIPHomeAgent::add_handlers(){
		//add_write_handler("set_lifetime", &changeLifetimeHandler, (void *)0);
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
			this->sendReply(registration, 1, port, 0);
		}else{
			// Now let's check the lifetime
			unsigned int lifetime = registration.lifetime;
			if(registration.lifetime > this->maxAcceptedLifetime){
				lifetime = this->maxAcceptedLifetime;
			}

			// Add and send reply
			this->mobilityBindings.add(registration.home, registration.careOf, lifetime);
			this->sendReply(registration, 1, port, lifetime);
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
	if(destination == IPAddress("255.255.255.255") ){
		if(port == 0){
			source = this->privateAddress;
		}else if(port == 1){
			source == this->publicAddress;
		}
	}

	WritablePacket* packet = buildRegistrationReplyPacket(lifetime, code, registration.home, registration.homeAgent);
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

	WritablePacket* packet = buildRegistrationReplyPacket(this->maxAcceptedLifetime, 136, registration.home, this->publicAddress);
	UDPIPfy(packet, source, 434, destination, registration.UDP.sourcePort, 1);

	output(port).push(packet);

	return false;
}

bool MobileIPHomeAgent::checkRegistrationValidity(registrationRequest registration, unsigned int port){
		// TODO check if send subnet-directed broadcast address and reply p61

		// check if reserved fields are 0
		if(registration.r != false or registration.x != false){
			this->sendReply(registration, 70, port);
			return false;
		}

		// Check if options in the registration are set that aren't supported
		if(registration.B == true or
				registration.D == true or
				registration.M == true or
				registration.G == true or
				registration.T == true){
					this->sendReply(registration, 128, port);
					return false;
				}

		// TODO? When home address is zero return 129 -> p65

		return true;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPHomeAgent)
