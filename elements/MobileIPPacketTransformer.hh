#ifndef CLICK_MOBILEIPPACKETTRANSFORMER__HH
#define CLICK_MOBILEIPPACKETTRANSFORMER__HH

#include <click/element.hh>
#include "MobileIPSoliciter.hh"

CLICK_DECLS

class MobileIPPacketTransformer : public Element {
	public:

		int configure(Vector<String> &conf, ErrorHandler *errh);
		MobileIPPacketTransformer();
		~MobileIPPacketTransformer();

		const char *class_name() const { return "MobileIPPacketTransformer"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);

		MobileIPSoliciter* soliciter;
};

CLICK_ENDDECLS

#endif
