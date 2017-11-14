s :: MobileIPAgent;
s ->   EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap");
ICMPPingSource(192.168.1.2, 198.1.32.2) -> s;
