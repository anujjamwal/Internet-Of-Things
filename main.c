/*
 * main.c
 */

#include <msp430.h>
#include "bsp.h"
#include "uart.h"
#include "utils.h"
#include "ipcommons.h"
#include "internet.h"
#include "mac.h"

#define BLINK_LED 	LED_ON;\
					__delay_ms(100);\
					LED_OFF;\
					__delay_ms(100);\


ARPPacket packet;
boolean led = false;
unsigned char routerIP[4] = {192,168,0,1};

void echo(unsigned char data)
{
	uart_putc(data);
}

void led_on(DeviceAddress * address)
{
	LED_ON;
	led = true;
}

int main( void )
{
	bsp_init();
	LED_OFF;
	uart_rx_callback(&echo);
	ip_init();
	BLINK_LED

	ip_to_mac(routerIP, &led_on);

  while(1)
  {
	  if(!led){
		  BLINK_LED
	  }
	  mac_read((unsigned char *)&packet, 42);
	  handle_request((unsigned char *)&packet);
	  __delay_ms(1000);
  }
}
