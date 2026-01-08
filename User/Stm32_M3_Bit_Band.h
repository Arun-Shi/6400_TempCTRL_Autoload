#ifndef __H
#define __H

#define PAout(n)	*((volatile unsigned int*)(((0x40020000-0x40000000+0x14)*8+n)*4+0x42000000))
#define PBout(n)	*((volatile unsigned int*)(((0x40020400-0x40000000+0x14)*8+n)*4+0x42000000))
#define PCout(n)	*((volatile unsigned int*)(((0x40020800-0x40000000+0x14)*8+n)*4+0x42000000))
#define PDout(n)	*((volatile unsigned int*)(((0x40020C00-0x40000000+0x14)*8+n)*4+0x42000000))
#define PEout(n)	*((volatile unsigned int*)(((0x40021000-0x40000000+0x14)*8+n)*4+0x42000000))
#define PFout(n)	*((volatile unsigned int*)(((0x40021400-0x40000000+0x14)*8+n)*4+0x42000000))
#define PGout(n)	*((volatile unsigned int*)(((0x40021800-0x40000000+0x14)*8+n)*4+0x42000000))	
 
#define PAin(n)		*((volatile unsigned int*)(((0x40020000-0x40000000+0x10)*8+n)*4+0x42000000))
#define PBin(n)		*((volatile unsigned int*)(((0x40020400-0x40000000+0x10)*8+n)*4+0x42000000))
#define PCin(n)		*((volatile unsigned int*)(((0x40020800-0x40000000+0x10)*8+n)*4+0x42000000))
#define PDin(n)		*((volatile unsigned int*)(((0x40020C00-0x40000000+0x10)*8+n)*4+0x42000000))
#define PEin(n)		*((volatile unsigned int*)(((0x40021000-0x40000000+0x10)*8+n)*4+0x42000000))
#define PFin(n)		*((volatile unsigned int*)(((0x40021400-0x40000000+0x10)*8+n)*4+0x42000000))
#define PGin(n)		*((volatile unsigned int*)(((0x40021800-0x40000000+0x10)*8+n)*4+0x42000000))


#endif

