#ifndef CLICK_MOBILEIPHOMEAGENT__HH
#define CLICK_MOBILEIPHOMEAGENT__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "lists/MobilityBindingList.hh"

CLICK_DECLS

// Input 0 -> private network
// Input 1 -> public network
// Output 0 -> private network
// Output 1 -> public network
class MobileIPHomeAgent : public Element {
	public:
		MobileIPHomeAgent();
		~MobileIPHomeAgent();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);

		static String getMobilityBindingsHandler(Element *e, void * thunk);
		void add_handlers();

		const char *class_name() const { return "MobileIPHomeAgent"; }
		const char *port_count() const { return "2/2"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		void run_timer(Timer *timer);

		// Sends an reply to the MN
		void sendReply(registrationRequest registration, unsigned int code, unsigned int port);

		// Sends an reply to the MN with specified lifetime
		void sendReply(registrationRequest registration, unsigned int code, unsigned int port, unsigned int lifetime);

		// Checks wheter the registration request packet is valid
		bool checkRegistrationValidity(registrationRequest registration, unsigned int port);

		// Checks wether the home agent field is 0, if so it replies with the correct home agent address
		bool checkForUnkwownHomeAgent(registrationRequest registration, unsigned int port);

		IPAddress publicAddress; // The public network address
		IPAddress privateAddress; // The private network address
		IPAddress FAAddress; // The address of the FA

		unsigned int maxAcceptedLifetime = 1800; // The maximal lifetime this HA is accepting
		unsigned int sourcePort = 5241;

		// Timer that decreases the lifetime i the mobility list
		Timer mobilityBindingListTimer;

		// The Mobility Bindings
		MobilityBindingList mobilityBindings;
};

CLICK_ENDDECLS

#endif
