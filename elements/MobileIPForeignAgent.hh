#ifndef CLICK_MOBILEIPFOREIGNAGENT__HH
#define CLICK_MOBILEIPFOREIGNAGENT__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"

CLICK_DECLS

// Input 0 -> private network
// Input 1 -> public network
// Output 0 -> private network
// Output 1 -> public network
class MobileIPForeignAgent : public Element {
	//TODO: Add private attributes

	public:
		MobileIPForeignAgent();
		~MobileIPForeignAgent();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);
		void add_handlers();

		const char *class_name() const { return "MobileIPForeignAgent"; }
		const char *port_count() const { return "2/2"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);

		IPAddress publicAddress; // The public network address
		IPAddress privateAddress; // The private network address
		IPAddress HAAddress; // The address of the HA
};

CLICK_ENDDECLS

#endif
