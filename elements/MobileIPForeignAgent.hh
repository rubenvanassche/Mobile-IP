#ifndef CLICK_MOBILEIPFOREIGNAGENT__HH
#define CLICK_MOBILEIPFOREIGNAGENT__HH

#include <click/element.hh>

CLICK_DECLS

class MobileIPForeignAgent : public Element {
	//TODO: Add private attributes

	public:
		MobileIPForeignAgent();
		~MobileIPForeignAgent();

		const char *class_name() const { return "MobileIPForeignAgent"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);
};

CLICK_ENDDECLS

#endif
