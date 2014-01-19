/*
 * interrupts.c
 *
 *  Created on: Jan 18, 2014
 *      Author: TheuCWorld
 */

#include "interrupts.h"


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	if(IFG2 & UCA0RXIFG)
		{
			usca0_rx_interrupt(UCA0RXBUF);
		} else if(IFG2 & UCB0RXIFG) {
			uscb0_rx_interrupt(UCB0RXBUF);
		}
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
	if(IFG2 & UCA0TXIFG)
	{
		usca0_tx_interrupt();
	} else if(IFG2 & UCB0TXIFG) {
		uscb0_tx_interrupt();
	}
}

