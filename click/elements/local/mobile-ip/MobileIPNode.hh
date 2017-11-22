#ifndef CLICK_MOBILEIPNODE__HH
#define CLICK_MOBILEIPNODE__HH

#include <click/element.hh>
#include <click/args.hh>
#include <iostream>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "lists/RequestList.hh"

CLICK_DECLS

class MobileIPNode : public Element {
	//TODO: Add private attributes

	public:
		MobileIPNode();
		~MobileIPNode();

		int configure(Vector<String> &conf, ErrorHandler *errh);

		const char *class_name() const { return "MobileIPNode"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		Packet *simple_action(Packet *p);

		// Process a Reply
		void processReply(registrationReply reply);

		// Reregister a node when the node was moved
		bool reregister(IPAddress address, unsigned int lifetime);

		// Register by Link-Layer protocol
		bool registerLL();

		// Register with a given Foreign Agent
		bool registerFA(IPAddress FAAddress, unsigned int lifetime);

		// Register with the home Agent
		bool registerHA(unsigned int lifetime);

		// Deregister with home agent for all care-of-addresses
		bool deregister(IPAddress FAAddress);

		// Deregister with home agent for specified care of address
		bool deregister(IPAddress FAAddress, IPAddress address);

		// The home address of the MN
		IPAddress homeAddress;
		// The home agent adress of the MN
		IPAddress homeAgentAddress;
		// The care of adress obtained by the MN
		IPAddress careOfAddress;
		// Port from where the registration request packets get send
		unsigned int sourcePort = 5241;

		// Structure for keeping pending registrations
		RequestList requests;
};

CLICK_ENDDECLS

#endif
