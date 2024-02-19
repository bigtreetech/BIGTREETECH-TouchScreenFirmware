#ifndef _MY_MISC_H_
#define _MY_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>  // for size_t

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M)  STRINGIFY_(M)

#define COUNT(n) (sizeof(n) / sizeof(n[0]))

#define ABS(n)    ((n) > 0 ? (n) : -(n))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define NOBEYOND(min, v, max) MAX(min, MIN(v, max))

#define WITHIN(N, L, H) ((N) >= (L) && (N) <= (H))
#define NUMERIC(a)      WITHIN(a, '0', '9')

// Bitwise macros

// Get bit status at selected index
#define GET_BIT(num, index) (((num) >> (index)) & 1)

// Set bit status at selected index to 1
#define SET_BIT_ON(num, index) ((num) |= (1 << (index)))

// Set bit status at selected index to 0
#define SET_BIT_OFF(num, index) ((num) &= ~(1 << (index)))

// Set bit status value (0 or 1) at selected index
#define SET_BIT_VALUE(num, index, value) ((num) = ((num) & (~(1 << (index)))) | ((value) << (index)))

// Toggle bit status at selected index
#define TOGGLE_BIT(num, index) ((num) ^= (1 << (index)))

// Flip all bits
#define FLIP_BITS(num) ~num

// Time conversion
#define SEC_TO_MS(t)  (t * 1000)           // seconds to milliseconds
#define MS_TO_SEC(t)  (t / 1000)           // milliseconds to seconds
#define MIN_TO_SEC(t) (t * 60)             // minute to seconds
#define MIN_TO_MS(t)  (SEC_TO_MS(t) * 60)  // minute to milliseconds

#define HOURS(t)   (t / (60 * 60))       // hours completed
#define MINUTES(t) (t % (60 * 60) / 60)  // minutes remaining to next hour
#define SECONDS(t) (t % 60)              // seconds remaining to next minute

#define strtod strtod_ligth  // light weight strtod() function without exponential support

#define strncpy(...) \
  do { \
    _Pragma("GCC error \"Error: strncpy() is deprecated! Use the alternatives like strncpy_pad() or strncpy_no_pad()\""); \
  } while (0)

// call processes from the argument and than loopProcessAndGUI() while condition is true
// tasks from argument must be separated by ";" ("TASK_LOOP_WHILE(condition, task1(); task2(); ...))
#define TASK_LOOP_WHILE(condition, ...) \
  while (condition)                     \
  {                                     \
    __VA_ARGS__;                        \
    loopProcessAndGUI();                \
  }

uint8_t inRange(int cur, int tag , int range);
long map(long x, long in_min, long in_max, long out_min, long out_max);

uint32_t calculateCRC16(const uint8_t *data, uint32_t length);  // calculate CRC16 checksum

uint8_t string_2_uint8(const uint8_t *str);                               // string convert to uint8, MSB ("2C" to 0x2C)
uint8_t *uint8_2_string(uint8_t num, uint8_t *str);                       // uint8 convert to string, MSB (0x2C to "2C")
uint32_t string_2_uint32(const uint8_t *str, const uint8_t bytes_num);    // string convert to uint32, MSB
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *str);  // uint32 convert to string, MSB
void time_2_string(char *buf, char *str_format, uint32_t time);           // convert time to string with given formatting

double strtod_ligth(char *str, char **endptr);               // light weight strtod() function without exponential support
void strncpy_pad(char *dest, const char *src, size_t n);     // light weight and safe strncpy() function with padding
void strncpy_no_pad(char *dest, const char *src, size_t n);  // light weight and safe strncpy() function without padding

#ifdef __cplusplus
}
#endif

#endif
