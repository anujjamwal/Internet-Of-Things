/*
 * utils.h
 *
 *  Created on: Jan 19, 2014
 *      Author: TheuCWorld
 */
#include "bsp.h"

#ifndef UTILS_H_
#define UTILS_H_

typedef unsigned char boolean;
#define true 1
#define false 0

void __delay_ms(int);
void log(char * text);
void log_arr(unsigned char * text, unsigned int i);

#endif /* UTILS_H_ */
