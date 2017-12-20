#include <click/config.h>
#include "MobileIPClassifier.hh"

CLICK_DECLS

MobileIPClassifier::MobileIPClassifier() { };
MobileIPClassifier::~MobileIPClassifier() { };

int MobileIPClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	int registration = -1;
	int reply = -1;
	int ipinip = -1;
	int solicitation = -1;
	int advertisement = -1;

	if(Args(conf, this, errh)
		.read_or_set("REGISTRATION", registration, -1)
		.read_or_set("REPLY", reply, -1)
		.read_or_set("IPINIP", ipinip, -1)
		.read_or_set("SOLICITATION", solicitation, -1)
		.read_or_set("ADVERTISEMENT" ,advertisement, -1)
		.execute() < 0){
			return -1;
		}

		if(registration != -1 and this->addMapping(REGISTRATION, registration) == false){
			click_chatter("Registration could not be mapped");
			return -1;
		}

		if(reply != -1 and this->addMapping(REPLY, reply) == false){
			click_chatter("Reply could not be mapped");
			return -1;
		}

		if(ipinip != -1 and this->addMapping(IPINIP, ipinip) == false){
			click_chatter("IPinIP could not be mapped");
			return -1;
		}

		if(solicitation != -1 and this->addMapping(SOLICITATION, solicitation) == false){
			click_chatter("Solicitation could not be mapped");
			return -1;
		}

		if(advertisement != -1 and this->addMapping(ADVERTISEMENT, advertisement) == false){
			click_chatter("Advertisment could not be mapped");
			return -1;
		}


		return 0;
}

bool MobileIPClassifier::addMapping(PacketType type, int port){
		// Can't map on port 0
		if(port == 0){
			return false;
		}

		// Check if table already has mapping
		for(auto it = this->mappings.begin();it != this->mappings.end();it++){
			if(it->second == port){
				return false;
			}
		}

		this->mappings.emplace(type, port);
		return true;
}

void MobileIPClassifier::push(int port, Packet *p){
	PacketType type = getPacketType(p);

	if(this->mappings.find(type) != this->mappings.end()){
		int outputPort = this->mappings.at(type);
		output(outputPort).push(p);
	}else{
		output(0).push(p);
	}
};

CLICK_ENDDECLS
EXPORT_ELEMENT(MobileIPClassifier)
