
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "enc28j60.h"

//#define MYIP_ADDRESS toIP(169, 254, 154, 82)
#define MYIP_ADDRESS toIP(192, 168, 100, 90)
#define IPV4_BROADCAST 0xFFFFFFFF

#define IPv4_TTL 128

typedef union{
	uint32_t value;
	uint8_t byte[4];
} ip_t;

typedef struct
{
    unsigned    ihl:4;              // internet header length in 32-bit words	
	unsigned    version:4;          // 4 for IPV4
    unsigned    ecn:2;              // Explicit Congestion Notification RFC3168
    unsigned    dscp:6;             // Differentiated Service Code Point RFC3260
    uint16_t    length;                 // total length including header & data (shouldn't be more than 576 octets)
    uint16_t    identifcation;          // ID for packet fragments
    unsigned    fragmentOffsetHigh:5; // offset for a fragment...needed for reassembly
    unsigned    :1;                 // leave this bit zero
    unsigned    dontFragment:1;    // Drop if fragmentation is required to route
    unsigned    moreFragments:1;   // fragments have this bit set (except for the final packet)
    uint8_t     fragmentOffsetLow;        // low byte for the fragment offset
    uint8_t     timeToLive;   // decrement at each hop...discard when zero
    uint8_t     protocol;       // IP Protocol (from RFC790)
    uint16_t    headerCksm;    // RFC1071 defines this calculation
    uint32_t    srcIpAddress;
    uint32_t    dstIpAddress;
    // options could go here if IHL > 5
    // payload goes here
} ipv4Header_t;

