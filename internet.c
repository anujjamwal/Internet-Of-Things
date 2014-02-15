/*
 * internet.c
 *
 *  Created on: Jan 20, 2014
 *      Author: TheuCWorld
 */
#include "internet.h"
#include "mac.h"
#include "ipcommons.h"
#include <string.h>

static ARPPacket arpPacket;
static DeviceAddress address;
static void (*arp_response_handler)(DeviceAddress * address);
static void (*udp_handler)(DeviceAddress * address, unsigned char* data, int len);
static void (*tcp_handler)(DeviceAddress * address, unsigned char* data, int len);

static void send_arp_request(unsigned char *targetIp);
static void handle_arp_request(ARPPacket *packet);
static void send_arp_reply(ARPPacket * packet);
void ping_reply(ICMPHeader* ping, unsigned int len);
void handle_udp(UDPHeader* packet);
void handle_tcp(TCPHeader* packet);


void udp_callback(void (*handler)(DeviceAddress * address, unsigned char* data, int len)) {
	udp_handler = handler;
}

void tcp_callback(void (*handler)(DeviceAddress * address, unsigned char* data, int len)) {
	tcp_handler = handler;
}

void handle_request(unsigned char * packet)
{
	EthernetHeader *header = (EthernetHeader*)packet;

	if(header->type == ARPPACKET) {
		handle_arp_request((ARPPacket *)packet);

	} else if( header->type == IPPACKET ) {

		IPHeader *ip = (IPHeader*)packet;
		int len = ip->len + sizeof(EthernetHeader);

		if( ip->protocol == ICMPPROTOCOL ) {
		     ping_reply((ICMPHeader*)packet, len);
		} else if(ip->protocol == UDPPROTOCOL) {
			handle_udp((UDPHeader*)packet);
		} else if(ip->protocol == TCPPROTOCOL) {
			handle_tcp((TCPHeader*)packet);
		}
    }
}

void ip_to_mac(unsigned char *targetIp, void (*handler)(DeviceAddress * address))
{
	arp_response_handler = handler;
	send_arp_request(targetIp);
}

/*
 *  Sending and Responding ARP Requests
 */
static void send_arp_request(unsigned char *targetIp)
{
	memcpy( arpPacket.eth.SrcAddrs, Config.macAddress, 6);
	memset( arpPacket.eth.DestAddrs, 0xff, 6);
	arpPacket.eth.type = ARPPACKET;

	arpPacket.hardware = ETHERNET;
	arpPacket.protocol = IPPACKET;
	arpPacket.hardwareSize = 6;
	arpPacket.protocolSize = 4;
	arpPacket.opCode = ARPREQUEST;
	memset( arpPacket.target.macAddress, 0, 6 );
	memcpy( arpPacket.sender.macAddress, Config.macAddress, 6);
	memcpy( arpPacket.target.ipAddress, targetIp, 4);
	memcpy( arpPacket.sender.ipAddress, Config.ipAddress, 4);

	mac_write((unsigned char*)&arpPacket, sizeof(ARPPacket));
}

static void send_arp_reply(ARPPacket * packet)
{
	memcpy( packet->eth.DestAddrs, packet->eth.SrcAddrs, 6);
	memcpy( packet->eth.SrcAddrs, Config.macAddress, 6);

	packet->opCode = ARPREPLY;
	memcpy( packet->target.macAddress, packet->sender.macAddress, 6 );
	memcpy( packet->sender.macAddress, Config.macAddress, 6);
	memcpy( packet->target.ipAddress, packet->sender.ipAddress, 4);
	memcpy( packet->sender.ipAddress, Config.ipAddress, 4);

	mac_write((unsigned char*)packet,sizeof(ARPPacket));
}

static void handle_arp_request(ARPPacket *packet)
{
	if(packet->opCode == ARPREPLY)
	{
		arp_response_handler(&packet->sender);
	}
	else if(!memcmp(Config.ipAddress, packet->target.ipAddress, 4))
	{
		send_arp_reply(packet);
	}
}

void SetupBasicIPPacket( unsigned char* packet, unsigned char protocol, unsigned char* destIP )
{
  IPHeader* ip = (IPHeader*)packet;

  ip->eth.type = HTONS(IPPACKET);
  memcpy( ip->eth.DestAddrs, Router.macAddress, 6 );
  memcpy( ip->eth.SrcAddrs, Config.macAddress, 6);

  ip->version = 0x4;
  ip->hdrlen = 0x5;
  ip->diffsf = 0;
  ip->ident = 2; //Chosen at random roll of dice
  ip->flags = 0x2;
  ip->fragmentOffset1 = 0x0;
  ip->fragmentOffset2 = 0x0;
  ip->ttl = 128;
  ip->protocol = protocol;
  ip->chksum = 0x0;
  memcpy( ip->source , Config.ipAddress, 4 );
  memcpy( ip->dest, destIP, 4 );
}

