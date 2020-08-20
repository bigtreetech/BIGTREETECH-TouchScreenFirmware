#include "my_misc.h"

uint8_t inRange(int cur, int tag , int range)
{
  if ((cur <= tag + range) && (cur >= tag - range))
    return 1;
  return 0;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// string convert to uint8, MSB
// "2C" to 0x2C
uint8_t string_2_uint8_t(const uint8_t *string)
{
	uint8_t rtv = 0;
	for (unsigned char i = 0; i < 2; i++)
	{
		rtv <<= 4;
		if (string[i] >= '0' && string[i] <= '9')
			rtv |= string[i] - '0';
		else if (string[i] >= 'A' && string[i] <= 'F')
			rtv |= string[i] - 'A' + 0xA;
		else if (string[i] >= 'a' && string[i] <= 'f')
			rtv |= string[i] - 'a' + 0xA;
		else
			rtv |= 0;
	}
	return rtv;
}

// uint8 convert to string, MSB
// 0x2C to "2C"
uint8_t *uint8_2_string(uint8_t num, uint8_t *string)
{
	for (unsigned char i = 0; i < 2; i++)
	{
		uint8_t _4bits = (num & 0xF0) >> 4;
		if (_4bits <= 9)
			string[i] = _4bits + '0';
		else if (_4bits >= 0xA && _4bits <= 0xF)
			string[i] = _4bits - 0xA + 'A';
		else
			string[i] = 'F';
		num <<= 4;
	}
	return string;
}

// string convert to uint32, MSB
uint32_t string_2_uint32(const uint8_t *string, const uint8_t bytes_num)
{
	uint32_t rtv = 0;
	for (uint8_t i = 0; i < bytes_num; i++)
	{
		rtv <<= 8;
		rtv |= string_2_uint8_t(string + 2*i);
	}
	return rtv;
}

// uint32 convert to string, MSB
uint8_t *uint32_2_string(uint32_t num, uint8_t bytes_num, uint8_t *string)
{
	for (uint8_t i = 0; i < bytes_num; i++)
	{
		uint8_t bit = (bytes_num - i - 1) * 8;
		uint8_t _8bits = (num & (0xFF << bit)) >> bit;
		uint8_2_string(_8bits, string + 2*i);
	}
	return string;
}
