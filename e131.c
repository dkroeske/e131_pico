#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "common.h"	
#include "e131.h"

const unsigned char VECTOR_E131_DATA_PACKET[] = {0, 0, 0, 0x02};
#define VECTOR_E131_DATA_PACKET_OFFSET 40
const unsigned char VECTOR_DMP_SET_PROPERTY = 0x02;
#define VECTOR_DMP_SET_PROPERTY_OFFSET 117
const unsigned char VECTOR_ROOT_E131_DATA[] = {0, 0, 0, 0x4};
#define VECTOR_ROOT_E131_DATA_OFFSET 18

//VECTOR_ROOT_E131_EXTENDED = (0, 0, 0, 0x8)

//VECTOR_E131_EXTENDED_SYNCHRONIZATION = (0, 0, 0, 0x1)
//VECTOR_E131_EXTENDED_DISCOVERY = (0, 0, 0, 0x2)
//VECTOR_UNIVERSE_DISCOVERY_UNIVERSE_LIST = (0, 0, 0, 0x1)


// local prototypes
int e131_handleDataPacket(uint8_t *raw_data, DMX_DATAPACKET_STRUCT *data_packet);
void e131_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

// UDP holder
struct udp_pcb *upcb;

// Holder for callback function
static void(*onDataAvailable)(void *) = NULL;

/*
 * Try to decode E131 datapackets
 */
//int handleDataPacket(uint8_t *raw_data, E131_DATAPACKET_STRUCT *data_packet) {
int e131_handleDataPacket(uint8_t *raw_data, DMX_DATAPACKET_STRUCT *data_packet) {
	
	uint8_t retval = false;
	
  	// Check for valid E131 DATAPACKET
	if( 0 == memcmp(&raw_data[VECTOR_ROOT_E131_DATA_OFFSET], VECTOR_ROOT_E131_DATA, 4) && 
	    0 == memcmp(&raw_data[VECTOR_E131_DATA_PACKET_OFFSET], VECTOR_E131_DATA_PACKET, 4) && 
	    raw_data[VECTOR_DMP_SET_PROPERTY_OFFSET] == VECTOR_DMP_SET_PROPERTY )
	{
		if( NULL != data_packet ) {
			data_packet->datap = &raw_data[126];
			retval = true;
		}
	} else {
		printf("Invalid E131 packet, bummer\n");
	}

	return retval;
}


/*
 * Callback incomming UDP datagrams
 */
void e131_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {

		DMX_DATAPACKET_STRUCT dp;

   	if( true == e131_handleDataPacket( (unsigned char *)p->payload, &dp) ) {

		if( NULL != onDataAvailable ) {
			onDataAvailable(&dp);		
		}
	}

	pbuf_free(p);
}

/*
 * Init UDP interface
 */
int initE131( void(*cb)(void *) )
{
	// Save callback and holder datapacket
	onDataAvailable = cb;

	// Setup new UDP connection
	upcb = udp_new();

	// Bind to port E131_PORT
  	err_t err = udp_bind(upcb, IP_ADDR_ANY, E131_PORT);
	
	// Start receiver and set callback
	udp_recv(upcb, e131_recv, NULL);
	
	return true;
}
