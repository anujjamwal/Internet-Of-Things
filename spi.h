/*
 * spi.h
 *
 *  Created on: Jan 17, 2014
 *      Author: TheuCWorld
 */

#ifndef SPI_H_
#define SPI_H_

void spi_init(void);
void spi_putc(unsigned char data);
void spi_send_data(unsigned char* data, unsigned int length);
unsigned char spi_getc(void);

#endif /* SPI_H_ */
