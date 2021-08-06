#ifndef _MY_MISC_H_
#define _MY_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

#define COUNT(n) (sizeof(n)/sizeof(n[0]))

#define ABS(n) ((n) > 0 ? (n) : -(n))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define NOBEYOND(min, v, max) MAX(min, MIN(v, max))

#define strtod stringToDouble  // enable light weight string to double function without exponential support

uint8_t inRange(int cur, int tag , int range);
long map(long x, long in_min, long in_max, long out_min, long out_max);

uint8_t string_2_uint8_t(const uint8_t *string);
uint8_t *uint8_2_string(uint8_t num, uint8_t *string);
uint32_t string_2_uint32(const uint8_t *string, const uint8_t bytes_num);
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *string);
double stringToDouble(char *str, char **endptr);

#ifdef __cplusplus
}
#endif

#endif
