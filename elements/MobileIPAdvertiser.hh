#ifndef CLICK_MOBILEIPADVERTISER__HH
#define CLICK_MOBILEIPADVERTISER__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include "packets/PacketBuilders.hh"

CLICK_DECLS

/**
 * Accepts ICMP Router Solicitations and sends ICMP Router Advertisments
 * input 0 -> expects router Solicitations
 * output 1 -> sends Router Solicitations
 */
class MobileIPAdvertiser : public Element {
	//TODO: Add private attributes

	public:
		MobileIPAdvertiser();
		~MobileIPAdvertiser();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);

		const char *class_name() const { return "MobileIPAdvertiser"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);

		// Send to multicast address
		bool sendAdvertisement();
		// Send to specified unicast address
		bool sendAdvertisement(IPAddress destination);

		void run_timer(Timer *timer);

		bool isHomeAgent = false;
		bool isForeignAgent = false;

		unsigned int sequenceNumber = 0;
		unsigned int lifetime = 100;
		unsigned int registrationLifetime = 10;

		IPAddress linkAddress;
		IPAddress careOfAddress; // Only when FA

		Timer advertisementTimer;
};

CLICK_ENDDECLS

#endif
