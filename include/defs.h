#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define FRM_HEAD_LEN         64
#define FRM_FORMINFO_LEN     288

#define IO_SIZE              4096

//2字节
typedef short          int16;
typedef unsigned short uint16;
//4字节
typedef int            int32;
typedef unsigned int   uint32;
//8字节
typedef unsigned long long int ulonglong; /* ulong or unsigned long long */
typedef long long int	longlong;

typedef unsigned char  uchar;
//typedef unsigned char  bool;
//#define true   1
//#define false  0


#define sint2korr(A)	(*((int16 *) (A)))
#define uint2korr(A)	(*((uint16 *) (A)))

#define uint3korr(A)	(uint32) (((uint32) ((uchar) (A)[0])) +\
				  (((uint32) ((uchar) (A)[1])) << 8) +\
				  (((uint32) ((uchar) (A)[2])) << 16))

#define sint4korr(A)	(*((long *) (A)))
#define uint4korr(A)	(*((uint32 *) (A)))

#define uint8korr(A)	(*((ulonglong *) (A)))
#define sint8korr(A)	(*((longlong *) (A)))




//------------------------------------------------------------------

extern char* legacy_db_types[128];
extern char* row_type[8];
extern unsigned int innodb_slot;

char *strmov(register char *dst, register const char *src);
int read_string(FILE *file, unsigned char **to, size_t length) ;
void printf_buff(char* debug_info,unsigned char *buff, size_t length);
char** split_string(char* buff, char* split, int* result_len);

#endif 
