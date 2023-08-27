/**
  enc28j60.h
	
  Company:
    Microchip Technology Inc.

  File Name:
    enc28j60.h

  Summary:
    Brief Description of the file (will placed in a table if using Doc-o-Matic)

  Description:
    This section is for a description of the file.  It should be in complete
    sentences describing the purpose of this file.

 */

/*

©  [2015] Microchip Technology Inc. and its subsidiaries.  You may use this software 
and any derivatives exclusively with Microchip products. 
  
THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS, 
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF 
NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS 
INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE 
IN ANY APPLICATION. 

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL 
OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED 
TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY 
OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S 
TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED 
THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS. 

*/


#ifndef __ENC28J60_H
#define __ENC28J60_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "mac_address.h"

#pragma anon_unions

#define LEDBLINK(delay) while(1){ HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13); HAL_Delay(delay);}

#define byteSwap16(a) ((((uint16_t)a & (uint16_t)0xFF00) >> 8) | (((uint16_t)a & (uint16_t)0x00FF) << 8))
#define byteReverse32(a) ((((uint32_t)a&(uint32_t)0xff000000) >> 24) | \
                          (((uint32_t)a&(uint32_t)0x00ff0000) >>  8) | \
                          (((uint32_t)a&(uint32_t)0x0000ff00) <<  8) | \
                          (((uint32_t)a&(uint32_t)0x000000ff) << 24) )

#define toIP(a, b, c, d) (((uint32_t)d << 24) | \
                          ((uint32_t)c << 16) | \
                          ((uint32_t)b << 8)  | \
                          ((uint32_t)a) )


// host to network & network to host macros
#define htons(a) byteSwap16(a)
#define ntohs(a) byteSwap16(a)
#define htonl(a) byteReverse32(a)
#define ntohl(a) byteReverse32(a)

#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_ARP  0x0806
#define ETHERTYPE_IPV6 0x86DD
#define ETHERTYPE_VLAN 0x8100
#define ETHERTYPE_LLDP 0x88CC

#define SFR_BANK0  0x00
#define SFR_BANK1  0x40
#define SFR_BANK2  0x80
#define SFR_BANK3  0xC0
#define SFR_COMMON 0xE0
#define BANK_MASK  0xE0
#define SFR_MASK   0x1F

#define MAX_TX_PACKET (1518)

#define TXSTART (0x1FFF - MAX_TX_PACKET)
#define TXEND	(0x1FFF)
#define RXSTART (0)
#define RXEND	(TXSTART - 2)

//#define RXSTART							0x0000
//#define RXEND								0x0BFF
//#define TXSTART							0x0C00
//#define TXEND								0x11FF

#define ETH_packetReady() ethData.pktReady
#define ETH_linkCheck()   ethData.up
#define ETH_linkChanged() ethData.linkChange

typedef enum
{
    ERROR_COM =0,
            SUCCESS_COM,
            LINK_NOT_FOUND,
            BUFFER_BUSY,
            TX_LOGIC_NOT_IDLE,
            MAC_NOT_FOUND,
            IP_WRONG_VERSION,
            IPV4_CHECKSUM_FAILS,
            DEST_IP_NOT_MATCHED,
            ICMP_CHECKSUM_FAILS,
            UDP_CHECKSUM_FAILS,
            TCP_CHECKSUM_FAILS,
            DMA_TIMEOUT,
            PORT_NOT_AVAILABLE,
            ARP_IP_NOT_MATCHED,
            BAD_PORT_HTTP
}error_msg;

typedef enum
{
    rcr_inst = 0x00,
    rbm_inst = 0x3A,
    wcr_inst = 0x40,
    wbm_inst = 0x7A,
    bfs_inst = 0x80,
    bfc_inst = 0xa0,
    src_inst = 0xFF
}spi_inst_t;

typedef enum
{
	sfr_bank0 = SFR_BANK0,
	sfr_bank1 = SFR_BANK1,
	sfr_bank2 = SFR_BANK2,
	sfr_bank3 = SFR_BANK3,
	sfr_common = SFR_COMMON
} sfr_bank_t;

