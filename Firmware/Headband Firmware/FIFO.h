/*
 * FIFO Buffer
 * Implementation uses arrays to conserve memory
 * 
 * The MIT License (MIT)
 * 
 * Adapted from 2015 Daniel Eisterhold
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
 
#ifndef __FIFO__
#define __FIFO__

#include "communication.h"

#include "Arduino.h"

#define FIFO_SIZE 1000

class FIFO {
  private:
    int head;
    int tail;
    int numElements;
    struct_message buffer[FIFO_SIZE];
  public:
    FIFO();
    ~FIFO();
    void push(struct_message data);
    struct_message pop();
    int size();
};

#endif // __FIFO__
