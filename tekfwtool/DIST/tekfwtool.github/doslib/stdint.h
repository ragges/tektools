#ifndef _STDINT_H
#define _STDINT_H

#include <limits.h>

#if (USHRT_MAX != 0xFFFFUL)
	#error Bad unsigned short !
#elif (ULONG_MAX != 0xFFFFFFFFUL)
	#error Bad unsigned long !
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;


#endif // _STDINT_H