typedef enum {
	// bank 0
	J60_ERDPTL    = (SFR_BANK0 | 0x00),
	J60_ERDPTH    = (SFR_BANK0 | 0x01),
	J60_EWRPTL    = (SFR_BANK0 | 0x02),
	J60_EWRPTH    = (SFR_BANK0 | 0x03),
	J60_ETXSTL    = (SFR_BANK0 | 0x04),
	J60_ETXSTH    = (SFR_BANK0 | 0x05),
	J60_ETXNDL    = (SFR_BANK0 | 0x06),
	J60_ETXNDH    = (SFR_BANK0 | 0x07),
	J60_ERXSTL    = (SFR_BANK0 | 0x08),
	J60_ERXSTH    = (SFR_BANK0 | 0x09),
	J60_ERXNDL    = (SFR_BANK0 | 0x0A),
	J60_ERXNDH    = (SFR_BANK0 | 0x0B),
	J60_ERXRDPTL  = (SFR_BANK0 | 0x0C),
	J60_ERXRDPTH  = (SFR_BANK0 | 0x0D),
	J60_ERXWRPTL  = (SFR_BANK0 | 0x0E),
	J60_ERXWRPTH  = (SFR_BANK0 | 0x0F),
	J60_EDMASTL   = (SFR_BANK0 | 0x10),
	J60_EDMASTH   = (SFR_BANK0 | 0x11),
	J60_EDMANDL   = (SFR_BANK0 | 0x12),
	J60_EDMANDH   = (SFR_BANK0 | 0x13),
	J60_EDMADSTL  = (SFR_BANK0 | 0x14),
	J60_EDMADSTH  = (SFR_BANK0 | 0x15),
	J60_EDMACSL   = (SFR_BANK0 | 0x16),
	J60_EDMACSH   = (SFR_BANK0 | 0x17),
	RSRV_018  = (SFR_BANK0 | 0x18),
	RSRV_019  = (SFR_BANK0 | 0x19),
	RSRV_01A  = (SFR_BANK0 | 0x1A),
	// bank 1
	J60_EHT0      = (SFR_BANK1 | 0x00),
	J60_EHT1      = (SFR_BANK1 | 0x01),
	J60_EHT2      = (SFR_BANK1 | 0x02),
	J60_EHT3      = (SFR_BANK1 | 0x03),
	J60_EHT4      = (SFR_BANK1 | 0x04),
	J60_EHT5      = (SFR_BANK1 | 0x05),
	J60_EHT6      = (SFR_BANK1 | 0x06),
	J60_EHT7      = (SFR_BANK1 | 0x07),
	J60_EPMM0     = (SFR_BANK1 | 0x08),
	J60_EPMM1     = (SFR_BANK1 | 0x09),
	J60_EPMM2     = (SFR_BANK1 | 0x0A),
	J60_EPMM3     = (SFR_BANK1 | 0x0B),
	J60_EPMM4     = (SFR_BANK1 | 0x0C),
	J60_EPMM5     = (SFR_BANK1 | 0x0D),
	J60_EPMM6     = (SFR_BANK1 | 0x0E),
	J60_EPMM7     = (SFR_BANK1 | 0x0F),
	J60_EPMCSL    = (SFR_BANK1 | 0x10),
	J60_EPMCSH    = (SFR_BANK1 | 0x11),
	RSRV_112  = (SFR_BANK1 | 0x12),
	RSRV_113  = (SFR_BANK1 | 0x13),
	J60_EPMOL     = (SFR_BANK1 | 0x14),
	J60_EPMOH     = (SFR_BANK1 | 0x15),
	RSRV_116  = (SFR_BANK1 | 0x16),
	RSRV_117  = (SFR_BANK1 | 0x17),
	J60_ERXFCON   = (SFR_BANK1 | 0x18),
	J60_EPKTCNT   = (SFR_BANK1 | 0x19),
	RSRV_11A  = (SFR_BANK1 | 0x1A),
	// bank 2
	J60_MACON1    = (SFR_BANK2 | 0x00),
	RSRV_201  = (SFR_BANK2 | 0x01),
	J60_MACON3    = (SFR_BANK2 | 0x02),
	J60_MACON4    = (SFR_BANK2 | 0x03),
	J60_MABBIPG   = (SFR_BANK2 | 0x04),
	RSRV_205  = (SFR_BANK2 | 0x05),
	J60_MAIPGL    = (SFR_BANK2 | 0x06),
	J60_MAIPGH    = (SFR_BANK2 | 0x07),
	J60_MACLCON1  = (SFR_BANK2 | 0x08),
	J60_MACLCON2  = (SFR_BANK2 | 0x09),
	J60_MAMXFLL   = (SFR_BANK2 | 0x0A),
	J60_MAMXFLH   = (SFR_BANK2 | 0x0B),
	RSRV_20C  = (SFR_BANK2 | 0x0C),
	RSRV_20D  = (SFR_BANK2 | 0x0D),
	RSRV_20E  = (SFR_BANK2 | 0x0E),
	RSRV_20F  = (SFR_BANK2 | 0x0F),
	RSRV_210  = (SFR_BANK2 | 0x10),
	RSRV_211  = (SFR_BANK2 | 0x11),
	J60_MICMD     = (SFR_BANK2 | 0x12),
	RSRV_213  = (SFR_BANK2 | 0x13),
	J60_MIREGADR  = (SFR_BANK2 | 0x14),
	RSRV_215  = (SFR_BANK2 | 0x15),
	J60_MIWRL     = (SFR_BANK2 | 0x16),
	J60_MIWRH     = (SFR_BANK2 | 0x17),
	J60_MIRDL     = (SFR_BANK2 | 0x18),
	J60_MIRDH     = (SFR_BANK2 | 0x19),
	RSRV_21A  = (SFR_BANK2 | 0x1A),
	// bank 3
	J60_MAADR5   = (SFR_BANK3 | 0x00),
	J60_MAADR6   = (SFR_BANK3 | 0x01),
	J60_MAADR3   = (SFR_BANK3 | 0x02),
	J60_MAADR4   = (SFR_BANK3 | 0x03),
	J60_MAADR1   = (SFR_BANK3 | 0x04),
	J60_MAADR2   = (SFR_BANK3 | 0x05),
	J60_EBSTSD   = (SFR_BANK3 | 0x06),
	J60_EBSTCON  = (SFR_BANK3 | 0x07),
	J60_EBSTCSL  = (SFR_BANK3 | 0x08),
	J60_EBSTCSH  = (SFR_BANK3 | 0x09),
	J60_MISTAT   = (SFR_BANK3 | 0x0A),
	RSRV_30B = (SFR_BANK3 | 0x0B),
	RSRV_30C = (SFR_BANK3 | 0x0C),
	RSRV_30D = (SFR_BANK3 | 0x0D),
	RSRV_30E = (SFR_BANK3 | 0x0E),
	RSRV_30F = (SFR_BANK3 | 0x0F),
	RSRV_310 = (SFR_BANK3 | 0x10),
	RSRV_311 = (SFR_BANK3 | 0x11),
	J60_EREVID   = (SFR_BANK3 | 0x12),
	RSRV_313 = (SFR_BANK3 | 0x13),
	RSRV_314 = (SFR_BANK3 | 0x14),
	J60_ECOCON   = (SFR_BANK3 | 0x15),
	RSRV_316 = (SFR_BANK3 | 0x16),
	J60_EFLOCON  = (SFR_BANK3 | 0x17),
	J60_EPAUSL   = (SFR_BANK3 | 0x18),
	J60_EPAUSH   = (SFR_BANK3 | 0x19),
	RSRV_31A = (SFR_BANK3 | 0x1A),
	// the following 5 registers are in all banks
	J60_EIE       = (SFR_COMMON | 0x1B),
	J60_EIR       = (SFR_COMMON | 0x1C),
	J60_ESTAT     = (SFR_COMMON | 0x1D),
	J60_ECON2     = (SFR_COMMON | 0x1E),
	J60_ECON1     = (SFR_COMMON | 0x1F)
} enc28j60_registers_t;

