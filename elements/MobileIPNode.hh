#ifndef CLICK_MOBILEIPNODE__HH
#define CLICK_MOBILEIPNODE__HH

#include <click/element.hh>
#include <iostream>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"

CLICK_DECLS

class MobileIPNode : public Element {
	//TODO: Add private attributes

	public:
		MobileIPNode();
		~MobileIPNode();

		const char *class_name() const { return "MobileIPNode"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		Packet *simple_action(Packet *p);

		// The home address of the MN
		IPAddress homeAddress;
		// The home agent adress of the MN
		IPAddress homeAgentAddress;
		// The care of adress obtained by the MN
		IPAddress careOfAddress;
};

CLICK_ENDDECLS

#endif
