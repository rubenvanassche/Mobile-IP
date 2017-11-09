AddressInfo@1 :: AddressInfo(mobile_node_address 192.168.2.1/24 tap0:eth); AddressInfo@2 :: AddressInfo(home_agent_private_address 192.168.2.254/24 tap1:eth); AddressInfo@3 :: AddressInfo(home_agent_public_address 192.168.0.2/24 tap2:eth); AddressInfo@4 :: AddressInfo(corresponding_node_address 192.168.0.1/24 tap3:eth); AddressInfo@5 :: AddressInfo(foreign_agent_public_address 192.168.0.3/24 tap4:eth); AddressInfo@6 :: AddressInfo(foreign_agent_private_address 192.168.3.254/24 tap5:eth); FromHost@8 :: FromHost(tap0); ToHost@9 :: ToHost(tap0); ICMPPingResponder@10 :: ICMPPingResponder; EtherEncap@11 :: EtherEncap(0x0800, mobile_node_address:eth, mobile_node_address:eth); mobile_node/mip_class :: IPClassifier(icmp type 9, src udp port 434, -); mobile_node/ip :: Strip(14); mobile_node/CheckIPHeader@3 :: CheckIPHeader; mobile_node/rt :: LinearIPLookup(    mobile_node_address:ip/32 0,    mobile_node_address:ipnet 1,    0.0.0.0/0 home_agent_private_address 1); mobile_node/ipgw :: IPGWOptions(mobile_node_address); mobile_node/FixIPSrc@6 :: FixIPSrc(mobile_node_address); mobile_node/ttl :: DecIPTTL; mobile_node/frag :: IPFragmenter(1500); mobile_node/arpq :: ARPQuerier(mobile_node_address); mobile_node/ICMPError@10 :: ICMPError(mobile_node_address, parameterproblem); mobile_node/ICMPError@11 :: ICMPError(mobile_node_address, timeexceeded); mobile_node/ICMPError@12 :: ICMPError(mobile_node_address, unreachable, needfrag); mobile_node/HostEtherFilter@13 :: HostEtherFilter(mobile_node_address); mobile_node/in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800); mobile_node/arp_res :: ARPResponder(mobile_node_address); mobile_node/tee :: Tee(2); mobile_node/AgentSolicitationSender@17 :: AgentSolicitationSender; mobile_node/IPEncap@18 :: IPEncap(PROTO icmp, SRC mobile_node_address, DST 255.255.255.255, TTL 1); mobile_node/EtherEncap@19 :: EtherEncap(0x0800, mobile_node_address, ff:ff:ff:ff:ff:ff); mobile_node/mobile_node :: MobileNode(mobile_node_address, home_agent_private_address, home_agent_public_address, rt, arpq); mobile_node/CheckUDPHeader@21 :: CheckUDPHeader; mobile_node/mip_class [2] -> mobile_node/rt     -> ICMPPingResponder@10     -> EtherEncap@11     -> mobile_node/HostEtherFilter@13     -> mobile_node/in_cl     -> mobile_node/arp_res     -> ToHost@9; mobile_node/rt [1] -> mobile_node/ipgw     -> mobile_node/FixIPSrc@6     -> mobile_node/ttl     -> mobile_node/frag     -> mobile_node/arpq     -> ToHost@9; mobile_node/ipgw [1] -> mobile_node/ICMPError@10     -> ToHost@9; mobile_node/ttl [1] -> mobile_node/ICMPError@11     -> ToHost@9; mobile_node/frag [1] -> mobile_node/ICMPError@12     -> ToHost@9; mobile_node/in_cl [1] -> [1] mobile_node/arpq; mobile_node/in_cl [2] -> mobile_node/ip     -> mobile_node/CheckIPHeader@3     -> mobile_node/mip_class     -> mobile_node/tee     -> mobile_node/AgentSolicitationSender@17     -> mobile_node/IPEncap@18     -> mobile_node/EtherEncap@19     -> ToHost@9; mobile_node/tee [1] -> [1] mobile_node/mobile_node; mobile_node/mip_class [1] -> mobile_node/CheckUDPHeader@21     -> mobile_node/mobile_node     -> mobile_node/arpq; mobile_node/mobile_node [1] -> mobile_node/rt; FromHost@8 -> mobile_node/HostEtherFilter@13; 