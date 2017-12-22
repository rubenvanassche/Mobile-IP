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

	// Mobile IP
	mipnode :: MobileIPNode(HOME_ADDRESS $address:ip, HA_PRIVATE_ADDRESS $gateway:ip, HA_PUBLIC_ADDRESS $home_agent:ip);
	mipsoliciter :: MobileIPSoliciter(LINK_ADDRESS $address:ip, MN mipnode);
	miptransformer :: MobileIPPacketTransformer(mipsoliciter);

	// Shared IP input path
	ip :: Strip(14)
		-> CheckIPHeader
		-> rt :: LinearIPLookup(
			$address:ip/32 0,
			$address:ipnet 1,
			0.0.0.0/0 $gateway 1)
		-> [1]output;


	rt[1] ->  ipgw :: IPGWOptions($address)
		-> FixIPSrc($address)
		-> ttl :: DecIPTTL
		-> frag :: IPFragmenter(1500)
		-> EtherEncap(0x800, ff:ff:ff:ff:ff:ff, ff:ff:ff:ff:ff:ff)
		-> ToDump("test.pcap")
		-> Strip(14)
		-> miptransformer
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
		-> mipclass :: MobileIPClassifier(REPLY 1, ADVERTISEMENT 2)
		-> ip;

	// Mobile IP functionality
	mipclass[1] -> Strip(14) -> mipnode;
	mipnode -> arpq; // send registration requests out

	// Get solicitations
	mipsoliciter -> arpq; // ignore solicitations at this time

	mipclass[2] -> mipsoliciter; // adv with other ip's

	miptransformer[1] -> ToDump("test2.pcap") -> output; // Packets ready to go out when conencted with fA

}
