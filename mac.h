/*
 * mac.h
 *
 *  Created on: Jan 19, 2014
 *      Author: TheuCWorld
 */

#ifndef MAC_H_
#define MAC_H_

extern void mac_init();
extern void mac_write(unsigned char * data, unsigned int length);
extern unsigned char mac_read(unsigned char * data, unsigned int length);

#endif /* MAC_H_ */
