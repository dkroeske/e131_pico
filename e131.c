#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "common.h"
#include "ws2812.h"
#include "ws2812.pio.h"
#include "receiver.h"
#include "artnet.h"
#include "web.h"
#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "config.h"

// SSID and PASS for Wifi connection, options given by -D cmake
char *ssid = WIFI_SSID;
char *pass = WIFI_PASSWORD;

// Global var's
bool idle_loop = false;

// Function prototypes
void handle_idle();
int mount_sd(void);
void onDataEvent(void* data);


/*
 * Callback
 */
void onDataEvent(void *data){

	DMX_DATAPACKET_STRUCT *dp = (DMX_DATAPACKET_STRUCT *)data;
//	printf("(%.4d): ", dp->sequence );
//	for( uint8_t idx = 0; idx < 4; idx++ ) {
//		printf("0x%.2X ", dp->datap[idx]);
//	}
//	printf("\n");
	for(uint16_t idx = 0; idx < NR_LEDS*3; idx+=3) {
		pixel(dp->datap[idx], dp->datap[idx+1], dp->datap[idx+2]);
	}

	// Mark idleloop = false;
	idle_loop = false;

}


/*
 * Set up Wifi and init UDP receiver
 */
int main() {

 	int retval = OK;

	stdio_init_all();
	sleep_ms(2000);
	
	stdio_flush();
	printf("Pico Pixelcontroller\n");

	// 
	// Try mount SD card. Essential for WIFI connection 
	// and idle sequence
	//
	retval = mount_sd();

	//
	// Read config from SD.
	//
	if( OK == retval ) {
		retval =  config_init("config.txt");
	}

	// 
	// Connect to AP
	//
	if( OK == retval ) {
		// Init WiFi with given credentials
		if( cyw43_arch_init() ) {
			retval = NOK;
			printf("Failed to initialize\n");
		}

		// ... and connect to AP
		cyw43_arch_enable_sta_mode();
		if( !cyw43_arch_wifi_connect_timeout_ms(
			config_get_ssid(),
			config_get_password(),
			CYW43_AUTH_WPA2_AES_PSK, 
			30000)) {
			printf("IP = %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));
		} else {
			retval = NOK;
			printf("Failed to connect\n");
		}
	}

	// Init services web, e131 and artnet
	if( OK == retval ) {
		// Init and start webserver:80
		init_web();

		// Init Artnet handling discovery protocol
		initArtNet(onDataEvent);

		// Connect to UDP with callback and e131 datapacket
		initE131(onDataEvent);
	}
	

	// 
	// Init pixels
	//
	ws2812_init();

	// 
	// Enter main loop
	//
	uint16_t t = 0;
	while(true) {
		
		// Check is idle_loop can be executed
		if(true == idle_loop){
			handle_idle();	
		}		
	
		printf("pico_w 0x%.4X\n", t++);
		sleep_ms(5000);
	}
}


/*
 * Try to mount SD card.
 */
int mount_sd(void) {

	int retval = OK;
		
	// Mount SD card if possible 
	if( OK == retval ) {
		sd_card_t *pSD = sd_get_by_num(0);
		FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
		if( FR_OK != fr ) {
			retval = NOK;
		} else {
			sleep_ms(250);
		}
	}
	return retval;
}


/* 
 * Handle idle loop
 */
void handle_idle() {

	uint8_t idx = 0;
	uint8_t buf[NR_LEDS*3];

	FIL fp;
	FRESULT fr = f_open(&fp, "idle.eseq", FA_READ);
	if( FR_OK != fr ) {
		printf("Error open idle.eseq\n");
	} else {
		while(idle_loop) {

			// Skip header in eseq file
			f_lseek(&fp, 20);
			do {
		
				// Read up NR_LEDS
				f_read(&fp, buf, NR_LEDS_IDLE_LOOP*3, NULL);
	      	
				// Playback
				for(int idx = 0; idx < NR_LEDS*3; idx+=3) {
					pixel(buf[idx], buf[idx+1], buf[idx+2]);
				}

				// looptime
				sleep_ms(25);

			} while(!f_eof(&fp) && idle_loop);
			
			f_rewind(&fp);
		}
	}
	f_close(&fp);
}
  



