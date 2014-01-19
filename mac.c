/*
 * mac.c
 *
 *  Created on: Jan 19, 2014
 *      Author: TheuCWorld
 */

#include "mac.h"
#include "enc28j60.h"
#include "bsp.h"
#include "utils.h"
#include "ipcommons.h"

void mac_init()
{
	enc28j60_init(Config.ipAddress);
}

void mac_write(unsigned char * data, unsigned int length)
{
	while(!enc28j60_write_packet(data, length))
	{
		log("Retrying after 10ms");
		__delay_ms(10);
	}
}

unsigned char mac_read(unsigned char * data, unsigned int length)
{
	return enc28j60_read_packet(data, length);
}
