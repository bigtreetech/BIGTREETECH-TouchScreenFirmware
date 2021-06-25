/*-
 * MIT License
 *
 * Copyright (c) 2021 H. Gregor Molter <gregor.molter@secretlab.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Base64 decoder based on:
 * https://github.com/rafagafe/base64/blob/master/base64.c
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2016-2018 Rafa Garcia <rafagarcia77@gmail.com>.
 */

#include "base64.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/** Escape values. */
enum base64_special_e {
    notabase64 = 64, /**< Value to return when a non base64 digit is found. */
    terminator = 65, /**< Value to return when the character '=' is found.  */
    gcodecomment = 66 /**< Value to return when the character ';', ' ', '\r', '\n' is found.  */
};

/** Lookup table that converts a base64 digit to integer. */
static char const base64_digittobin[] = {
// 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 66, 64, 64, 66, 64, 64,
//16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47
  66, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
//48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 66, 64, 65, 64, 64,
//64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  
  64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
//80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95  
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
//96  97  98  99 100 101 102 103 104 105 106 107 108 109 110 111  
  64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
//112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127  
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
//128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//224 225 226 227 228 229 230 231 232 233 234 235 236 237 238 239  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
//240 241 242 243 244 245 246 247 248 249 250 251 252 253 254 255  
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

/* Initializes a static or dynamically allocated b64_decoder_t struct.
 * fp is the gcode file seeked to the beginning of the base64 block.
 */
void b64_init(b64_decoder_t * bd, FIL * fp, uint32_t base64_length) {
  bd->rest_in_block = 0;
  bd->remaining_base64 = base64_length;
  bd->fp = fp;
  bd->finished = false;
}

/* Reads the next Base64 digit from the gcode file. 
 * Skips ';', ' ', '\n' and '\r'. 
 */
char const b64_read_next_digit(b64_decoder_t * bd) {
  unsigned char current;
  char digit;
  UINT br = 0;
  if (bd->remaining_base64 == 0) {
    return terminator;
  }
  do {
    if (f_read(bd->fp, &current, 1, &br) != FR_OK) return 0;
    digit = base64_digittobin[current];
  } while (digit == gcodecomment);
  bd->remaining_base64--;
  return digit;
}

/* Convert a base64 string (encoded as a gcode comment 
 * blockwise (3 bytes per block) to binary format.)
*/
void b64_read_next_block(b64_decoder_t * bd) {
  if (bd->finished) goto failed;

  uint8_t const a = b64_read_next_digit(bd);
  if ( a == notabase64 ) goto finished; 
  if ( a == terminator ) goto finished; 

  uint8_t const b = b64_read_next_digit(bd);
  if ( b == notabase64 ) goto failed;
  if ( b == terminator ) goto failed;

  bd->block[0] = ( a << 2u ) | ( b >> 4u );
  bd->rest_in_block = 1;

  uint8_t const c = b64_read_next_digit(bd);
  if ( c == notabase64 ) goto failed;

  uint8_t const d = b64_read_next_digit(bd);
  if ( d == notabase64 ) goto failed;
  if ( c == terminator ) {
      if ( d != terminator ) goto failed;
      goto finished;
  }

  bd->block[1] = ( b << 4u ) | ( c >> 2u );
  bd->rest_in_block = 2;

  if ( d == terminator ) goto finished;

  bd->block[2] = ( c << 6u ) | ( d >> 0u );
  bd->rest_in_block = 3;
  return;
failed:
  bd->rest_in_block = 0;
  return;
finished:
  // We have <3 bytes in the buffer and the caller expects that the the buffer is filled towards the end
  // For example: if rest_in_block is 1 than it is expected that the data is at block[sizeof(block)-rest_in_block] = block[2]
  // Because we reached finished, bd->rest_in_block is >0 and <3. Thus, it is only 1 or 2.
  if (bd->rest_in_block == 1) {
    bd->block[2] = bd->block[0];
  } else { /* bd->rest_in_block == 2 */
    bd->block[2] = bd->block[1];
    bd->block[1] = bd->block[0];
  }
  bd->finished = true;
  return;
}

/* Decodes 'n' bytes of base64 encoded data to the buffer 'pc'. */
ssize_t b64_read(b64_decoder_t * bd, char * pc, ssize_t n) {
  ssize_t rest = n;
  while (rest > 0) {
    if (bd->rest_in_block>0) {
      uint8_t fed = MIN(bd->rest_in_block, rest);
      uint8_t *buf = &bd->block[sizeof(bd->block)-bd->rest_in_block];
      for (uint8_t i=0; i<fed; i++) {
        *pc++ = *buf++;
      }
      rest -= fed;
      if (rest == 0) {
        // Adjust rest of block and return
        bd->rest_in_block -= fed;
        return n; 
      }
    }
    if (rest > 0) {
      // Load the next b64 block
      b64_read_next_block(bd);
      if (bd->rest_in_block == 0) {
        // We will not get any more data
        return n-rest;
      }
    }
  }
  return n;
}