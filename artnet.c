#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "common.h"
#include "artnet.h"
#include "ws2812.h"

// local prototypes
void artnet_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

// UDP holder
struct udp_pcb *pcb;

// Holder for callback function
static void(*onDataAvailable)(void *) = NULL;
static uint8_t nrArtPollResponses = 0;

/*
 * ArtPoll received, reply with ArtPollReply
 */
void handleArtDiscovery(uint8_t *data, struct ArtPollReply *reply) {
	
	// Get the Artpoll ...
	struct ArtPoll artpoll = {};
	strncpy(artpoll.ID, data, 8);
	artpoll.OpCode = data[8] + data[9]<<8;	
	artpoll.ProtVerHi = data[10];	
	artpoll.ProtVerLo = data[11];	
	artpoll.Flags = data[12];
	
	// ... wait random 100ms < t < 500ms according protocol
	sleep_ms( (rand()%(500-100+1))+100);

	// Construct ArtPollReply
	strcpy(reply->ID,"Art-Net");
	reply->OpCode = 0x2100; // 0x2100, LSB first
	uint32_t ip = ip4_addr_get_u32(netif_ip4_addr(netif_list));
	reply->IPAddr[3] = (ip >> 24) & 0xFF;
	reply->IPAddr[2] = (ip >> 16) & 0xFF;
	reply->IPAddr[1] = (ip >>  8) & 0xFF;
	reply->IPAddr[0] = (ip >>  0) & 0xFF;
	reply->Port = 0x1936; 	// 0x1936, LSB first
	reply->VersInfoH = 0x01;
	reply->VersInfoL = 0x00;
	strcpy(reply->ShortName, "PC_V1.0");
	sprintf(reply->NodeReport, "#0001 %.4d 0001 dkroeske@gmail.com", nrArtPollResponses++);
	reply->PortTypes[0] = 0x80; // 1 input, DMX512
}

/*
 * Callback incomming UDP datagrams
 */
void artnet_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {

	uint8_t *data = (unsigned char*)p->payload;
	
	// 
	uint16_t OpCode = data[9]<<8+data[8];
	switch( OpCode ) {
		case 0x2000: {
			// Construct ArtNet reply
			struct ArtPollReply reply = {};
			handleArtDiscovery(data, &reply);
			
			// UDP transmit to caller of ArtPoll
			cyw43_arch_lwip_begin();
			struct pbuf *tx_buff = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct ArtPollReply), PBUF_RAM);
			memcpy((uint8_t *)tx_buff->payload, (uint8_t *)&reply, sizeof(struct ArtPollReply));
			udp_sendto(pcb, tx_buff, addr, ARTNET_DISCOVERY_PORT);
			pbuf_free(tx_buff);
			cyw43_arch_lwip_end();
			break;
		}	

		// ArtDmx
		case 0x5000: {

			struct ArtDmx dmx;

			/* Fast fill ArtDmx struct and convert payload to ArtDmx */
			memcpy( (uint8_t *)&dmx, data, sizeof(struct ArtDmx));
			
			/* Construct general DMX packet */			
			DMX_DATAPACKET_STRUCT dp;
			dp.sequence = dmx.Sequence;
			dp.datap = dmx.Data; 

			/* Call back to proces */
			if( NULL != onDataAvailable) {
				onDataAvailable(&dp);
			}
					
			break;
		}
		
	}

	// Mandatory
	pbuf_free(p);
}

/*
 * Init UDP interface
 */
int initArtNet( void(*cb)(void *) )
{
	// Save callback and holder datapacket
	onDataAvailable = cb;

	// Setup new UDP connection
	pcb = udp_new();

	// Bind to port Artnet discovery port
  	err_t err = udp_bind(pcb, IP_ADDR_ANY, ARTNET_DISCOVERY_PORT);

	// Start receiver and set callback
	udp_recv(pcb, artnet_recv, NULL);
	
	return true;
}
