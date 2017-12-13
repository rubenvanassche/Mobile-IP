#ifndef CLICK_MOBILEIPENCAPSULATOR__HH
#define CLICK_MOBILEIPENCAPSULATOR__HH

#include <click/element.hh>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "MobileIPHomeAgent.hh"

CLICK_DECLS

// Input 0 -> all packages
// Output 0 -> all packages not tunneled
// Output 1 -> packages tunneled, that should be send on the public network
class MobileIPEncapsulator : public Element {
	public:
		MobileIPEncapsulator();
		~MobileIPEncapsulator();

		int configure(Vector<String> &conf, ErrorHandler *errh);

		const char *class_name() const { return "MobileIPEncapsulator"; }
		const char *port_count() const { return "1/2"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		MobileIPHomeAgent* HA;
};

CLICK_ENDDECLS

#endif
