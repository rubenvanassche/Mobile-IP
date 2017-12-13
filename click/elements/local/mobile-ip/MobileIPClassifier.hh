#ifndef CLICK_MOBILEIPCLASSIFIER__HH
#define CLICK_MOBILEIPCLASSIFIER__HH

#include <click/element.hh>
#include "packets/PacketProcessors.hh"

CLICK_DECLS

/**
 * Passes through all packets except MobileIp ones
 *
 * Input 0 -> all
 * Output 0 -> The rest
 * Output 1 -> registration requests
 * Output 2 -> registration replies
 * Output 3 -> IPinIP tunneling
 * Output 4 -> router solicitation
 */
class MobileIPClassifier : public Element {
	public:
		MobileIPClassifier();
		~MobileIPClassifier();

		const char *class_name() const { return "MobileIPClassifier"; }
		const char *port_count() const { return "1/5"; }
		const char *processing() const { return PUSH; }

		void push(int port, Packet *p);
};

CLICK_ENDDECLS

#endif
