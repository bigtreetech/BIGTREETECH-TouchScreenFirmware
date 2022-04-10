#ifndef _MY_MISC_H_
#define _MY_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Menu Macros
#define OPEN_MENU(x)    infoMenu.menu[++infoMenu.cur] = x
#define REPLACE_MENU(x) infoMenu.menu[infoMenu.cur] = x
#define CLOSE_MENU()    infoMenu.cur--
#define MENU_IS(x)      infoMenu.menu[infoMenu.cur] == x
#define MENU_IS_NOT(x)  infoMenu.menu[infoMenu.cur] != x

// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M)  STRINGIFY_(M)

#define COUNT(n) (sizeof(n)/sizeof(n[0]))

#define ABS(n)    ((n) > 0 ? (n) : -(n))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define NOBEYOND(min, v, max) MAX(min, MIN(v, max))

#define WITHIN(N, L, H) ((N) >= (L) && (N) <= (H))
#define NUMERIC(a)      WITHIN(a, '0', '9')

// Bitwise macros

// Get bit status at selected index
#define GET_BIT(num, index) ((num >> index) & 1)

// set bit status at selected index to 1
#define SET_BIT_ON(num, index) num = (1 << index) | num

// set bit status at selected index to 0
#define SET_BIT_OFF(num, index) num = num & (~(1 << index))

// set bit status value (0 or 1) at selected index
#define SET_BIT_VALUE(num, index, value) num = ((value) == 1) ? (1 << index) | num : num & (~(1 << index))

// Toggle bit status at selected index
#define TOGGLE_BIT(num, index) num =  num ^ (1 << index)

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

#define strtod stringToDouble  // enable light weight string to double function without exponential support

uint8_t inRange(int cur, int tag , int range);
long map(long x, long in_min, long in_max, long out_min, long out_max);

uint8_t string_2_uint8_t(const uint8_t *string);
uint8_t *uint8_2_string(uint8_t num, uint8_t *string);
uint32_t string_2_uint32(const uint8_t *string, const uint8_t bytes_num);
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *string);
double stringToDouble(char *str, char **endptr);
void timeToString(char *buf, char *strFormat, uint32_t time);

const char *stripHead(const char *str);  // strip out any leading " ", "/" or ":" character that might be in the string
void stripChecksum(char *str);           // strip out any trailing checksum that might be in the string
uint8_t getChecksum(char *str);
bool validateChecksum(char *str);

#ifdef __cplusplus
}
#endif

#endif
