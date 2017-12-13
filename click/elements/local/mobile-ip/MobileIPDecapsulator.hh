#ifndef CLICK_MOBILEIPDECAPSULATOR__HH
#define CLICK_MOBILEIPDECAPSULATOR__HH

#include <click/element.hh>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "MobileIPForeignAgent.hh"

CLICK_DECLS

// Input 0 -> all possible packets
// Output 0 -> all packages not tunneled
// Output 1 -> packages tunneled to this FA
class MobileIPDecapsulator : public Element {
	public:
		MobileIPDecapsulator();
		~MobileIPDecapsulator();

		int configure(Vector<String> &conf, ErrorHandler *errh);

		const char *class_name() const { return "MobileIPDecapsulator"; }
		const char *port_count() const { return "1/2"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);

		MobileIPForeignAgent* FA;
};

CLICK_ENDDECLS

#endif
