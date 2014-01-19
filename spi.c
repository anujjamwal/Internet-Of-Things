/*
 * spi.c
 *
 *  Created on: Jan 17, 2014
 *      Author: TheuCWorld
 */
#include "spi.h"
#include <msp430.h>

volatile unsigned int index;

void spi_init(void)
{
	UCB0CTL1 = UCSWRST;
	  // MSB first, Master, Synchronous mode, two stop bits
	UCB0CTL0 = UCSYNC + UCSPB + UCMSB + UCCKPH;
	  // Use CPU SMCLK
	UCB0CTL1 |= UCSSEL_2;
	  // Use No division on CLK
	UCB0BR0 = 0x1;
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST;
}

void spi_putc(unsigned char data)
{
	while (! (IFG2 & UCB0TXIFG));
	UCB0TXBUF = data;
    while (! (IFG2 & UCB0TXIFG));
}

unsigned char spi_getc(void)
{
	while (! (IFG2 & UCB0TXIFG));
	UCB0TXBUF = 0x00;
	while (! (IFG2 & UCB0TXIFG));
    return UCB0RXBUF;
}

void spi_send_data(unsigned char* data, unsigned int length)
{
	while (! (IFG2 & UCB0TXIFG));
	for(index = 0; index < length; index++)
	{
		UCB0TXBUF = data[index];
		while (! (IFG2 & UCB0TXIFG));
	}
}

void uscb0_tx_interrupt(void)
{

}

void uscb0_rx_interrupt(unsigned char data)
{

}