void send_ping(unsigned char *targetIp, void (*handler)(boolean result)) {
	handler(true);

	ICMPHeader ping;
	SetupBasicIPPacket( (unsigned char*)&ping, ICMPPROTOCOL, targetIp );

	ping.ip.flags = 0x0;
	ping.type = 0x8;
	ping.code = 0x0;
	ping.chksum = 0x0;
	ping.iden = HTONS(0x1);
	ping.seqNum = HTONS(76);

	ping.chksum = HTONS( ~( chksum(0, ((unsigned char*)&ping) + sizeof(IPHeader ),
	                                    sizeof(ICMPHeader) - sizeof(IPHeader) ) ) );
	ping.ip.len = HTONS(60-sizeof(EthernetHeader));
	ping.ip.chksum = HTONS( ~( chksum( 0, (unsigned char*)&ping + sizeof(EthernetHeader),
	                                  sizeof(IPHeader) - sizeof(EthernetHeader))));

	mac_write( (unsigned char*)&ping, sizeof(ICMPHeader) );
}

void ping_reply(ICMPHeader* ping, unsigned int len)
{
  if ( ping->type == ICMPREQUEST )
  {
    ping->type = ICMPREPLY;
    ping->chksum = 0x0;
    ping->ip.chksum = 0x0;
    //Swap the destination MAC address'
    memcpy( ping->ip.eth.DestAddrs, ping->ip.eth.SrcAddrs, 6 );
    memcpy( ping->ip.eth.SrcAddrs, Config.macAddress, 6 );
    //Swap the destination IP address'
    memcpy( ping->ip.dest, ping->ip.source, 4 );
    memcpy( ping->ip.source, Config.ipAddress, 4 );

    ping->chksum = HTONS( ~( chksum(0, ((unsigned char*) ping) + sizeof(IPHeader ),
                                    len - sizeof(IPHeader) ) ) );
    ping->ip.chksum = HTONS( ~( chksum(0, ((unsigned char*) ping) + sizeof(EthernetHeader),
                                       sizeof(IPHeader) - sizeof(EthernetHeader) ) ) );
    mac_write((unsigned char*) ping, len);
  }
}

void add32(unsigned char *op32, unsigned int op16)
{
  op32[3] += (op16 & 0xff);
  op32[2] += (op16 >> 8);

  if(op32[2] < (op16 >> 8)) {
    ++op32[1];
    if(op32[1] == 0) {
      ++op32[0];
    }
  }


  if(op32[3] < (op16 & 0xff)) {
    ++op32[2];
    if(op32[2] == 0) {
      ++op32[1];
      if(op32[1] == 0) {
	++op32[0];
      }
    }
  }
}

unsigned int ackTcp(TCPHeader* tcp, unsigned int len)
{
  //Zero the checksums
  tcp->chksum = 0x0;
  tcp->ip.chksum = 0x0;
  // First sort out the destination mac and source
  memcpy( tcp->ip.eth.DestAddrs, tcp->ip.eth.SrcAddrs, sizeof(Config.macAddress));
  memcpy( tcp->ip.eth.SrcAddrs, Config.macAddress, sizeof(Config.macAddress));
  // Next flip the ips
  memcpy( tcp->ip.dest, tcp->ip.source, sizeof(Config.ipAddress));
  memcpy( tcp->ip.source, Config.ipAddress, sizeof(Config.ipAddress));
  // Next flip ports
  unsigned int destPort = tcp->destPort;
  tcp->destPort = tcp->sourcePort;
  tcp->sourcePort = destPort;
  // Swap ack and seq
  char ack[4];
  memcpy( ack, tcp->ackNo, sizeof(ack) );
  memcpy( tcp->ackNo, tcp->seqNo, sizeof(ack) );
  memcpy( tcp->seqNo, ack, sizeof(ack) );

  if( tcp->SYN )
  {
    add32( tcp->ackNo, 1 );
  }
  else
  {
    add32( tcp->ackNo, len - sizeof(TCPHeader) );
  }
  tcp->SYN = 0;
  tcp->ACK = 1;
  tcp->hdrLen = (sizeof(TCPHeader)-sizeof(IPHeader))/4;
  len = sizeof(TCPHeader);
  tcp->ip.len = HTONS(len-sizeof(EthernetHeader));

  int pseudochksum = chksum(TCPPROTOCOL+len-sizeof(IPHeader),
                            tcp->ip.source, sizeof(Config.ipAddress)*2);
  tcp->chksum = HTONS(~(chksum(pseudochksum,
                               ((unsigned char*)tcp) + sizeof(IPHeader),
                               len-sizeof(IPHeader))));

  tcp->ip.chksum = HTONS(~(chksum(0,((unsigned char*)tcp) + sizeof(EthernetHeader),
                                  sizeof(IPHeader)-sizeof(EthernetHeader))));
  return len;
}

void handle_udp(UDPHeader* packet) {
	udp_handler(&address, (unsigned char*)packet + sizeof(UDPHeader), packet->len - (sizeof(UDPHeader) - sizeof(IPHeader)));
}

void handle_tcp(TCPHeader* packet) {
	tcp_handler(&address, (unsigned char*)packet + sizeof(IPHeader) + packet->hdrLen,
			packet->ip.len - (sizeof(IPHeader) - sizeof(EthernetHeader)) - packet->hdrLen);
}
