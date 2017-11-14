#ifndef CLICK_MOBILEIPAGENT__HH
#define CLICK_MOBILEIPAGENT__HH

#include <click/element.hh>
#include <iostream>
#include "packets/packets.hh"
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"

CLICK_DECLS

class MobileIPAgent : public Element {
	public:
		MobileIPAgent();
		~MobileIPAgent();

		const char *class_name() const { return "MobileIPAgent"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);
};

CLICK_ENDDECLS

#endif
