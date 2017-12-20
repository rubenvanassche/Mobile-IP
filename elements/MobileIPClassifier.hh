#ifndef CLICK_MOBILEIPCLASSIFIER__HH
#define CLICK_MOBILEIPCLASSIFIER__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/hashtable.hh>
#include "packets/PacketProcessors.hh"
#include <map>

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

		int configure(Vector<String> &conf, ErrorHandler *errh);

		const char *class_name() const { return "MobileIPClassifier"; }
		const char *port_count() const { return "1/0-5"; }
		const char *processing() const { return PUSH; }

		// Adds a mapping of packet to port
		bool addMapping(PacketType type, int port);

		void push(int port, Packet *p);
	private:
		std::map<PacketType, int> mappings;
};

CLICK_ENDDECLS

#endif
