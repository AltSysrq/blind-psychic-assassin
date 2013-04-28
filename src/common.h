/*
  Copyright (c) 2013 Jason Lingle
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the name of the author nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

     THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
     ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
     FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
     DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
     OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
     OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
     SUCH DAMAGE.
*/
#ifndef COMMON_H_
#define COMMON_H_

#define MAP_MAX_COORD 32
#define NUM_NPCS 63

#ifndef HAVE_COSF
#define cosf(x) ((float)cos(x))
#endif

#ifndef HAVE_SINF
#define sinf(x) ((float)sin(x))
#endif

#ifndef HAVE_FMODF
#define fmodf(x,y) ((float)fmod(x,y))
#endif

#ifndef HAVE_ATAN2F
#define atan2f(x,y) ((float)atan2(x,y))
#endif

#ifndef HAVE_FABSF
#define fabsf(x) ((float)fabs(x))
#endif

#ifndef HAVE_FMAXF
static inline float fmaxf(float a, float b) {
  return a > b? a : b;
}
#endif

#ifndef HAVE_FMINF
static inline float fminf(float a, float b) {
  return a < b? a : b;
}
#endif

#ifndef HAVE_SQRTF
#define sqrtf(x) ((float)sqrt(x))
#endif

#define PI 3.141592653589793f

#endif /* COMMON_H_ */
