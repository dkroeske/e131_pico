#include <stdlib.h>

#ifndef RECEIVER_INC
#define RECEIVER_INC

#define E131_PORT 5568	// default E1.31 port (sACN protocol)

// E131 datapacket
typedef struct {
	uint8_t sequence;
	uint8_t data[512];
} E131_DATAPACKET_STRUCT;


/*
 * Init UDP interface
 */
//int initUDPReceiver(onDataAvailable oda);
int initUDPReceiver( void(*cb)(void *),  E131_DATAPACKET_STRUCT *data);

#endif
