#include <stdlib.h>

#ifndef E131_INC
#define E131_INC

#define E131_PORT 5568	// default E1.31 port (sACN protocol)

/*
 * Init UDP interface
 */
int initE131( void(*cb)(void *) );

#endif
