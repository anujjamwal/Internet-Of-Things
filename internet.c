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
static void (*arp_response_handler)(DeviceAddress * address);

static void send_arp_request(unsigned char *targetIp);
static void handle_arp_request(ARPPacket *packet);
static void send_arp_reply(ARPPacket * packet);


void handle_request(unsigned char * packet)
{
	EthernetHeader *header = (EthernetHeader*)packet;

	if(header->type == ARPPACKET)
	{
		handle_arp_request((ARPPacket *)packet);
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
	memcpy( arpPacket.eth.SrcAddrs, Config.macAddress, 6);
	memcpy( arpPacket.eth.DestAddrs, packet->sender.macAddress, 6);
	arpPacket.eth.type = ARPPACKET;

	arpPacket.hardware = ETHERNET;
	arpPacket.protocol = IPPACKET;
	arpPacket.hardwareSize = 6;
	arpPacket.protocolSize = 4;
	arpPacket.opCode = ARPREPLY;
	memcpy( arpPacket.target.macAddress, packet->sender.macAddress, 6 );
	memcpy( arpPacket.sender.macAddress, Config.macAddress, 6);
	memcpy( arpPacket.target.ipAddress, packet->sender.ipAddress, 4);
	memcpy( arpPacket.sender.ipAddress, Config.ipAddress, 4);

	mac_write((unsigned char*)&arpPacket,sizeof(ARPPacket));
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