typedef enum {
	J60_PHCON1  = 0x00,
	J60_PHSTAT1 = 0x01,
	J60_PHID1   = 0x02,
	J60_PHID2   = 0x03,
	J60_PHCON2  = 0x10,
	J60_PHSTAT2 = 0x11,
	J60_PHIE    = 0x12,
	J60_PHIR    = 0x13,
	J60_PHLCON  = 0x14 //display receive activity
} enc28j60_phy_registers_t;

typedef union
{
    uint8_t val[4];
    struct
    {
        uint16_t byteCount;

        unsigned longDropEvent:1;
        unsigned  :1;
        unsigned oldCarrierEvent:1;
        unsigned  :1;
        unsigned CRCError:1;
        unsigned lengthCheckError:1;
        unsigned lengthOOR:1;
        unsigned receivedOK:1;
        
        unsigned multicastPacket:1;
        unsigned broadcastPacket:1;
        unsigned dribbleNibble:1;
        unsigned controlFrame:1;
        unsigned pauseControl:1;
        unsigned unknownOP:1;
        unsigned vlan :1;
        unsigned zero :1;
    };
} receiveStatusVector_t;

// select register definitions
typedef union
{
    char val;
    struct
    {
        unsigned RXERIF:1;
        unsigned TXERIF:1;
        unsigned :1;
        unsigned TXIF:1;
        unsigned LINKIF:1;
        unsigned DMAIF:1;
        unsigned PKTIF:1;
        unsigned :1;
    };
} eir_t;


typedef union
{
    unsigned int val;
    struct
    {
        unsigned        :5;
        unsigned PLRITY :1;
        unsigned        :3;
        unsigned DPXSTAT:1;
        unsigned LSTAT  :1;
        unsigned COLSTAT:1;
        unsigned RXSTAT :1;
        unsigned TXSTAT :1;
        unsigned        :2;
        
    };
} phstat2_t;

