
#include "stdint.h"
#include "mac_address.h"
#include "enc28j60.h"

#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ARP_NAK 10

typedef struct __attribute__((packed))
{
    uint16_t htype;         // Hardware Type
    uint16_t ptype;         // Protocol Type
    uint8_t  hlen;          // Hardware Address Length
    uint8_t  plen;          // Protocol Address Length
    uint16_t oper;          // Operation
    mac48Address_t  sha;    // Sender Hardware Address
    uint32_t spa;           // Sender Protocol Address
    mac48Address_t  tha;    // Target Hardware Address
    uint32_t tpa;           // Target Protocol Address
} arpHeader_t;

error_msg ARPV4_Request(uint32_t value);
void ARPV4_Packet(void);
void ARPV4_AddMAC(mac48Address_t mac_address, uint32_t ip_address);
mac48Address_t * ARPV4_Lookup(uint32_t ip_address);