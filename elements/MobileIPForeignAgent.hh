#ifndef CLICK_MOBILEIPFOREIGNAGENT__HH
#define CLICK_MOBILEIPFOREIGNAGENT__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include <algorithm>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "lists/RequestList.hh"
#include "lists/VisitorList.hh"

CLICK_DECLS

// Input 0 -> private network - registration requests + solicitations
// Input 1 -> public network
// Output 0 -> private network
// Output 1 -> public network
class MobileIPForeignAgent : public Element {
	public:
		MobileIPForeignAgent();
		~MobileIPForeignAgent();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);

		static String getVisitorsHandler(Element *e, void * thunk);
		void add_handlers();

		const char *class_name() const { return "MobileIPForeignAgent"; }
		const char *port_count() const { return "2/2"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		void run_timer(Timer *timer);

		// Send an reply to an request that has been in the requests list too long
		void sendRequestTimedOutReply(std::vector<RequestListItem> requests);

		// Sends an reply to the MN
		void sendReply(registrationRequest registration, unsigned int code);

		// Sends an reply to the MN with specified lifetime
		void sendReply(registrationRequest registration, unsigned int code, unsigned int lifetime);

		// Sends an reply to the MN based upon an reply recieved from HA
		void sendReply(registrationReply registration, IPAddress destination, unsigned int destinationPort);

		// Sends an reply recieved from the HA to the MN
		void relayReply(registrationReply reply);

		// Sends an request recieved from the MN to the HA
		void relayRequest(registrationRequest registration);

		// Checks wheter the registration request packet is valid
		bool checkRegistrationValidity(registrationRequest registration);

		// Checks wheter the registration reply packet is valid
		bool checkRegistrationValidity(registrationReply registration, RequestListItem request);

		IPAddress publicAddress; // The public network address
		IPAddress privateAddress; // The private network address
		IPAddress careOfAddress; // The care of address provided by this FA
		IPAddress HAAddress; // The address of the HA

		unsigned int maxPendingRegistrations = 5; // The maximal amount of registrations
		unsigned int maxAcceptedLifetime = 1800; // The maximal lifetime this FA is accepting
		unsigned int sourcePort = 3581;
		unsigned int requestTimeout = 7; // After x seconds with no response the request times out and sends an error reply

		// The timer tracking the current requests
		Timer requestsTimer;

		// The requests that are waiting for an reply
		RequestList requests;

		// The visitor list
		VisitorList visitors;
};

CLICK_ENDDECLS

#endif
