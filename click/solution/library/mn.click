// Mobile Node
// The input/output configuration is as follows:
//
// Packets for the network are put on output 0
// Packets for the host are put on output 1

// Input:
//	[0]: packets received from the network
//
// Output:
//	[0]: packets sent to the network
//	[1]: packets destined for the host itself


elementclass MobileNode {
	$address, $gateway, $home_agent |

	// 0: Agent Advertisements
	// 1: MIP reply
	// 2: other IP
	mip_class :: IPClassifier(icmp type 9, src udp port 434, -)

	// Shared IP input path
	ip :: Strip(14)
		-> CheckIPHeader
		-> mip_class[2]
		-> rt :: LinearIPLookup(
			$address:ip/32 0,
			$address:ipnet 1,
			0.0.0.0/0 $gateway 1)
		-> [1]output;

	rt[1]	-> ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> arpq :: ARPQuerier($address)
		-> output;

	ipgw[1]	-> ICMPError($address, parameterproblem)
		-> output;

	ttl[1]	-> ICMPError($address, timeexceeded)
		-> output;

	frag[1]	-> ICMPError($address, unreachable, needfrag)
		-> output;

	// incoming packets
	input	-> HostEtherFilter($address)
		-> in_cl :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> arp_res :: ARPResponder($address)
		-> output;

	in_cl[1]
		-> [1]arpq;

	in_cl[2]
		-> ip;

	mip_class
		-> tee :: Tee(2)
		-> AgentSolicitationSender
		-> IPEncap(PROTO icmp, SRC $address, DST 255.255.255.255, TTL 1)
		-> EtherEncap(0x0800, $address, ff:ff:ff:ff:ff:ff)
		-> output;

	mobile_node :: MobileNode($address, $gateway, $home_agent, rt, arpq)

	tee[1]	-> [1]mobile_node

	mip_class[1]
		-> CheckUDPHeader
		-> mobile_node
		-> arpq

	mobile_node[1]
		-> rt
}
