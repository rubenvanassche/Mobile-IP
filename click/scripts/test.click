//s :: MobileIPAgent;
//s -> d ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap");
//ICMPPingSource(192.168.1.2, 198.1.32.2) -> s;

m :: MobileIPNode(HOME_ADDRESS 192.168.1.2, HA_ADDRESS 198.1.32.2);
a :: MobileIPAdvertiser(LINK_ADDRESS 192.165.2.1, HA 1);
s :: MobileIPSoliciter(LINK_ADDRESS 192.165.2.2, MN m);

a ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap") -> Strip(14) -> s;
s ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test1.pcap") -> Strip(14) -> a;

Idle -> m;
m -> Discard;
