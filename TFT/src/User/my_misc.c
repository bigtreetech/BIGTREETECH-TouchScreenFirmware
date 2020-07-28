#include "my_misc.h"

int inRange(int cur, int tag , int range)
{
  if((cur<=tag+range)&&(cur>=tag-range))
    return 1;
  return 0;
}

int limitValue(int min, int value, int max)
{
  if(value<min)
    return min;
  if(value>max)
    return max;
  return value;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int intToString(char* str, int n, int radix, char isNegative)  //Express integers as characters  //将整数表达成字符形态
{
  int i = 0, j = 0, remain = 0;
  int len = 0;
  char tmp = 0;

  if (n < 0)
  {
    isNegative = 1;
    n = -n;
  }

  do
  {
    remain = n % radix;
    if (remain > 9)
      str[i] = remain - 10 + 'A';  //For hex, 10 will be represented as A  //为了十六进制，10将表示成A
    else
      str[i] = remain + '0';      //Integer + '0' = ASCII code corresponding to the integer  //将整数+'0' = 整数对应的ASCII码
    i++;
  } while (n /= radix);

  if (isNegative == 1)
    str[i++] = '-';
  str[i] = '\0';
  len = i;

  for (i--, j = 0; j <= i; j++, i--)  //25%10 = 5,25/10 = 2,2%10 = 2,2/10 = 0，So the result in str is upside down.  //所以str中结果是倒置的，翻转一下
  {
    tmp = str[j];
    str[j] = str[i];
    str[i] = tmp;
  }

  return len;
}

const uint32_t POW_10[] = {
1, 10,100, 1000, 10000, 100000, 1000000, 10000000,
100000000, 1000000000
};

int my_vsprintf(char *buf, const char *fmt, my_va_list args)
{
  char* p;
  my_va_list p_next_arg = args;
  uint8_t bit_width[2] = {0, 6};
  uint8_t bit_sel = 0;

  for (p=buf; *fmt; fmt++)
  {
    if (*fmt != '%')
    {
      *p++ = *fmt;
      continue;
    }
    bit_width[0] = 0;
    bit_width[1] = 6;
    bit_sel = 0;

    repeat:
    fmt++;
    if (*fmt >= '0' && *fmt <= '9' && bit_sel < 2)
    {
      bit_width[bit_sel] = *fmt - '0';
      goto repeat;
    }
    switch (*fmt)
    {
      case 'd': //Decimal integer  //十进制整数
      {
        int n = my_va_arg(p_next_arg, int);
        p += intToString(p, n, 10, 0);
        break;
      }
      case 'x': //Hexadecimal integer  //十六进制整数
      {
        int n = my_va_arg(p_next_arg, int);
        p += intToString(p, n, 16, 0);
        break;
      }
      case 'f': //Floating point number  //浮点数
      {
        if((unsigned long)p_next_arg & 0x7)   //Variable parametersFloating point numbers are double by default.Ensure 8-byte memory alignment  //可变参 浮点数默认是double类型 保证内存8字节对齐
        {
          p_next_arg = (my_va_list)((unsigned long)p_next_arg + 0x7);
          p_next_arg = (my_va_list)((unsigned long)p_next_arg & 0xFFFFFFF8);
        }
        double f = my_va_arg(p_next_arg, double);  //%f, output floating point number  //%f，输出浮点数
        int n = (int)f;
        p += intToString(p, n, 10, f < 0);
        *p++ = '.';

        double d = ABS(f - n) + 0.5/MIN(1000000, POW_10[bit_width[1]]);
        for(int i=0; i < MIN(6, bit_width[1]); i++)
        {
          d *= 10;
          *p++ = (((int)d) % 10) + '0';
        }
        break;
      }
      case 'c': //Single ASCII character  //单个 ASCII 字符
      {
        *p++ = my_va_arg(p_next_arg, int);
        break;
      }
      case 's': //String  //字符串
      {
        char *str = my_va_arg(p_next_arg, char *);
        for (; *str != 0; )
        {
          *p++ = *str++;
        }
        break;
      }
      case '%': //
      {
        *p++ = '%';
        break;
      }
      case '.':
      {
        bit_sel++;
        goto repeat;
      }
      default:
      {
        break;
      }
    }
  }
  *p++ = 0;
  return (p - buf);
}

void my_sprintf(char *buf, const char *fmt, ...)
{
  my_va_list ap;
  my_va_start(ap, fmt);
  my_vsprintf(buf, fmt, ap);
  my_va_end(ap);
}

// string convert to uint8, MSB
// "2C" to 0x2C
uint8_t string_2_uint8_t(const uint8_t *string)
{
	uint8_t rtv = 0;
	for (char i = 0; i < 2; i++)
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
	for (char i = 0; i < 2; i++)
	{
		uint8_t _4bits = (num & 0xF0) >> 4;
		if (_4bits >= 0 && _4bits <= 9)
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
