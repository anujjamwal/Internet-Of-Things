/*
 * enc28j60.c
 *
 *  Created on: Jan 18, 2014
 *      Author: TheuCWorld
 */

#include "enc28j60.h"
#include "bsp.h"
#include "utils.h"

#define ENC_ACTIVE ENC28J60_CS_0
#define ENC_INACTIVE ENC28J60_CS_1

#define ERRATAFIX   set_bit_field(ECON1, ECON1_TXRST);clr_bit_field(ECON1, ECON1_TXRST);clr_bit_field(EIR, EIR_TXERIF | EIR_TXIF)


static unsigned int nextpckptr;
TXStatus txStatus;
RXStatus ptrRxStatus;

static void reset_soft(void);
static void reset_hard(void);
static void select_bank(unsigned char);
static unsigned char read_eth_reg(unsigned char);
static void write_ctrl_reg(unsigned char, unsigned char);
static void write_phy_reg(unsigned char, unsigned int);
static void set_bit_field(unsigned char, unsigned char);
static void clr_bit_field(unsigned char, unsigned char);
static void read_mac_buffer(unsigned char * bytBuffer,unsigned int byt_length);
static void write_mac_buffer(unsigned char * bytBuffer,unsigned int ui_len);


/** Initialize enc28j60
 *
 * 1) Reset The Chip
 * 2) Setup Buffer Memory for Packet input output
 * 3) Setup Receive Filter. Only unicast for now
 * 4) Setup MAC for packet size and crc padding
 * 5) Set MAC Address for device
 * 6) Enable Packet receive
 *
 */
