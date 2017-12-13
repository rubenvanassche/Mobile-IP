#ifndef CLICK_MOBILEIPNODE__HH
#define CLICK_MOBILEIPNODE__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include <iostream>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "lists/RequestList.hh"

CLICK_DECLS

/**
 * input 0 -> Registration Replies
 * output 0 -> Registration request
 */
class MobileIPNode : public Element {
	public:
		MobileIPNode();
		~MobileIPNode();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		void run_timer(Timer *timer);
		int initialize(ErrorHandler *errh);
		void add_handlers();

		const char *class_name() const { return "MobileIPNode"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		// Process a Reply
		void processReply(registrationReply reply);

		void sendRequest(IPAddress destination, IPAddress careOfAddress, unsigned int lifetime, unsigned int ttl = 1);

		// Reregister a node when the node was moved
		bool reregister(IPAddress address, IPAddress careOfAddress, unsigned int lifetime);

		// Register by Link-Layer protocol
		bool registerLL();

		// Register with a given Foreign Agent
		bool registerFA(IPAddress FAAddress, IPAddress careOfAddress, unsigned int lifetime);

		// Register with the home Agent
		bool registerHA(unsigned int lifetime);

		// Deregister with HA for all-care-of-addresses
		bool deregister();

		// Deregister with FA for all care-of-addresses
		bool deregister(IPAddress FAAddress);

		// Deregister with FA for specified care of address
		bool deregister(IPAddress FAAddress, IPAddress address);

		// Checks if the recieved reply is valid
		bool checkReplyValidity(registrationReply reply);

		// The home address of the MN
		IPAddress homeAddress;
		// The home agent adress of the MN
		IPAddress homeAgentPublicAddress;
		// The home agent adress of the MN on the link
		IPAddress homeAgentPrivateAddress;
		// Port from where the registration request packets get send
		unsigned int sourcePort = 5241;
		// X seconds until an request is timed out and an new request is send
		unsigned int requestTimeout = 10;

		// Connection with agent
		agentConnection connection;

		// Structure for keeping pending registrations
		RequestList requests;

		// Keeps track of the lifetime in pending requests
		Timer requestsTimer;
};

CLICK_ENDDECLS

#endif
