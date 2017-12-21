//mn :: MobileIPNode(HOME_ADDRESS 192.168.1.1, HA_PRIVATE_ADDRESS 192.1.1.1, HA_PUBLIC_ADDRESS 192.168.1.1)
//sol :: MobileIPSoliciter(LINK_ADDRESS 192.168.1.1,MN mn);
adv :: MobileIPAdvertiser(LINK_ADDRESS 192.168.1.1, CAREOF_ADDRESS 192.168.1.1, HA true);
//cl :: MobileIPClassifier(ADVERTISEMENT 1, SOLICITATION 2);

//mn -> Discard;
//Idle -> mn;

//sol -> cl;
//adv -> cl;

//cl -> Discard;

//cl[1]  -> sol;
//Idle -> [1]sol;
//cl[2] -> EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap") -> Strip(14) -> adv;

Idle -> adv;
adv -> ToDump("test.pcap");
