//s :: MobileIPAgent;
//s -> d ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap");
//ICMPPingSource(192.168.1.2, 198.1.32.2) -> s;

m :: MobileIPNode(HOME_ADDRESS 192.168.1.2, HA_ADDRESS 198.1.32.2);
f :: MobileIPForeignAgent(PUBLIC_ADDRESS 192.168.1.1, PRIVATE_ADDRESS 192.168.1.2, HA_ADDRESS 192.168.2.1);
h :: MobileIPHomeAgent(PUBLIC_ADDRESS 192.168.2.1, PRIVATE_ADDRESS 192.168.2.2, FA_ADDRESS 192.168.1.1);
a :: MobileIPAdvertiser(LINK_ADDRESS 192.165.2.1, HA 1);
s :: MobileIPSoliciter(LINK_ADDRESS 192.165.2.2, MN m);

a ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test.pcap") -> Strip(14) -> s;
s ->  EtherEncap(0x0800, 1:1:1:1:1:1, 2:2:2:2:2:2) -> ToDump("test1.pcap") -> Strip(14) -> a;

f[1] -> [1]h;
h[1] -> [1]f;
m -> [0]f;
f[0] -> m;

h[0] -> Discard;
Idle -> [0]h
