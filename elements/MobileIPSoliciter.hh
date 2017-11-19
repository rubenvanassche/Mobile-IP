#ifndef CLICK_MOBILEIPSOLICITER__HH
#define CLICK_MOBILEIPSOLICITER__HH

#include <click/element.hh>

CLICK_DECLS

class MobileIPSoliciter : public Element {
	//TODO: Add private attributes

	public:
		MobileIPSoliciter();
		~MobileIPSoliciter();

		const char *class_name() const { return "MobileIPSoliciter"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
