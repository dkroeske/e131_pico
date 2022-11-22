#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include "common.h"
#include "ff.h"
#include "diskio.h"
#include "f_util.h"
#include "hw_config.h"
#include "config.h"
#include "settings.h"
#include "lex.yy.h"

CONFIG_STRUCT config;

char* config_get_ssid(void){
	return config.ssid;
}

char* config_get_password(void){
	return config.password;
}

int config_get_universe(void){
	return config.universe;
}

int config_init(const char *file_name) {

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

	// Open config file and parse config
	if( OK == retval ) {

		FIL fp;
		FRESULT fr = f_open(&fp, file_name, FA_READ);
		if( FR_OK != fr ) {
			retval = NOK;
		} else {
		
			char buf[1024] = "";
			char line_buf[80];
			
			while( NULL != f_gets(line_buf, 80, &fp) ) {
				strcat(buf, line_buf);
			}
			
			YY_BUFFER_STATE bs = yy_scan_buffer(buf, 1024);
			yy_switch_to_buffer(bs);
			yyparse(&config);
			
			printf("%s\n", config.ssid);
			printf("%s\n", config.password);
			
		} 
	} 
	
	return retval;
}
