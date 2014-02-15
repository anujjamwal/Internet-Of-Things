/*
 * internet.h
 *
 *  Created on: Jan 20, 2014
 *      Author: TheuCWorld
 */
#include "ipcommons.h"
#include "utils.h"

#ifndef INTERNET_H_
#define INTERNET_H_

typedef struct {
	unsigned char DestAddrs[6];
	unsigned char SrcAddrs[6];
	unsigned int type;
} EthernetHeader;

typedef struct {
	EthernetHeader eth;
	unsigned int hardware;
	unsigned int protocol;
	unsigned char hardwareSize;
	unsigned char protocolSize;
	unsigned int opCode;
	DeviceAddress sender;
	DeviceAddress target;
} ARPPacket;

typedef struct {
	EthernetHeader eth;
	unsigned char hdrlen : 4;
	unsigned char version : 4;
	unsigned char diffsf;
	unsigned int len;
	unsigned int ident;
	unsigned int fragmentOffset1: 5;
	unsigned int flags : 3;
	unsigned int fragmentOffset2 : 8;
	unsigned char ttl;
	unsigned char protocol;
	unsigned int chksum;
	unsigned char source[4];
	unsigned char dest[4];
} IPHeader;
// IP protocols
#define ICMPPROTOCOL 0x1
#define UDPPROTOCOL 0x11
#define TCPPROTOCOL 0x6

typedef struct {
  IPHeader ip;
  unsigned char type;
  unsigned char code;
  unsigned int chksum;
  unsigned int iden;
  unsigned int seqNum;
  unsigned char * data;
}ICMPHeader;

#define ICMPREPLY 0x0
#define ICMPREQUEST 0x8

typedef struct {
	IPHeader ip;
  unsigned int sourcePort;
  unsigned int destPort;
  unsigned char seqNo[4];
  unsigned char ackNo[4];
  unsigned char NS:1;
  unsigned char reserverd : 3;
  unsigned char hdrLen : 4;
  unsigned char FIN:1;
  unsigned char SYN:1;
  unsigned char RST:1;
  unsigned char PSH:1;
  unsigned char ACK:1;
  unsigned char URG:1;
  unsigned char ECE:1;
  unsigned char CWR:1;
  unsigned int wndSize;
  unsigned int chksum;
  unsigned int urgentPointer;
  //unsigned char options[8];
}TCPHeader;

typedef struct {
  IPHeader ip;
  unsigned int sourcePort;
  unsigned int destPort;
  unsigned int len;
  unsigned int chksum;
}UDPHeader;

typedef struct
{
  UDPHeader udp;
  unsigned int id;
  unsigned int flags;
//  unsigned char QR : 1;
//  unsigned char opCode :4;
//  unsigned char AA:1;
//  unsigned char TC:1;
//  unsigned char RD:1;
//  unsigned char RA:1;
//  unsigned char Zero:3;
//  unsigned char Rcode:4;
  unsigned int qdCount;
  unsigned int anCount;
  unsigned int nsCount;
  unsigned int arCount;
}DNSHeader;

#define DNSUDPPORT 53
#define DNSQUERY 0
#define DNSREPLY 1



void handle_request(unsigned char * packet);
void ip_to_mac(unsigned char *targetIp, void (*handler)(DeviceAddress *address));
void send_ping(unsigned char *targetIp, void (*handler)(boolean result));
void udp_callback(void (*handler)(DeviceAddress * address, unsigned char* data, int len));
void tcp_callback(void (*handler)(DeviceAddress * address, unsigned char* data, int len));

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
