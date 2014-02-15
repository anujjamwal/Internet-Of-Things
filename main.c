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


unsigned char buffer[100];
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

void populate_router_address(DeviceAddress * address)
{
	memcpy(Router.macAddress, address->macAddress, 6);
	led_on(address);

	log("Router Mac Address found and saved\r\n");
	log(" Mac Address :");
	log_arr(Router.macAddress, 6);
	log("\r\n");
}

void udp_handler(DeviceAddress * address, unsigned char* data, int len) {
	log("Got a UDP Message\r\n");
	log_arr(data, len);
	log("\r\n\n");
}

int main( void )
{
	bsp_init();
	LED_OFF;
	uart_rx_callback(&echo);
	ip_init();
	BLINK_LED

	log("Initializing device...\r\n");
	log(" Mac Address :");
	log_arr(Config.macAddress, 6);
	log("\r\n");
	log(" IP Address :");
	log_arr(Config.ipAddress, 4);
	log("\r\n");

	log("Requesting Router Mac address...\r\n");
	log(" IP Address :");
	log_arr(Router.ipAddress, 4);
	log("\r\n");

	udp_callback(&udp_handler);

	//initialize router ip settings
	ip_to_mac(routerIP, &populate_router_address);

	while(1)
	{
		if(!led){
			BLINK_LED
		}
		int size = mac_read(buffer, 100);
		if(size > 0)
			handle_request(buffer);
		__delay_ms(1000);
	}
}
