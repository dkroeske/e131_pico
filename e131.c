#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "ws2812.h"
#include "ws2812.pio.h"
#include "receiver.h"

// SSID and PASS for Wifi connection, options given by -D cmake
char *ssid = WIFI_SSID;
char *pass = WIFI_PASSWORD;

//typedef unsigned char uchar8_t;


/* Callback */
void onDataEvent(void* data);

void onDataEvent(void *data){
	
	E131_DATAPACKET_STRUCT *dp = (E131_DATAPACKET_STRUCT *)data;
//	printf("(%d): ", dp->sequence );
//	for( uint8_t idx = 0; idx < 9; idx++ ) {
//		printf("0x%.2X ", dp->data[idx]);
//	}
//	printf("\n");
	for(uint16_t idx = 0; idx < 100*3; idx+=3) {
        	pixel(dp->data[idx], dp->data[idx+2], dp->data[idx+1]);
	}
}


/*
 * Set up Wifi and init UDP receiver
 */
int main() {
	stdio_init_all();

	sleep_ms(500);
	printf("Simple E131 pixelcontroller\n");

	// Init WiFi with given credentials
	if( cyw43_arch_init() ) {
		printf("Failed to initialize\n");
	}

	// ... and connect to AP
	cyw43_arch_enable_sta_mode();
	if( !cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
		printf("IP = %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));
	} else {
		printf("Failed to connect\n");
	}

	// Connect to UDP with callback and e131 datapacket
	E131_DATAPACKET_STRUCT datapacket;
	initUDPReceiver(onDataEvent, &datapacket);

	// Init pixels
	ws2812_init();

	uint8_t t = 0;

	pixel(0xFF,0,0);
	pixel(0xFF,0,0);
	pixel(0,0xFF,0);
	pixel(0,0xFF,0);
	pixel(0,0,0xFF);
	pixel(0,0,0xFF);

	while(true) {
	
		printf("pico_w 0x%.2X\n", t++);
		sleep_ms(5000);
	}
}
