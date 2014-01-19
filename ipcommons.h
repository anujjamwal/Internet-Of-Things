/*
 * ipcommons.h
 *
 *  Created on: Jan 20, 2014
 *      Author: TheuCWorld
 */

#ifndef IPCOMMONS_H_
#define IPCOMMONS_H_

#define MAC_ADDRESS {0x70, 0x45, 0x54, 0xde, 0x6a, 0x50}
#define IP_ADDRESS  {192,168,0,13}

typedef struct {
	unsigned char macAddress[6];
	unsigned char ipAddress[4];
} DeviceAddress;

DeviceAddress Config;

void ip_init(void);

#endif /* IPCOMMONS_H_ */
