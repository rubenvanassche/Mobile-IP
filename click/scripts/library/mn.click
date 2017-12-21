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

	// Shared IP input path
	ip :: Strip(14)
		-> CheckIPHeader
		-> rt :: LinearIPLookup(
			$address:ip/32 0,
			$address:ipnet 1,
			255.255.255.255 2,
			0.0.0.0/0 $gateway 1)
		-> [1]output;


	rt[1] ->  ipgw :: IPGWOptions($address)
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
		-> mipclass :: MobileIPClassifier(REPLY 1, ADVERTISEMENT 2)
		-> ip;

	// Mobile IP functionality
	mipnode :: MobileIPNode(HOME_ADDRESS $address:ip, HA_PRIVATE_ADDRESS $gateway:ip, HA_PUBLIC_ADDRESS $home_agent:ip);
	mipclass[1] -> Strip(14) -> mipnode;
	mipnode -> arpq; // send registration requests out

	// Get solicitations
	mipsoliciter :: MobileIPSoliciter(LINK_ADDRESS $address:ip, MN mipnode);
	mipsoliciter -> arpq; // ignore solicitations at this time

	rt[2] -> mipsoliciter; // addresses on 255.255.255.255
	mipclass[2] -> Strip(14) -> [1]mipsoliciter; // adv with other ip's


}
