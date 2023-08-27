
#include "network.h"
#include "enc28j60.h"
#include "stm32f1xx_hal.h"
#include "arp.h"
#include "ip.h"
#include "tcp.h"

extern volatile ethernetDriver_t ethData;
uint8_t data[42];

void Net_Init(void){
	ETH_Init();
	IPV4_Init();
	TCP_Init();
}

//void Net_Run(void){
//	ARPV4_Request(toIP(169, 254, 154, 80));
//	ETH_EventHandler();
//	if(ETH_PendPacket()){
//		ETH_NextPacketUpdate();
//	    ETH_ReadBlock(data,42);
//	    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
//		ETH_DecPacket();
//		ETH_Flush();		
//		ethData.pktReady = false;
//	}
//	if(data[21] == 0x02){
//		while(1){
//			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
//			HAL_Delay(1000);
//		}
//	}
//	HAL_Delay(10000);
//	
//}

void Net_Run(void){
	ETH_EventHandler();
	Net_Read();
}

void Net_Read(void){
    ethernetFrame_t header;

    if(ETH_packetReady())
    {
        ETH_NextPacketUpdate();
        ETH_ReadBlock((char *)&header, sizeof(header));
        header.id.type = ntohs(header.id.type); // reverse the type field
        switch (header.id.type)
        {
            case ETHERTYPE_VLAN:
                break;
            case ETHERTYPE_ARP:  
//				LEDBLINK(1000);
                ARPV4_Packet();
                break;
            case ETHERTYPE_IPV4:
                IPV4_Packet();
                break;
            case ETHERTYPE_IPV6:
                break;
            default:
                break;
        }        
        ETH_Flush();
    }	
}