// List from RFC5237 http://www.iana.org/assignments/protocol-numbers/protocol-numbers.txt
typedef enum
{
    HOPOPT          =  0,    // IPv6 Hop-by-Hop Option    [RFC2460]
    ICMP            =  1,    // Internet Control Message    [RFC792]
    IGMP            =  2,    // Internet Group Management    [RFC1112]
    GGP             =  3,    // Gateway-to-Gateway    [RFC823]
    IPV4            =  4,    // IPv4 encapsulation    [RFC2003]
    ST              =  5,    // Stream    [RFC1190][RFC1819]
    TCP             =  6,    // Transmission Control    [RFC793]
    CBT             =  7,    // CBT    [Tony_Ballardie]
    EGP             =  8,    // Exterior Gateway Protocol    [RFC888][David_Mills]
    IGP             =  9,    // any private interior gateway (used by Cisco for their IGRP)    [Internet_Assigned_Numbers_Authority]
    BBN_RCC_MON     = 10,    // BBN RCC Monitoring    [Steve_Chipman]
    NVP_II          = 11,    // Network Voice Protocol    [RFC741][Steve_Casner]
    PUP             = 12,    // PUP    [Boggs, D., J. Shoch, E. Taft, and R. Metcalfe, "PUP: An Internetwork Architecture", XEROX Palo Alto Research Center, CSL-79-10, July 1979; also in IEEE Transactions on Communication, Volume COM-28, Number 4, April 1980.][[XEROX]]
    ARGUS           = 13,    // ARGUS    [Robert_W_Scheifler]
    EMCON           = 14,    // EMCON    [<mystery contact>]
    XNET            = 15,    // Cross Net Debugger    [Haverty, J., "XNET Formats for Internet Protocol Version 4", IEN 158, October 1980.][Jack_Haverty]
    CHAOS           = 16,    // Chaos    [J_Noel_Chiappa]
    UDP             = 17,    // User Datagram    [RFC768][Jon_Postel]
    MUX             = 18,    // Multiplexing    [Cohen, D. and J. Postel, "Multiplexing Protocol", IEN 90, USC/Information Sciences Institute, May 1979.][Jon_Postel]
    DCN_MEAS        = 19,    // DCN Measurement Subsystems    [David_Mills]
    HMP             = 20,    // Host Monitoring    [RFC869][Robert_Hinden]
    PRM             = 21,    // Packet Radio Measurement    [Zaw_Sing_Su]
    XNS_IDP         = 22,    // XEROX NS IDP    ["The Ethernet, A Local Area Network: Data Link Layer and Physical Layer Specification", AA-K759B-TK, Digital Equipment Corporation, Maynard, MA. Also as: "The Ethernet - A Local Area Network", Version 1.0, Digital Equipment Corporation, Intel Corporation, Xerox Corporation, September 1980. And: "The Ethernet, A Local Area Network: Data Link Layer and Physical Layer Specifications", Digital, Intel and Xerox, November 1982. And: XEROX, "The Ethernet, A Local Area Network: Data Link Layer and Physical Layer Specification", X3T51/80-50, Xerox Corporation, Stamford, CT., October 1980.][[XEROX]]
    TRUNK_1         = 23,    // Trunk-1    [Barry_Boehm]
    TRUNK_2         = 24,    // Trunk-2    [Barry_Boehm]
    LEAF_1          = 25,    // Leaf-1    [Barry_Boehm]
    LEAF_2          = 26,    // Leaf-2    [Barry_Boehm]
    RDP             = 27,    // Reliable Data Protocol    [RFC908][Robert_Hinden]
    IRTP            = 28,    // Internet Reliable Transaction    [RFC938][Trudy_Miller]
    ISO_TP4         = 29,    // ISO Transport Protocol Class 4    [RFC905][<mystery contact>]
    NETBLT          = 30,    // Bulk Data Transfer Protocol    [RFC969][David_Clark]
    MFE_NSP         = 31,    // MFE Network Services Protocol    [Shuttleworth, B., "A Documentary of MFENet, a National Computer Network", UCRL-52317, Lawrence Livermore Labs, Livermore, California, June 1977.][Barry_Howard]
    MERIT_INP       = 32,    // MERIT Internodal Protocol    [Hans_Werner_Braun]
    DCCP            = 33,    // Datagram Congestion Control Protocol    [RFC4340]
    THREEPC         = 34,    // Third Party Connect Protocol    [Stuart_A_Friedberg]
    IDPR            = 35,    // Inter-Domain Policy Routing Protocol    [Martha_Steenstrup]
    XTP             = 36,    // XTP    [Greg_Chesson]
    DDP             = 37,    // Datagram Delivery Protocol    [Wesley_Craig]
    IDPR_CMTP       = 38,    // IDPR Control Message Transport Proto    [Martha_Steenstrup]
    TPpp            = 39,    // TP++ Transport Protocol    [Dirk_Fromhein]
    IL              = 40,    // IL Transport Protocol    [Dave_Presotto]
    IPV6_TUNNEL     = 41,    // IPv6 encapsulation    [RFC2473]
    SDRP            = 42,    // Source Demand Routing Protocol    [Deborah_Estrin]
    IPV6_Route      = 43,    // Routing Header for IPv6    [Steve_Deering]
    IPV6_Frag       = 44,    // Fragment Header for IPv6    [Steve_Deering]
    IDRP            = 45,    // Inter-Domain Routing Protocol    [Sue_Hares]
    RSVP            = 46,    // Reservation Protocol    [RFC2205][RFC3209][Bob_Braden]
    GRE             = 47,    // Generic Routing Encapsulation    [RFC1701][Tony_Li]
    DSR             = 48,    // Dynamic Source Routing Protocol    [RFC4728]
    BNA             = 49,    // BNA    [Gary Salamon]
    ESP             = 50,    // Encap Security Payload    [RFC4303]
    AH              = 51,    // Authentication Header    [RFC4302]
    I_NLSP          = 52,    // Integrated Net Layer Security TUBA    [K_Robert_Glenn]
    SWIPE           = 53,    // IP with Encryption    [John_Ioannidis]
    NARP            = 54,    // NBMA Address Resolution Protocol    [RFC1735]
    MOBILE          = 55,    // IP Mobility    [Charlie_Perkins]
    TLSP            = 56,    // Transport Layer Security Protocol using Kryptonet key management    [Christer_Oberg]
    SKIP            = 57,    // SKIP    [Tom_Markson]
    IPV6_ICMP       = 58,    // ICMP for IPv6    [RFC2460]
    IPV6_NoNxt      = 59,    // No Next Header for IPv6    [RFC2460]
    IPV6_Opts       = 60,    // Destination Options for IPv6    [RFC2460]
    CFTP            = 62,    // CFTP    [Forsdick, H., "CFTP", Network Message, Bolt Beranek and Newman, January 1982.][Harry_Forsdick]
    SAT_EXPAK       = 64,    // SATNET and Backroom EXPAK    [Steven_Blumenthal]
    KRYPTOLAN       = 65,    // Kryptolan    [Paul Liu]
    RVD             = 66,    // MIT Remote Virtual Disk Protocol    [Michael_Greenwald]
    IPPC            = 67,    // Internet Pluribus Packet Core    [Steven_Blumenthal]
    SAT_MON         = 69,    // SATNET Monitoring    [Steven_Blumenthal]
    VISA            = 70,    // VISA Protocol    [Gene_Tsudik]
    IPCV            = 71,    // Internet Packet Core Utility    [Steven_Blumenthal]
    CPNX            = 72,    // Computer Protocol Network Executive    [David Mittnacht]
    CPHB            = 73,    // Computer Protocol Heart Beat    [David Mittnacht]
    WSN             = 74,    // Wang Span Network    [Victor Dafoulas]
    PVP             = 75,    // Packet Video Protocol    [Steve_Casner]
    BR_SAT_MON      = 76,    // Backroom SATNET Monitoring    [Steven_Blumenthal]
    SUN_ND          = 77,    // SUN ND PROTOCOL-Temporary    [William_Melohn]
    WB_MON          = 78,    // WIDEBAND Monitoring    [Steven_Blumenthal]
    WB_EXPAK        = 79,    // WIDEBAND EXPAK    [Steven_Blumenthal]
    ISO_IP          = 80,    // ISO Internet Protocol    [Marshall_T_Rose]
    VMTP            = 81,    // VMTP    [Dave_Cheriton]
    SECURE_VMTP     = 82,    // SECURE-VMTP    [Dave_Cheriton]
    VINES           = 83,    // VINES    [Brian Horn]
    TTP             = 84,    // TTP    [Jim_Stevens]
    IPTM            = 84,    // Protocol Internet Protocol Traffic Manager    [Jim_Stevens]
    NSFNET_IGP      = 85,    // NSFNET-IGP    [Hans_Werner_Braun]
    DGP             = 86,    // Dissimilar Gateway Protocol    [M/A-COM Government Systems, "Dissimilar Gateway Protocol Specification, Draft Version", Contract no. CS901145, November 16, 1987.][Mike_Little]
    TCF             = 87,    // TCF    [Guillermo_A_Loyola]
    EIGRP           = 88,    // EIGRP    [Cisco Systems, "Gateway Server Reference Manual", Manual Revision B, January 10, 1988.][Guenther_Schreiner]
    OSPFIGP         = 89,    // OSPFIGP    [RFC1583][RFC2328][RFC5340][John_Moy]
    Sprite_RPC      = 90,    // Sprite RPC Protocol    [Welch, B., "The Sprite Remote Procedure Call System", Technical Report, UCB/Computer Science Dept., 86/302, University of California at Berkeley, June 1986.][Bruce Willins]
    LARP            = 91,    // Locus Address Resolution Protocol    [Brian Horn]
    MTP             = 92,    // Multicast Transport Protocol    [Susie_Armstrong]
    AX25            = 93,    // AX.25 Frames    [Brian_Kantor]
    IPIP            = 94,    // IP-within-IP Encapsulation Protocol    [John_Ioannidis]
    MICP            = 95,    // Mobile Internetworking Control Pro.    [John_Ioannidis]
    SCC_SP          = 96,    // Semaphore Communications Sec. Pro.    [Howard_Hart]
    ETHERIP         = 97,    // Ethernet-within-IP Encapsulation    [RFC3378]
    ENCAP           = 98,    // Encapsulation Header    [RFC1241][Robert_Woodburn]
    GMTP            = 100,    // GMTP    [[RXB5]]
    IFMP            = 101,    // Ipsilon Flow Management Protocol    [Bob_Hinden][November 1995, 1997.]
    PNNI            = 102,    // PNNI over IP    [Ross_Callon]
    PIM             = 103,    // Protocol Independent Multicast    [RFC4601][Dino_Farinacci]
    ARIS            = 104,    // ARIS    [Nancy_Feldman]
    SCPS            = 105,    // SCPS    [Robert_Durst]
    QNX             = 106,    // QNX    [Michael_Hunter]
    A_N             = 107,    // Active Networks    [Bob_Braden]
    IPComp          = 108,    // IP Payload Compression Protocol    [RFC2393]
    SNP             = 109,    // Sitara Networks Protocol    [Manickam_R_Sridhar]
    Compaq_Peer     = 110,    // Compaq Peer Protocol    [Victor_Volpe]
    IPX_in_IP       = 111,    // IPX in IP    [CJ_Lee]
    VRRP            = 112,    // Virtual Router Redundancy Protocol    [RFC5798]
//    PGM             = 113,    // PGM Reliable Transport Protocol    [Tony_Speakman]
    L2TP            = 115,    // Layer Two Tunneling Protocol    [RFC3931][Bernard_Aboba]
    DDX             = 116,    // D-II Data Exchange (DDX)    [John_Worley]
    IATP            = 117,    // Interactive Agent Transfer Protocol    [John_Murphy]
    STP             = 118,    // Schedule Transfer Protocol    [Jean_Michel_Pittet]
    SRP             = 119,    // SpectraLink Radio Protocol    [Mark_Hamilton]
    UTI             = 120,    // UTI    [Peter_Lothberg]
//    SMP             = 121,    // Simple Message Protocol    [Leif_Ekblad]
    SM              = 122,    // SM    [Jon_Crowcroft]
    PTP             = 123,    // Performance Transparency Protocol    [Michael_Welzl]
    ISIS            = 124,    //  over IPv4        [Tony_Przygienda]
    FIRE            = 125,    // [Criag_Partridge]
    CRTP            = 126,    // Combat Radio Transport Protocol    [Robert_Sautter]
    CRUDP           = 127,    // Combat Radio User Datagram    [Robert_Sautter]
    SSCOPMCE        = 128,    // [Kurt_Waber]
    IPLT            = 129,    // [[Hollbach]]
    SPS             = 130,    // Secure Packet Shield    [Bill_McIntosh]
    PIPE            = 131,    // Private IP Encapsulation within IP    [Bernhard_Petri]
    SCTP            = 132,    // Stream Control Transmission Protocol    [Randall_R_Stewart]
    FC              = 133     // Fibre Channel    [Murali_Rajagopal][RFC6172]
} ipProtocolNumbers;

// pseudo header used for checksum calculation on UDP and TCP
typedef struct
{
    uint32_t srcIpAddress;
    uint32_t dstIpAddress;
    uint8_t  protocol;
    uint8_t  z; // used to clean the memory
    uint16_t length;
} ipv4_pseudo_header_t;

void IPV4_Init(void);
uint32_t IPV4_GetMyip(void);
uint16_t IPV4_PseudoHeaderChecksum(uint16_t payloadLen);
error_msg IPV4_Packet(void);
error_msg IPv4_Start(uint32_t destAddress, ipProtocolNumbers protocol);
error_msg IPV4_Send(uint16_t payloadLength);
;