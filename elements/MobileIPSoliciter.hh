#ifndef CLICK_MOBILEIPSOLICITER__HH
#define CLICK_MOBILEIPSOLICITER__HH

#include <click/element.hh>
#include <click/args.hh>
#include <click/timer.hh>
#include <iostream>
#include "packets/PacketBuilders.hh"
#include "packets/PacketProcessors.hh"
#include "MobileIPNode.hh"

CLICK_DECLS

// output 0 -> ICMP Router Solicitations
// input 0 -> ICMP Router Advertisements
class MobileIPSoliciter : public Element {
	//TODO: Add private attributes

	public:
		MobileIPSoliciter();
		~MobileIPSoliciter();

		int configure(Vector<String> &conf, ErrorHandler *errh);
		int initialize(ErrorHandler *errh);

		// HANDLER for changing the solicitation interval
		static int changeSolicitationIntervalHandler(const String &conf, Element *e, void * thunk, ErrorHandler * errh);
		void add_handlers();

		const char *class_name() const { return "MobileIPSoliciter"; }
		const char *port_count() const { return "1/1"; }
		const char *processing() const { return AGNOSTIC; }

		Packet *simple_action(Packet *p);

		// Starts sending solicitaions or stops every x seconds
		void changeSolicitationInterval(unsigned int seconds);

		void run_timer(Timer *timer);

		bool sendSolicitation();
		bool sendSolicitation(IPAddress destination);

		// Raise the lifetime remaining
		void raiseLifetime(int seconds);

		// The node is moved
		void move();

		// Is the node connected to a router?
		bool connected = false;

	private:
		IPAddress linkAddress;
		IPAddress routerAddress;

		// tracks how many seconds remain before the lifetime from the latest router advertisement ends
		Timer lifetimeTimer;
		Timer solicitationTimer;

		// Send solicitations every x seconds, if 0 -> do not send
		unsigned int solicitationInterval = 0;

		// Connection with the Mobile IP node
		MobileIPNode* MN;
};

CLICK_ENDDECLS

#endif
