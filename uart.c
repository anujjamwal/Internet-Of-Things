/*
 * uart.c
 *
 *  Created on: Jan 17, 2014
 *      Author: TheuCWorld
 */
#include "uart.h"
#include <msp430g2553.h>

static void (*rx_function)(unsigned char data);
static volatile unsigned char * uart_data;
static volatile unsigned char uart_IS_STRING;
static volatile unsigned int uart_index;
static volatile unsigned int uart_len;
static volatile unsigned char uart_busy = 0;

void uart_init(void)
{
	UCA0CTL1 |= UCSSEL_2; 				//SMCLK
	UCA0BR0 = 52;                  		//8MHz, OSC16, 9600
	UCA0BR1 = 0;                   	 	//((8MHz/9600)/16) = 52.08333
	UCA0MCTL = 0x10|UCOS16; 			//UCBRFx=1,UCBRSx=0, UCOS16=1
	UCA0CTL1 &= ~UCSWRST; 				//USCI state machine
	uart_busy = 0;
}

void uart_rx_callback(void (*func)(unsigned char))
{
	rx_function = func;
	IE2 |= UCA0RXIE; 					// Enable USCI_A0 RX interrupt
	__bis_SR_register(GIE);
}

void uart_putc(unsigned char data)
{
	while (!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = data;
}

unsigned char uart_getc(void)
{
	while (!(IFG2 & UCA0RXIFG));
	return UCA0RXBUF;
}

void uart_puts(char* data)
{
	while(uart_busy);
	uart_busy = 1;
	uart_data = data;
	uart_IS_STRING = 1;
	IE2 |= UCA0TXIE;  // enable Tx interrupt
	UCA0TXBUF = *uart_data++;
}

void uart_send_data(unsigned char* data, unsigned int length)
{
	while(uart_busy);
	uart_busy = 1;
	uart_data = data ;
	uart_IS_STRING = 0;
	uart_index = 0;
	uart_len = length;
	IE2 |= UCA0TXIE;  // enable Tx interrupt
	UCA0TXBUF = uart_data[uart_index++];
}

void usca0_rx_interrupt(unsigned char data)
{
	rx_function(data);
}

void usca0_tx_interrupt(void)
{
	if(uart_IS_STRING)
	   {
		   if(*uart_data)
		   {
			   UCA0TXBUF = *uart_data++;
		   } else
		   {
			   IE2 &= ~UCA0TXIE;  // disable Tx interrupt
			   uart_busy = 0;
		   }
	   } else
	   {
		   if(uart_index < uart_len)
		   {
		      UCA0TXBUF = uart_data[uart_index++];
		   } else
		   {
		      IE2 &= ~UCA0TXIE;  // disable Tx interrupt
		      uart_busy = 0;
		   }
	   }
}
