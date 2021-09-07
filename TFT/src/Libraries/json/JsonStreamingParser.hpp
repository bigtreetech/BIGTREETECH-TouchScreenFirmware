/**The MIT License (MIT)

Copyright (c) 2015 by Daniel Eichhorn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

See more at http://blog.squix.ch and https://github.com/squix78/json-streaming-parser
*/

#pragma once

#include <stdlib.h>
#include "JsonListener.hpp"

#define STATE_START_DOCUMENT     0
#define STATE_DONE               -1
#define STATE_IN_ARRAY           1
#define STATE_IN_OBJECT          2
#define STATE_END_KEY            3
#define STATE_AFTER_KEY          4
#define STATE_IN_STRING          5
#define STATE_START_ESCAPE       6
#define STATE_UNICODE            7
#define STATE_IN_NUMBER          8
#define STATE_IN_TRUE            9
#define STATE_IN_FALSE           10
#define STATE_IN_NULL            11
#define STATE_AFTER_VALUE        12
#define STATE_UNICODE_SURROGATE  13

#define STACK_OBJECT             0
#define STACK_ARRAY              1
#define STACK_KEY                2
#define STACK_STRING             3

#define BUFFER_MAX_LENGTH  1024

class JsonStreamingParser {
  private:


    int state;
    int stack[20];
    int stackPos = 0;
    JsonListener* myListener;

    bool doEmitWhitespace = false;
    // fixed length buffer array to prepare for c code
    char buffer[BUFFER_MAX_LENGTH];
    int bufferPos = 0;

    char unicodeEscapeBuffer[10];
    int unicodeEscapeBufferPos = 0;

    char unicodeBuffer[10];
    int unicodeBufferPos = 0;

    int characterCounter = 0;

    int unicodeHighSurrogate = 0;

    void increaseBufferPointer();

    void endString();

    void endArray();

    void startValue(char c);

    void startKey();

    void processEscapeCharacters(char c);

    bool isDigit(char c);

    bool isHexCharacter(char c);

    char convertCodepointToCharacter(int num);

    void endUnicodeCharacter(int codepoint);

    void startNumber(char c);

    void startString();

    void startObject();

    void startArray();

    void endNull();

    void endFalse();

    void endTrue();

    void endDocument();

    int convertDecimalBufferToInt(char myArray[], int length);

    void endNumber();

    void endUnicodeSurrogateInterstitial();

    bool doesCharArrayContain(char myArray[], int length, char c);

    int getHexArrayAsDecimal(char hexArray[], int length);

    void processUnicodeCharacter(char c);

    void endObject();



  public:
    JsonStreamingParser();
    void parse(char c);
    void setListener(JsonListener* listener);
    void reset();
};

extern JsonStreamingParser jsonStreamingParser;
