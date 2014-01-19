/*
 * uart.h
 *
 *  Created on: Jan 17, 2014
 *      Author: TheuCWorld
 */

#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_putc(unsigned char data);
void uart_puts(char* data);
void uart_send_data(unsigned char* data, unsigned int length);
unsigned char uart_getc(void);
void uart_rx_callback(void (*func)(unsigned char));

#endif /* UART_H_ */
