/*
 * bsp.c
 *
 *  Created on: Jan 17, 2014
 *      Author: TheuCWorld
 */

#include "bsp.h"
#include "interrupts.h"
#include "uart.h"
#include "spi.h"

void bsp_init(void)
{
	// Setup Clock and Watchdog
	WDTCTL = WDTPW | WDTHOLD;
	DCOCTL = CALDCO_8MHZ;
	BCSCTL1 = CALBC1_8MHZ;

	// Setup Peripherals
	uart_init();
	spi_init();

	// Initializing Ports
	P1SEL  = TX | RX | SCLK | MISO | MOSI;
	P1SEL2 = TX | RX | SCLK | MISO | MOSI;
	P1DIR  = SCLK | MOSI | LED;
	P1DIR &= ~MISO;
	P2DIR  = ENC28J60_CS | ENC28J60_RST ;

}
