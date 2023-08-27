
#include "arp.h"
#include "ip.h"

extern volatile ethernetDriver_t ethData;
extern uint32_t myip;
extern const mac48Address_t broadcastMAC;
const mac48Address_t tha_address = {0, 0, 0, 0, 0, 0};

struct __attribute__((packed)) {
	mac48Address_t mac_address;
	uint32_t ip_address;
	bool ip_static;
} ARP_TABLE[10];

void ARPV4_Init(void){
	
}

error_msg ARPV4_Request(uint32_t ip_address){
	arpHeader_t arp_packet;
	arp_packet.htype = htons(0x0001);
	arp_packet.ptype = htons(0x0800);
	arp_packet.hlen  = 0x06;
	arp_packet.plen  = 0x04;
	arp_packet.oper  = htons(ARP_REQUEST);
	ETH_GetMAC((uint8_t *)arp_packet.sha.mac_array);
	arp_packet.spa   = myip;
	arp_packet.tha   = tha_address;
	arp_packet.tpa   = htons(ip_address);
	ETH_WriteStart(&broadcastMAC, ETHERTYPE_ARP);
	ETH_WriteBlock(&arp_packet, sizeof(arpHeader_t));
    return ETH_Send();
}

void ARPV4_Packet(void){
    arpHeader_t header;
    uint16_t length;

    length = ETH_ReadBlock((char*)&header,sizeof(arpHeader_t));
    if(length == sizeof(arpHeader_t)){
		if((myip && (myip == header.tpa)) || header.tpa == 0xFFFFFFFF){	
			if(header.oper == ntohs(ARP_REQUEST)){
				ARPV4_AddMAC(header.sha, ntohl(header.spa));
                ETH_WriteStart(&header.sha ,ETHERTYPE_ARP);
				header.tha.s = header.sha.s;
				ETH_GetMAC((uint8_t*)&header.sha.s);
				header.tpa = header.spa;
				header.spa = myip;
				header.oper = htons(ARP_REPLY);
				ETH_WriteBlock((char*)&header,sizeof(header));
				ETH_Send(); // remember this could fail to send.		
			}
			if(header.oper == ntohs(ARP_REPLY)){
				ARPV4_AddMAC(header.sha, ntohl(header.spa));
			}
		}
	}	
}

void ARPV4_AddMAC(mac48Address_t mac_address, uint32_t ip_address){
	static uint8_t maccnt = 0;
	if(ARPV4_Lookup(ip_address) == NULL){
		ARP_TABLE[maccnt].mac_address = mac_address;
		ARP_TABLE[maccnt].ip_address  = ip_address;
		ARP_TABLE[maccnt++].ip_static   = 0;
		if(maccnt == 10) maccnt = 0; 
	}
}

mac48Address_t * ARPV4_Lookup(uint32_t ip_address){
	for(uint8_t maci = 0;maci < 10;maci++){
		if(ARP_TABLE[maci].ip_address == ip_address) 
			return &ARP_TABLE[maci].mac_address;
	}
	return NULL;
}