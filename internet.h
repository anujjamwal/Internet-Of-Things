/*
 * internet.h
 *
 *  Created on: Jan 20, 2014
 *      Author: TheuCWorld
 */
#include "ipcommons.h"

#ifndef INTERNET_H_
#define INTERNET_H_

typedef struct {
	unsigned char DestAddrs[6];
	unsigned char SrcAddrs[6];
	unsigned int type;
} EthernetHeader;

typedef struct
{
	EthernetHeader eth;
	unsigned int hardware;
	unsigned int protocol;
	unsigned char hardwareSize;
	unsigned char protocolSize;
	unsigned int opCode;
	DeviceAddress sender;
	DeviceAddress target;
} ARPPacket;

void handle_request(unsigned char * packet);
void ip_to_mac(unsigned char *targetIp, void (*handler)(DeviceAddress *address));

//Host order to device order.
#define HTONS(x) ((x<<8)|(x>>8))

// Ethernet Header Types.
#define ARPPACKET HTONS(0x0806)
#define IPPACKET HTONS(0x0800)

//ARP opCodes
#define ARPREPLY  HTONS(0x0002)
#define ARPREQUEST HTONS(0x0001)
//ARP hardware types
#define ETHERNET HTONS(0x0001)

#endif /* INTERNET_H_ */
