
#include "enc28j60.h"
#include "stdint.h"
#include "ip.h"
#include "mac_address.h"

typedef struct
{
    union
    {
        uint16_t typeCode;
        struct
        {
            uint8_t code;
            uint8_t type;
        };
    };
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;

} icmpHeader_t;

// ICMP Types and Codes
typedef enum
{
    ECHO_REPLY = 0x0000,
    // Destination Unreachable
    DEST_NETWORK_UNREACHABLE = 0x0300,
    DEST_HOST_UNREACHABLE = 0x0301,
    DEST_PROTOCOL_UNREACHABLE = 0x0302,
    DEST_PORT_UNREACHABLE = 0x0303,
    FRAGMENTATION_REQUIRED = 0x0304,
    SOURCE_ROUTE_FAILED = 0x0305,
    DESTINATION_NETWORK_UNKNOWN = 0x0306,
    SOURCE_HOST_ISOLATED = 0x0307,
    NETWORK_ADMINISTRATIVELY_PROHIBITED = 0x0308,
    HOST_ADMINISTRATIVELY_PROHIBITED = 0x0309,
    NETWORK_UNREACHABLE_FOR_TOS = 0x030A,
    HOST_UNREACHABLE_FOR_TOS = 0x030B,
    COMMUNICATION_ADMINISTRATIVELY_PROHIBITED = 0x030C,
    HOST_PRECEDENCE_VIOLATION = 0x030D,
    PRECEDENCE_CUTOFF_IN_EFFECT = 0x030E,
    // Source Quench
    SOURCE_QUENCH = 0x0400,
    // redirect message
    REDIRECT_DATAGRAM_FOR_THE_NETWORK = 0x0500,
    REDIRECT_DATAGRAM_FOR_THE_HOST = 0x0501,
    REDIRECT_DATAGRAM_FOR_THE_TOS_AND_NETWORK = 0x0502,
    REDIRECT_DATAGRAM_FOR_THE_TOS_AND_HOST = 0x0503,
    //
    ALTERNATE_HOST_ADDRESS = 0x0600,
    // Echo Request
    ECHO_REQUEST = 0x0800, // ask for a ping!
    // router advertisement
    ROUTER_ADVERTISEMENT = 0x0900,
    ROUTER_SOLICITATION = 0x0A00,
    TRACEROUTE = 0x3000
} icmpTypeCodes_t;

error_msg ICMP_Receive(ipv4Header_t *ipv4Hdr);
error_msg ICMP_EchoReply(icmpHeader_t *icmpHdr, ipv4Header_t *ipv4Hdr);

;