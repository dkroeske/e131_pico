#include <stdlib.h>

#ifndef RECEIVER_INC
#define RECEIVER_INC

#define E131_PORT 5568	// default E1.31 port (sACN protocol)

/*
 * Init UDP interface
 */
int initE131( void(*cb)(void *) );

#endif
