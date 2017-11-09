// Home or Foreign Agent
// The input/output configuration is as follows:
//
// Input:
//	[0]: packets received on the private network
//	[1]: packets received on the public network
//
// Output:
//	[0]: packets sent to the private network
//	[1]: packets sent to the public network
//	[2]: packets destined for the router itself

elementclass Agent {
	$private_address, $public_address, $gateway |

	adv :: AgentAdvertisementSender($private_address, $public_address, INTERVAL 15, REGLIFETIME 30)
	mobility :: MobilityBinding($public_address, adv)

	mip_class :: IPClassifier(icmp type 10, dst udp port 434, -)

	visitor_class :: IPClassifier(dst udp port 434 and ip len 52, src udp port 434 and ip len 48, ip proto 4, -)
	request_class :: Classifier(28/01)
	reply_class :: Classifier(28/03)

	// Shared IP input path and routing table
	ip :: Strip(14)
		-> CheckIPHeader
		-> mip_class[2]
		-> visitor_class[3]	// at this point, input can only be IP-in-IP or other IP. Output is other IP
		-> mobility[2]
		-> rt :: StaticIPLookup(
					$private_address:ip/32 0,
					$public_address:ip/32 0,
					$private_address:ipnet 1,
					$public_address:ipnet 2,
					0.0.0.0/0 $gateway 2);
	
	// ARP responses are copied to each ARPQuerier and the host.
	arpt :: Tee (2);
	
	// Input and output paths for interface 0
	input
		-> HostEtherFilter($private_address)
		-> private_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> ARPResponder($private_address)
		-> output;

	private_arpq :: ARPQuerier($private_address)
		-> output;

	private_class[1]
		-> arpt
		-> [1]private_arpq;

	private_class[2]
		-> Paint(1)
		-> ip;

	// Input and output paths for interface 1
	input[1]
		-> HostEtherFilter($public_address)
		-> public_class :: Classifier(12/0806 20/0001, 12/0806 20/0002, 12/0800)
		-> ARPResponder($public_address)
		-> [1]output;

	public_arpq :: ARPQuerier($public_address)
		-> [1]output;

	public_class[1]
		-> arpt[1]
		-> [1]public_arpq;

	public_class[2]
		-> Paint(2)
		-> ip;

	// Local delivery
	rt[0]
		-> [2]output
	
	// Forwarding paths per interface
	rt[1]
		-> DropBroadcasts
		-> private_paint :: PaintTee(1)
		-> private_ipgw :: IPGWOptions($private_address)
		-> FixIPSrc($private_address)
		-> private_ttl :: DecIPTTL
		-> private_frag :: IPFragmenter(1500)
		-> private_arpq;
	
	private_paint[1]
		-> ICMPError($private_address, redirect, host)
		-> rt;

	private_ipgw[1]
		-> ICMPError($private_address, parameterproblem)
		-> rt;

	private_ttl[1]
		-> ICMPError($private_address, timeexceeded)
		-> rt;

	private_frag[1]
		-> ICMPError($private_address, unreachable, needfrag)
		-> rt;
	

	rt[2]
		-> DropBroadcasts
		-> public_paint :: PaintTee(2)
		-> public_ipgw :: IPGWOptions($public_address)
		-> FixIPSrc($public_address)
		-> public_ttl :: DecIPTTL
		-> public_frag :: IPFragmenter(1500)
		-> public_arpq;
	
	public_paint[1]
		-> ICMPError($public_address, redirect, host)
		-> rt;

	public_ipgw[1]
		-> ICMPError($public_address, parameterproblem)
		-> rt;

	public_ttl[1]
		-> ICMPError($public_address, timeexceeded)
		-> rt;

	public_frag[1]
		-> ICMPError($public_address, unreachable, needfrag)
		-> rt;
	
	visitors :: VisitorBinding($private_address, $public_address, adv, private_arpq)

	mip_class
		-> adv
		-> IPEncap(PROTO icmp, SRC $private_address, DST 255.255.255.255, TTL 1)
		-> EtherEncap(0x0800, $private_address, ff:ff:ff:ff:ff:ff)
		-> output

	mip_class[1]
		-> CheckUDPHeader
		-> visitor_class
		
	mobility[1]
		-> private_arpq;

	visitor_class
		-> request_class
		-> visitors
		-> public_arpq

	visitors[1]
		-> private_arpq

	visitors[2]
		-> DecIPTTL
		-> private_arpq

	visitors[3]
		-> [1]mobility
		-> public_arpq

	visitor_class[1]
		-> reply_class
		-> [1]visitors

	visitor_class[2]
		-> [2]visitors
}
