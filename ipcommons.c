/*
 * ipcommons.c
 *
 *  Created on: Jan 20, 2014
 *      Author: TheuCWorld
 */

#include <string.h>
#include "ipcommons.h"
#include "mac.h"

const unsigned char deviceMacAddress[] = MAC_ADDRESS;
const unsigned char deviceIpAddress[] = IP_ADDRESS;

void ip_init(void)
{
	memcpy(Config.macAddress, deviceMacAddress, 6);
	memcpy(Config.ipAddress, deviceIpAddress, 4);

	mac_init();
}
