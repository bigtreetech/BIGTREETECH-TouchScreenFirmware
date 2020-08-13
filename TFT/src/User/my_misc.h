#ifndef _MY_MISC_H_
#define _MY_MISC_H_

#include "stdint.h"

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

#define COUNT(n) (sizeof(n)/sizeof(n[0]))

typedef char*  my_va_list;
//_INTSIZEOF (n) macro: align sizeof (n) by sizeof (int)  //_INTSIZEOF(n)宏：将sizeof(n)按sizeof(int)对齐。
#define MY_INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

//// Take the first variable parameter address after the format parameter, 4 bytes aligned  //取format参数之后的第一个变参地址，4字节对齐
#define my_va_start(ap, format) ( ap = (my_va_list)&format + MY_INTSIZEOF(format) )

//For type data, first obtain its 4-byte aligned address, and then take its value  //对type类型数据，先取到其四字节对齐地址，再取其值
#define my_va_arg(ap, type)  ( *(type*)((ap += MY_INTSIZEOF(type)) - MY_INTSIZEOF(type)) )

#define my_va_end(ap)  ( ap = (my_va_list)0 )

#define ABS(n) ((n) > 0 ? (n) : -(n))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))


int  inRange(int cur, int tag , int range);
int  limitValue(int min, int value, int max);
float limitFloat(float min, float value, float max);
long map(long x, long in_min, long in_max, long out_min, long out_max);
int  my_vsprintf(char *buf, const char *fmt, my_va_list args);
void my_sprintf(char *buf, const char *fmt, ...);

uint8_t string_2_uint8_t(const uint8_t *string);
uint8_t *uint8_2_string(uint8_t num, uint8_t *string);
uint32_t string_2_uint32(const uint8_t *string, const uint8_t bytes_num);
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *string);

#endif
