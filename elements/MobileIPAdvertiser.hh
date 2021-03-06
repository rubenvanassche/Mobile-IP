#ifndef CLICK_MOBILEIPADVERTISER__HH
#define CLICK_MOBILEIPADVERTISER__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"

CLICK_DECLS

/**
 * Accepts ICMP Router Solicitations and sends ICMP Router Advertisments
 * input 0 -> expects router Solicitations
 * output 1 -> sends Router Solicitations
 */
class MobileIPAdvertiser : public Element {
	public:
		MobileIPAdvertiser();
		~MobileIPAdvertiser();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);

		// HANDLER for changing the lifetime
		static int changeLifetimeHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		void add_handlers();

		const char *class_name() const { return "MobileIPAdvertiser"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);

		// Send to multicast address
		bool sendAdvertisement();
		// Send to specified unicast address
		bool sendAdvertisement(IPAddress destination);

		// Reset the lifetime Timer
		void resetAdvertisementTimer(int seconds);

		void run_timer(Timer *timer);

		bool isHomeAgent = false;
		bool isForeignAgent = false;

		unsigned int sequenceNumber = 0;
		unsigned int lifetime = 45; // From RFC 1256
		unsigned int registrationLifetime = 60;

		IPAddress linkAddress;
		IPAddress careOfAddress; // Only when FA

		Timer advertisementTimer;
};

CLICK_ENDDECLS

#endif
