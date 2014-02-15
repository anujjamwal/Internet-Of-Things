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
const unsigned char routerIpAddress[] = ROUTER_ADDRESS;

void ip_init(void)
{
	memcpy(Config.macAddress, deviceMacAddress, 6);
	memcpy(Config.ipAddress, deviceIpAddress, 4);
	memcpy(Router.ipAddress, routerIpAddress, 4);

	mac_init();
}

unsigned int chksum(unsigned int sum, unsigned char *data, unsigned int len)
{
  unsigned int t;
  const unsigned char *dataptr;
  const unsigned char *last_byte;

  dataptr = data;
  last_byte = data + len - 1;

  while(dataptr < last_byte) {	/* At least two more bytes */
    t = (dataptr[0] << 8) + dataptr[1];
    sum += t;
    if(sum < t) {
      sum++;		/* carry */
    }
    dataptr += 2;
  }

  if(dataptr == last_byte) {
    t = (dataptr[0] << 8) + 0;
    sum += t;
    if(sum < t) {
      sum++;		/* carry */
    }
  }

  /* Return sum in host byte order. */
  return sum;
}
