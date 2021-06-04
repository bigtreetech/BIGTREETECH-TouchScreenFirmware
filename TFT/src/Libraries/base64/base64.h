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

#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif 

// Main struct
typedef struct {
  uint8_t block[3];
  uint8_t rest_in_block;
  uint32_t remaining_base64;
  FIL * fp;
  bool finished;
} b64_decoder_t;

// Basic interfaces
void b64_init(b64_decoder_t * bd, FIL * fp, uint32_t base64_length);
char const b64_read_next_digit(b64_decoder_t * bd);
void b64_read_next_block(b64_decoder_t * bd);
ssize_t b64_read(b64_decoder_t * bd, char * pc, ssize_t n);

#ifdef __cplusplus
}
#endif

#endif /* __BASE64_H__ */