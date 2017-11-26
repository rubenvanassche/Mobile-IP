#ifndef CLICK_MOBILEIPENCAPSULATOR__HH
#define CLICK_MOBILEIPENCAPSULATOR__HH

#include <click/element.hh>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "MobileIPHomeAgent.hh"

CLICK_DECLS

// Input 0 -> all packages
// Output 0 -> all packages + packages with MN that are not home are now tunneled
class MobileIPEncapsulator : public Element {
	//TODO: Add private attributes

	public:
		MobileIPEncapsulator();
		~MobileIPEncapsulator();

		int configure(Vector<String> &conf, ErrorHandler *errh);

		const char *class_name() const { return "MobileIPEncapsulator"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);

		MobileIPHomeAgent* HA;
};

CLICK_ENDDECLS

#endif
