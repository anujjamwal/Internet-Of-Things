/*
 * utils.c
 *
 *  Created on: Jan 19, 2014
 *      Author: TheuCWorld
 */

#include "utils.h"
#include "uart.h"

static int j;

void __delay_ms(int milliseconds)
{
	while(milliseconds--)
		__delay_cycles(CYCLES_PER_MS);
}

void log(char * text)
{
	uart_puts(text);
}

void log_arr(unsigned char * text, unsigned int i) {
	for(j = 0; j< i; j++) {
		uart_putc(text[j]);
	}
}
