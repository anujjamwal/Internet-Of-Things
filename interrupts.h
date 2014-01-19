/*
 * interrupts.h
 *
 *  Created on: Jan 18, 2014
 *      Author: TheuCWorld
 */
#include <msp430g2553.h>

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

extern void usca0_rx_interrupt(unsigned char data);
extern void usca0_tx_interrupt(void);
extern void uscb0_rx_interrupt(unsigned char data);
extern void uscb0_tx_interrupt(void);

#endif /* INTERRUPTS_H_ */
