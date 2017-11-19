//s :: MobileIPAgent;
//d :: MobileIPNode(HOME_ADDRESS 192.168.1.2, HA_ADDRESS 198.1.32.2);
//s -> d ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap");
//ICMPPingSource(192.168.1.2, 198.1.32.2) -> s;

a :: MobileIPAdvertiser(LINK_ADDRESS 192.165.2.1, HA 1);
a ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap");
Idle -> a;
