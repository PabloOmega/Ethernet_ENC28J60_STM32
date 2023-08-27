
#include "icmp.h"
#include "tcp.h"
#include "arp.h"
#include <stdio.h>
#include <stddef.h>

extern const mac48Address_t broadcastMAC;
extern const mac48Address_t tha_address;

uint32_t myip = MYIP_ADDRESS;
uint32_t subnetMask;
uint32_t hostIp;
uint32_t routerIp;

ipv4Header_t ipv4Header;

uint32_t remoteIpv4Address;

void IPV4_Init(void){
	subnetMask = 0xFFFFFF00;
	//routerIp = htonl(toIP(169, 254, 154, 80));
	routerIp = htonl(toIP(192, 168, 100, 1));
	hostIp = htonl(myip);
}

uint32_t IPV4_GetMyip(void){
	return hostIp;
}

uint16_t IPV4_PseudoHeaderChecksum(uint16_t payloadLen)
{
    ipv4_pseudo_header_t tmp;
    uint8_t len;
    uint32_t cksm = 0;
    uint16_t *v;

    tmp.srcIpAddress  = ipv4Header.srcIpAddress;
    tmp.dstIpAddress  = ipv4Header.dstIpAddress;
    tmp.protocol      = ipv4Header.protocol;
    tmp.z             = 0;
    tmp.length        = payloadLen;

    len = sizeof(tmp);
    len = len >> 1;

    v = (uint16_t *) &tmp;

    while(len)
    {
        cksm += *v;
        len--;
        v++;
    }

    // wrap the checksum
    cksm = (cksm & 0x0FFFF) + (cksm>>16);

    // Return the resulting checksum
    return cksm;
}

error_msg IPV4_Packet(void){
    uint16_t cksm = 0;
    uint16_t length = 0;
    char msg[40];
    uint8_t hdrLen;
    //calculate the IPv4 checksum
    cksm = ETH_RxComputeChecksum(sizeof(ipv4Header_t), 0);
    if (cksm != 0)
    {
        //SYSLOG_log("IP Header wrong cksm",true,LOG_KERN,LOG_DEBUG);
        return IPV4_CHECKSUM_FAILS;
    }

    ETH_ReadBlock((char *)&ipv4Header, sizeof(ipv4Header_t));
    if(ipv4Header.version != 4)
    {
        return IP_WRONG_VERSION; // Incorrect version number
    }

    ipv4Header.dstIpAddress = ntohl(ipv4Header.dstIpAddress);
    ipv4Header.srcIpAddress = ntohl(ipv4Header.srcIpAddress);
	
    if((ipv4Header.dstIpAddress == hostIp) ||( ipv4Header.dstIpAddress == IPV4_BROADCAST))
    {
        ipv4Header.length = ntohs(ipv4Header.length);

        hdrLen = (uint8_t)(ipv4Header.ihl << 2);

        if (ipv4Header.ihl > 5)
        {
            //Skip over the IPv4 Options field
            ETH_Dump((uint16_t)(hdrLen - sizeof(ipv4Header_t)));
        }
		
        switch((ipProtocolNumbers)ipv4Header.protocol)
        {
            case ICMP:
                {
                    // calculate and check the ICMP checksum
                    //SYSLOG_log("IPv4 RX ICMP",true,LOG_KERN,LOG_DEBUG);
                    length = ipv4Header.length - hdrLen;
                    cksm = ETH_RxComputeChecksum(length, 0);

                    if (cksm == 0)
                    {
                        ICMP_Receive(&ipv4Header);
                    }
                    else
                    {
                        //SYSLOG_log(msg,true,LOG_KERN,LOG_DEBUG);
                        return ICMP_CHECKSUM_FAILS;
                    }
                }
				//LEDBLINK(1000);
                break;
            case UDP:
//                // check the UDP header checksum
//                //SYSLOG_log("IPv4 RX UDP",true,LOG_KERN,LOG_DEBUG);
//                length = ipv4Header.length - hdrLen;
//                cksm = IPV4_PseudoHeaderChecksum(length);//Calculate pseudo header checksum
//                cksm = ETH_RxComputeChecksum(length, cksm); //1's complement of pseudo header checksum + 1's complement of UDP header, data
//                UDP_Receive(cksm);
                break;
            case TCP:
                // accept only uni cast TCP packets
                // check the TCP header checksum
                //IPV4_SyslogWrite("rx tcp");
                length = ipv4Header.length - hdrLen;
                cksm = IPV4_PseudoHeaderChecksum(length);
                cksm = ETH_RxComputeChecksum(length, cksm);

                // accept only packets with valid CRC Header
                if (cksm == 0)
                {
                    remoteIpv4Address = ipv4Header.srcIpAddress;
                    TCP_HTTP(remoteIpv4Address,length);
                    //TCP_Recv(remoteIpv4Address, length);
                }else
                {
                    //IPV4_SyslogWrite("rx bad tcp cksm");
                }
                break;
            default:
                ETH_Dump(ipv4Header.length);
                break;
        }
        return SUCCESS_COM;
    }
    else
    {
        return DEST_IP_NOT_MATCHED;
    }
}

error_msg IPv4_Start(uint32_t destAddress, ipProtocolNumbers protocol){
    error_msg ret = ERROR_COM;
    // get the dest mac address
    const mac48Address_t *macAddress;
    uint32_t targetAddress;

    // Check if we have a valid IPadress and if it's different then 127.0.0.1
    if(((IPV4_GetMyip() != 0) || (protocol == UDP))
     && (IPV4_GetMyip() != 0x7F000001))
    {
        if(destAddress != 0xFFFFFFFF) // this is NOT a broadcast message
        {
            if( ((destAddress ^ IPV4_GetMyip()) & subnetMask) == 0)
            {
                targetAddress = destAddress;
            }
            else
            {
                targetAddress = routerIp;
            }
            macAddress = ARPV4_Lookup(targetAddress);
            if(macAddress == NULL)
            {
                ret = ARPV4_Request(targetAddress); // schedule an arp request
                return ret;
            }
        }
        else
        {
            macAddress = &broadcastMAC;
        }
        ret = ETH_WriteStart(macAddress, ETHERTYPE_IPV4);
        if(ret == SUCCESS_COM)
        {
            ETH_Write16(0x4500); // VERSION, IHL, DSCP, ECN
            ETH_Write16(0); // total packet length
            ETH_Write32(0xAA554000); // My IPV4 magic Number..., FLAGS, Fragment Offset
            ETH_Write8(IPv4_TTL); // TTL
            ETH_Write8(protocol); // protocol
            ETH_Write16(0); // checksum. set to zero and overwrite with correct value
            ETH_Write32(IPV4_GetMyip());
            ETH_Write32(destAddress);

            // fill the pseudo header for checksum calculation
            ipv4Header.srcIpAddress = IPV4_GetMyip();
            ipv4Header.dstIpAddress = destAddress;
            ipv4Header.protocol = protocol;
        }
    }
    return ret;
}

error_msg IPV4_Send(uint16_t payloadLength){
    uint16_t totalLength;
    uint16_t cksm;
    error_msg ret;

    totalLength = 20 + payloadLength;
    totalLength = ntohs(totalLength);

    //Insert IPv4 Total Length
    ETH_Insert((char *)&totalLength, 2, sizeof(ethernetFrame_t) + offsetof(ipv4Header_t, length));

    cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t),sizeof(ipv4Header_t),0);
    //Insert Ipv4 Header Checksum
    ETH_Insert((char *)&cksm, 2, sizeof(ethernetFrame_t) + offsetof(ipv4Header_t,headerCksm));
    ret = ETH_Send();

    return ret;
}