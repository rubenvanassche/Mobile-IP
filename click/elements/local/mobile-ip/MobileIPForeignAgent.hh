#ifndef CLICK_MOBILEIPFOREIGNAGENT__HH
#define CLICK_MOBILEIPFOREIGNAGENT__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
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
	//TODO: Add private attributes

	public:
		MobileIPForeignAgent();
		~MobileIPForeignAgent();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);
		void add_handlers();

		const char *class_name() const { return "MobileIPForeignAgent"; }
		const char *port_count() const { return "2/2"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		void run_timer(Timer *timer);

		// Send an reply to an request that has been in the requests list too long
		void sendRequestTimedOutReply(std::vector<RequestListItem> requests);

		// Send an reply to an request when there are too many requets in the requets list
		void sendTooManyRegistrationsReply(registrationRequest registration);

		// Send an reply to an request when the reserved fields are not ZeroChecksumException
		void sendIncorrectReservedFieldsReply(registrationRequest registration);

		// Send an reply to an request when the care of address is not provided by this FA
		void sendCareOfAddressNotProvidedReply(registrationRequest registration);

		// Checks wheter the registration request packet is valid
		bool checkRegistrationValidity(registrationRequest registration);

		IPAddress publicAddress; // The public network address
		IPAddress privateAddress; // The private network address
		IPAddress careOfAddress; // The care of address provided by this FA
		IPAddress HAAddress; // The address of the HA

		unsigned int maxPendingRegistrations = 5; // The maximal amount of registrations
		unsigned int maxAcceptedLifetime = 1800; // The maximal lifetime this FA is accepting

		// The timer tracking the current requests
		Timer requestsTimer;

		// The requests that are waiting for an reply
		RequestList requests;

		// The visitor list
		VisitorList visitors;
};

CLICK_ENDDECLS

#endif
