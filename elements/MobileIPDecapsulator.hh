#ifndef CLICK_MOBILEIPDECAPSULATOR__HH
#define CLICK_MOBILEIPDECAPSULATOR__HH

#include <click/element.hh>

CLICK_DECLS

// Input 0 -> all possible packets
// Output 0 -> all packages not tunneled
// Output 1 -> packages tunneled to this FA
class MobileIPDecapsulator : public Element {
	//TODO: Add private attributes

	public:
		MobileIPDecapsulator();
		~MobileIPDecapsulator();

		const char *class_name() const { return "MobileIPDecapsulator"; }
		const char *port_count() const { return "1/2"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
