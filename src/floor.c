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
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <SDL_opengl.h>
#include <math.h>

#include "floor.h"
#include "common.h"

static void colour_at(float* colour, float x, float z) {
  /* Normalise to (0..1) */
  x = (x + MAP_MAX_COORD) / (2.0f * MAP_MAX_COORD);
  z = (z + MAP_MAX_COORD) / (2.0f * MAP_MAX_COORD);

  /* Red: Decreases with increase of min(x,z) */
  colour[0] = 1.0f - fminf(x, z);
  /* Green: Linear with x */
  colour[1] = x;
  /* Blue: Linear with z */
  colour[2] = z;
}

static void floor_vertex(float x, float z) {
  float colour[3];
  colour_at(colour, x, z);
  glColor3fv(colour);
  glVertex3f(x, 0.0f, z);
}

void draw_floor(void) {
  signed x, z;
  float fx, fz;

  glBegin(GL_QUADS);
  for (z = -MAP_MAX_COORD; z < MAP_MAX_COORD; ++z) {
    fz = z;
    for (x = -MAP_MAX_COORD; x < MAP_MAX_COORD; ++x) {
      fx = x;

      floor_vertex(fx+0.05f, fz+0.05f);
      floor_vertex(fx+0.95f, fz+0.05f);
      floor_vertex(fx+0.95f, fz+0.95f);
      floor_vertex(fx+0.05f, fz+0.95f);
    }
  }
  glEnd();
}