void enc28j60_init(unsigned char * macAddr)
{
	nextpckptr = RXSTART;
	reset_hard();

	reset_soft();

	select_bank(0);
	while (!(read_eth_reg(ESTAT) & ESTAT_CLKRDY));

	//---Setup Recieve Buffer---
	write_ctrl_reg(ERXSTL,(unsigned char)( RXSTART & 0x00ff));
	write_ctrl_reg(ERXSTH,(unsigned char)((RXSTART & 0xff00)>> 8));

	write_ctrl_reg(ERXNDL,(unsigned char)( RXEND   & 0x00ff));
	write_ctrl_reg(ERXNDH,(unsigned char)((RXEND   & 0xff00)>>8));
	//Rx Read pointer at start
	write_ctrl_reg(ERXRDPTL, (unsigned char)( RXSTART & 0x00ff));
	write_ctrl_reg(ERXRDPTH, (unsigned char)((RXSTART & 0xff00)>> 8));

	//---Setup Transmit Buffer---
	write_ctrl_reg(ETXSTL,(unsigned char)( TXSTART & 0x00ff));
	write_ctrl_reg(ETXSTH,(unsigned char)((TXSTART & 0xff00)>>8));


	select_bank(1);

	//---Setup packet filter---
	// For broadcast packets we allow only ARP packtets
	// All other packets should be unicast only for our mac (MAADR)
	//
	// The pattern to match on is therefore
	// Type     ETH.DST
	// ARP      BROADCAST
	// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	// in binary these poitions are:11 0000 0011 1111
	// This is hex 303F->EPMM0=0x3f,EPMM1=0x30
	write_ctrl_reg(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	write_ctrl_reg(EPMM0, 0x3f);
	write_ctrl_reg(EPMM1, 0x30);
	write_ctrl_reg(EPMCSL,0xf9);
	write_ctrl_reg(EPMCSH,0xf7);


	select_bank(2);

	//---Setup MAC registers---
	write_ctrl_reg(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);  // Enable Receive
	write_ctrl_reg(MACON2, 0x00);
	// enable automatic padding to 60bytes and CRC operations
	set_bit_field(MACON3, MACON3_FRMLNEN | MACON3_TXCRCEN | MACON3_PADCFG0);
	// set inter-frame gap
	write_ctrl_reg(MAIPGL , 0x12);  // non back to back
	write_ctrl_reg(MAIPGH , 0x0C);
	write_ctrl_reg(MABBIPG, 0x12);  // back to back
	// set frame length
	write_ctrl_reg(MAMXFLL, (unsigned char)( MAXFRAMELEN & 0x00ff));
	write_ctrl_reg(MAMXFLH, (unsigned char)((MAXFRAMELEN & 0xff00)>>8));


	select_bank(3);

	//---Setup MAC address---
	write_ctrl_reg(MAADR1,macAddr[0]);
	write_ctrl_reg(MAADR2,macAddr[1]);
	write_ctrl_reg(MAADR3,macAddr[2]);
	write_ctrl_reg(MAADR4,macAddr[3]);
	write_ctrl_reg(MAADR5,macAddr[4]);
	write_ctrl_reg(MAADR6,macAddr[5]);

	// Initialise the PHY registes
	// No Loopback of transmitted frames
	write_phy_reg(PHCON2, PHCON2_HDLDIS);

	/* Enable Interrupts
	 *
	 *  set_bit_field(EIE, EIE_INTIE|EIE_PKTIE);
	 *
	 */

	set_bit_field(ECON1,  ECON1_RXEN);     //Enable the chip for reception of packets
}

boolean enc28j60_write_packet(unsigned char * packet, int len)
{
	volatile unsigned int i;
	  //Control byte meaning use settings in MACON3
	  unsigned char  bytControl = 0x00;

	  select_bank(0);
	  // Set write buffer to point to start of Tx Buffer
	  write_ctrl_reg(EWRPTL,(unsigned char)( TXSTART & 0x00ff));
	  write_ctrl_reg(EWRPTH,(unsigned char)((TXSTART & 0xff00)>>8));
	  // Tell MAC when the end of the packet is
	  write_ctrl_reg(ETXNDL, (unsigned char)( (len+TXSTART) & 0x00ff));
	  write_ctrl_reg(ETXNDH, (unsigned char)(((len+TXSTART) & 0xff00)>>8));

	  // write per packet control byte
	  write_mac_buffer(&bytControl,1);
	  write_mac_buffer(packet, len);

	  clr_bit_field(EIR,EIR_TXIF);
	  set_bit_field(EIE, EIE_TXIE |EIE_INTIE);

	  ERRATAFIX;
	  set_bit_field(ECON1, ECON1_TXRTS);                     // begin transmitting;

	  do
	  {
	  }while (!(read_eth_reg(EIR) & (EIR_TXIF)));             // kill some time. Note: Nice place to block?             // kill some time. Note: Nice place to block?

	  clr_bit_field(ECON1, ECON1_TXRTS);

	  len++; //Adds on control byte
	  write_ctrl_reg(ERDPTL, (unsigned char)( len & 0x00ff));      // Setup the buffer read ptr to read status struc
	  write_ctrl_reg(ERDPTH, (unsigned char)((len & 0xff00)>>8));
	  read_mac_buffer(&txStatus.v[0], 7);

	  if (read_eth_reg(ESTAT) & ESTAT_TXABRT)                // did transmission get interrupted?
	  {
	    if (txStatus.bits.LateCollision)
	    {
	    	clr_bit_field(ECON1, ECON1_TXRTS);
	    	set_bit_field(ECON1, ECON1_TXRTS);

	      clr_bit_field(ESTAT,ESTAT_TXABRT | ESTAT_LATECOL);
	    }
	    clr_bit_field(EIR, EIR_TXERIF | EIR_TXIF);
	    clr_bit_field(ESTAT,ESTAT_TXABRT);

	    return false;                                          // packet transmit failed. Inform calling function
	  }                                                        // calling function may inquire why packet failed by calling [TO DO] function
	  else
	  {
	    return true;                                           // all fan dabby dozy
	  }
}


unsigned int enc28j60_read_packet(unsigned char* packet, unsigned int maxLen)
{
  volatile unsigned int pckLen;
  //volatile RXSTATUS ptrRxStatus;
  volatile unsigned char numPackets;

  select_bank(1);
  // How many packets have been received
  numPackets = read_eth_reg(EPKTCNT);
  if(numPackets == 0)
    return numPackets;    // No full packets received

  select_bank(0);
  //Set read pointer to start of packet
  write_ctrl_reg(ERDPTL,(unsigned char)( nextpckptr & 0x00ff));
  write_ctrl_reg(ERDPTH,(unsigned char)((nextpckptr & 0xff00)>>8));
  // read next packet ptr + 4 status bytes
  read_mac_buffer((unsigned char*)&ptrRxStatus.v[0],6);
  //Set nextpckptr for next call of ReadMAC
  nextpckptr = ptrRxStatus.bits.NextPacket;

  //We take away 4 as that is the CRC checksum and we do not need it
  pckLen=ptrRxStatus.bits.ByteCount - 4;
  if( pckLen > (maxLen-1) ) pckLen = maxLen-1;
  //read the packet
  read_mac_buffer(packet,pckLen);

  //free up ENC memory my adjustng the Rx Read ptr
  //See errata this fixes ERXRDPT as it has to always be odd.
  if ( ((nextpckptr - 1) < RXSTART) || ((nextpckptr-1) > RXEND))
  {
	  write_ctrl_reg(ERXRDPTL, (RXEND & 0x00ff));
	  write_ctrl_reg(ERXRDPTH, ((RXEND & 0xff00) >> 8));
  }
  else
  {
	  write_ctrl_reg(ERXRDPTL, (( nextpckptr ) & 0x00ff ));
	  write_ctrl_reg(ERXRDPTH, ((( nextpckptr )) >> 8 ));
  }
  // decrement packet counter
  set_bit_field(ECON2, ECON2_PKTDEC);

  return pckLen;
}

static void reset_soft(void)
{
	ENC_ACTIVE;
	ENC28J60_WRITE(RESET_OP);
	ENC_INACTIVE;

	__delay_ms(20);
}

static void reset_hard(void)
{
	ENC28J60_RST_0;
	__delay_ms(50);
	ENC28J60_RST_1;
	__delay_ms(50);
}

static void select_bank(unsigned char bank)
{
  if (bank >3)
    return;

  clr_bit_field( ECON1, ECON1_BSEL );
  set_bit_field( ECON1, bank );
}

static unsigned char read_eth_reg(unsigned char bytAddress)
{
  unsigned char bytData;

  ENC_ACTIVE;
  ENC28J60_WRITE(bytAddress);
  bytData = ENC28J60_READ();
  ENC_INACTIVE;

  return bytData;
}

static void write_ctrl_reg(unsigned char bytAddress, unsigned char bytData)
{
  if (bytAddress > 0x1f)
  {
    return;
  }

  bytAddress |= WCR_OP;

  ENC_ACTIVE;
  ENC28J60_WRITE(bytAddress);
  ENC28J60_WRITE(bytData);
  ENC_INACTIVE;
}

static void write_phy_reg(unsigned char address, unsigned int data)
{
  if (address > 0x14)
    return;

  select_bank(2);

  write_ctrl_reg(MIREGADR,address);                   // Write address of Phy reg
  write_ctrl_reg(MIWRL,(unsigned char)data);          // lower phy data
  write_ctrl_reg(MIWRH,((unsigned char)(data >>8)));  // Upper phydata
}

static void set_bit_field(unsigned char bytAddress, unsigned char bytData)
{
  if (bytAddress > 0x1f)
  {
    return;
  }

  bytAddress |= BFS_OP;       // Set opcode

  ENC_ACTIVE;
  ENC28J60_WRITE(bytAddress);    // Tx opcode and address
  ENC28J60_WRITE(bytData);       // Tx data
  ENC_INACTIVE;
}

static void clr_bit_field(unsigned char bytAddress, unsigned char bytData)
{
  if (bytAddress > 0x1f)
  {
    return;
  }

  bytAddress |= BFC_OP;       // Set opcode

  ENC_ACTIVE;
  ENC28J60_WRITE(bytAddress);    // Tx opcode and address
  ENC28J60_WRITE(bytData);       // Tx data
  ENC_INACTIVE;
}

static void read_mac_buffer(unsigned char * bytBuffer, unsigned int byt_length)
{
  volatile unsigned int index;

  ENC_ACTIVE;            // ENC CS low

  ENC28J60_WRITE(RBM_OP);   // Tx opcode
  for(index = 0; index < byt_length; index++)
	  bytBuffer[index] = ENC28J60_READ();
  ENC_INACTIVE;           // release CS
}

static void write_mac_buffer(unsigned char * bytBuffer,unsigned int ui_len)
{
  volatile unsigned int index;

  ENC_ACTIVE;            // ENC CS low

  ENC28J60_WRITE(WBM_OP);   // Tx opcode
  for(index = 0; index < ui_len; index++)
	  ENC28J60_WRITE(bytBuffer[index]);
  ENC_INACTIVE;           // release CS
}
