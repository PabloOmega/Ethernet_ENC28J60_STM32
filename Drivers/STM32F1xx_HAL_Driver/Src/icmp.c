
#include "icmp.h"
#include <stdio.h>
#include <stddef.h>

/**
 * ICMP packet receive
 * @param ipv4_header
 * @return
 */
error_msg ICMP_Receive(ipv4Header_t *ipv4Hdr){
    icmpHeader_t icmpHdr;
    error_msg ret = ERROR_COM;
    ETH_ReadBlock(&icmpHdr, sizeof(icmpHeader_t));
    ETH_SaveRDPT();
    
    switch(ntohs((icmpTypeCodes_t)icmpHdr.typeCode))
    {
        case ECHO_REQUEST:
        {            
            ret = ICMP_EchoReply(&icmpHdr,ipv4Hdr);
        }
        break;
        default:
            break;
    }

    return ret;
}

/**
 * ICMP Packet Start
 * @param icmp_header
 * @param dest_address
 * @param protocol
 * @param payload_length
 * @return
 */

error_msg ICMP_EchoReply(icmpHeader_t *icmpHdr, ipv4Header_t *ipv4Hdr){
    uint16_t cksm =0;
    error_msg ret = ERROR_COM;

    ret = IPv4_Start(ipv4Hdr->srcIpAddress, ipv4Hdr->protocol);
    if(ret == SUCCESS_COM)
    {
        uint16_t icmp_cksm_start;
        uint16_t ipv4PayloadLength = ipv4Hdr->length - sizeof(ipv4Header_t);

        ipv4PayloadLength = ipv4Hdr->length - (uint16_t)(ipv4Hdr->ihl << 2);

        ETH_Write16(ECHO_REPLY);
        ETH_Write16(0); // checksum
        ETH_Write16(ntohs(icmpHdr->identifier));
        ETH_Write16(ntohs(icmpHdr->sequence));
        
        // copy the next N bytes from the RX buffer into the TX buffer
        ret = ETH_Copy(ipv4PayloadLength - sizeof(icmpHeader_t));
        if(ret == SUCCESS_COM) // copy can timeout in heavy network situations like flood ping
        {
            ETH_SaveRDPT();
            // compute a checksum over the ICMP payload
            cksm = sizeof(ethernetFrame_t) + sizeof(ipv4Header_t);
            icmp_cksm_start = sizeof(ethernetFrame_t) + sizeof(ipv4Header_t);
            cksm = ETH_TxComputeChecksum(icmp_cksm_start, ipv4PayloadLength, 0);
            ETH_Insert((char *)&cksm,sizeof(cksm),sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(icmpHeader_t,checksum));
            ret = IPV4_Send(ipv4PayloadLength);
        }
    }
    return ret;
}

