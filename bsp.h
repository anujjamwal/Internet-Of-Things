/*
 * bsp.h
 *
 *  Created on: Jan 17, 2014
 *      Author: TheuCWorld
 */

#include <msp430g2553.h>
#include "spi.h"

#ifndef BSP_H_
#define BSP_H_

#define F_CPU 8000000
#define CYCLES_PER_MS 8000

// Port 1
#define LED BIT0
#define TX BIT1
#define RX BIT2
#define SCLK BIT5
#define MISO BIT6
#define MOSI BIT7

//Port 2
#define ENC28J60_CS BIT0
#define ENC28J60_RST BIT1

// Some  operations
#define ENC28J60_CS_0 P2OUT &= ~ENC28J60_CS
#define ENC28J60_CS_1 P2OUT |= ENC28J60_CS
#define ENC28J60_RST_0 P2OUT &= ~ENC28J60_RST
#define ENC28J60_RST_1 P2OUT |= ENC28J60_RST
#define ENC28J60_WRITE(data) spi_putc(data)
#define ENC28J60_READ() spi_getc()

#define LED_ON P1OUT |= LED
#define LED_OFF P1OUT &= ~LED

void bsp_init(void);

#endif /* BSP_H_ */
