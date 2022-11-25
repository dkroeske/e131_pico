#ifndef COMMON_INC
#define COMMON_INC

#define NOK 0 
#define OK  1

#define NR_LEDS 150
#define NR_LEDS_IDLE_LOOP 100

// DMX_datapacket
typedef struct {
	uint8_t sequence;
	uint8_t *datap;
} DMX_DATAPACKET_STRUCT;

#endif
