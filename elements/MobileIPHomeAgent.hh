#ifndef CLICK_MOBILEIPHOMEAGENT__HH
#define CLICK_MOBILEIPHOMEAGENT__HH

#include <click/element.hh>

CLICK_DECLS

class MobileIPHomeAgent : public Element {
	//TODO: Add private attributes

	public:
		MobileIPHomeAgent();
		~MobileIPHomeAgent();

		const char *class_name() const { return "MobileIPHomeAgent"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