typedef struct 
{
	unsigned error:1;
	unsigned pktReady:1;
	unsigned up:1;
	unsigned idle:1;
	unsigned linkChange:1;
	unsigned bufferBusy:1;
	unsigned :2;
        uint16_t TXPacketSize;
        uint16_t saveRDPT;
        uint16_t saveWRPT;
} ethernetDriver_t;

typedef struct
{
    uint8_t destinationMAC[6];
    uint8_t sourceMAC[6];
    union
    {
        uint16_t type;  // ethernet 2 frame type, 802.3 length, 802.1Q TPID
        uint16_t length;
        uint16_t tpid;
    }id;
    // if tpid == 0x8100 then TCI structure goes here
    // if tpid != 0x8100, then ethertype/length goes here
    // UP to 1500 Bytes of payload goes here
    // 32 bit checksum goes here
} ethernetFrame_t;

static void ENC28_BankSel(enc28j60_registers_t r);

static uint8_t ENC28_Rcr8(enc28j60_registers_t a);
static uint16_t ENC28_Rcr16(enc28j60_registers_t a);

static void ENC28_Wcr8(enc28j60_registers_t a, uint8_t v);
static void ENC28_Wcr16(enc28j60_registers_t a, uint16_t v);

static void ENC28_Bfs(enc28j60_registers_t a, char bits);
static void ENC28_Bfc(enc28j60_registers_t a, char bits);

static void ENC28_PhyWrite(enc28j60_phy_registers_t a, uint16_t d);
static uint16_t ENC28_PhyRead(enc28j60_phy_registers_t a);

static uint16_t checksumCalculation(uint16_t, uint16_t, uint16_t);


void ETH_Init(void);            // setup the Ethernet and get it running
void ETH_EventHandler(void);    // Manage the MAC events.  Poll this or put it in the ISR
void ETH_NextPacketUpdate(void);    // Update the pointers for the next available RX packets
void ETH_ResetReceiver(void);   // Reset the receiver
void ETH_SendSystemReset(void); // Reset the transmitter

// Read functions for data
uint16_t ETH_ReadBlock(void*, uint16_t); // read a block of data from the MAC
uint8_t ETH_Read8(void);                 // read 1 byte from the MAC
uint16_t ETH_Read16(void);               // read 2 bytes and return them in host order
uint32_t ETH_Read32(void);               // read 4 bytes and return them in host order
void ETH_Dump(uint16_t);                 // drop N bytes from a packet (data is lost)
void ETH_Flush(void);                    // drop the rest of this packet and release the buffer

error_msg ETH_WriteStart(const mac48Address_t *dest_mac, uint16_t type);
uint16_t ETH_WriteString(const char *string);                      // write a string of data into the MAC
uint16_t ETH_WriteBlock(void *, uint16_t);                         // write a block of data into the MAC
void ETH_Write8(uint8_t);                                          // write a byte into the MAC
void ETH_Write16(uint16_t);                                        // write 2 bytes into the MAC in Network order
void ETH_Write32(uint32_t);                                        // write 4 bytes into the MAC in Network order
void ETH_Insert(uint8_t *data, uint16_t len, uint16_t offset);     // insert N bytes into a specific offset in the TX packet
error_msg ETH_Copy(uint16_t len);                                      // copy N bytes from saved read location into the current tx location
error_msg ETH_Send(void);                                          // Send the TX packet

uint16_t ETH_TxComputeChecksum(uint16_t position, uint16_t len, uint16_t seed); // compute the checksum of len bytes starting with position.
uint16_t ETH_RxComputeChecksum(uint16_t len, uint16_t seed);

void ETH_GetMAC(uint8_t * mac);            // get the MAC address
void ETH_SetMAC(uint8_t * mac);            // set the MAC address

void ETH_SaveRDPT(void);               // save the receive pointer for copy
uint16_t ETH_GetReadPtr(void);         //returns the value of the read pointer
void ETH_SetReadPtr(uint16_t);         //sets the read pointer to a specific address
uint16_t ETH_GetStatusVectorByteCount(void);
void ETH_SetStatusVectorByteCount(uint16_t);

void ETH_ResetByteCount(void);
uint16_t ETH_GetByteCount(void);

uint16_t ETH_ReadSavedWRPT(void);
void ETH_SaveWRPT(void);

bool ETH_CheckLinkUp(void);

void ETH_TxReset(void);

bool ETH_PendPacket(void);

void ETH_DecPacket(void);


#endif // __ENC28J60_H