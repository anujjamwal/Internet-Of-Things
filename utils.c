/*
 * utils.c
 *
 *  Created on: Jan 19, 2014
 *      Author: TheuCWorld
 */

#include "utils.h"
#include "uart.h"

void __delay_ms(int milliseconds)
{
	while(milliseconds--)
		__delay_cycles(CYCLES_PER_MS);
}

void log(char * text)
{
	uart_puts(text);
}
