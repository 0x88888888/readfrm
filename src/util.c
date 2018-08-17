#include "defs.h"

char *strmov(register char *dst, register const char *src)
{
  while ((*dst++ = *src++)) ;
  return dst-1;
}

int read_string(FILE *file, unsigned char **to, size_t length) {
 
   *to = malloc(length +1);
   if (*to == 0) {
   	  printf("read_string : malloc failed\n");
   }

   fread(*to, length, 1, file);
   *((char*) *to+length)= '\0';
   
   return 0;
}


void printf_buff(char* debug_info,unsigned char *buff, size_t length) {
   printf("-------------------%s------------------\n", debug_info);
   int i=0;
   for(i=0; i<length; i++) {
   	    if(i!=0 && (i % 16 ==0)){
   	  	  printf("\n");
   	    }
   	    printf("%02x ", buff[i]);
   }
   printf("\n");
   printf("---------------------------------------\n");
}

/*
  buff 0xff abcd 0xff fgdf 0xff 这种格式
 */
char** split_string(char* buff, char* split, int* result_len) {
    int cnt=0;
    char* p = NULL;
    char** result =NULL;
    int new_buff_len =strlen(buff)+1;
    char* new_buff1=malloc(new_buff_len);
    char* new_buff2=malloc(new_buff_len);
    memset(new_buff1, 0, new_buff_len);
    memset(new_buff2, 0, new_buff_len);

    memcpy(new_buff1, buff, new_buff_len-1);
    memcpy(new_buff2, buff, new_buff_len-1);

    cnt =0;
    p=NULL;
    char* res1= strtok_r(new_buff1, split, &p);
    while(res1 !=NULL) {
    	cnt+=1;
    	res1 = strtok_r(NULL, split, &p);
    }
    result = malloc(cnt * sizeof(char*));
    
    cnt =0;
    p=NULL;
    res1= strtok_r(new_buff2, split, &p);
    while(res1 !=NULL) {
    	result[cnt] = res1;
    	cnt+=1;
    	res1 = strtok_r(NULL, split, &p);
    }
    
    *result_len = cnt;
    free(new_buff1); // new_buff2 不能释放, 
    return result;
}

