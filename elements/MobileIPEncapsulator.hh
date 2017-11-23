#ifndef CLICK_MOBILEIPENCAPSULATOR__HH
#define CLICK_MOBILEIPENCAPSULATOR__HH

#include <click/element.hh>

CLICK_DECLS

// Input 0 -> all packages
// Output 0 -> all packages + packages with MN that are not home are now tunneled
class MobileIPEncapsulator : public Element {
	//TODO: Add private attributes

	public:
		MobileIPEncapsulator();
		~MobileIPEncapsulator();

		const char *class_name() const { return "MobileIPEncapsulator"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
