s :: MobileIPAgent;
d :: MobileIPNode(HOME_ADDRESS 1.1.2.3, HA_ADDRESS 1.1.2.4);
s -> d ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap");
ICMPPingSource(192.168.1.2, 198.1.32.2) -> s;
