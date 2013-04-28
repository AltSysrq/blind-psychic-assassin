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

#include "weapon.h"
#include "common.h"
#include "person.h"
#include "model.h"

#define MAX_WEAPONS 256
#define RADIUS 0.05f
#define SPEED 12.5f

static struct {
  float x, y, z, vx, vz;
  int is_alive;
} weapons[MAX_WEAPONS];

#define S RADIUS
static const float model[][3] = {
  { -S, -S, -S },
  { -S, -S, +S },
  { +S, -S, +S },
  { +S, -S, -S },

  { -S, +S, -S },
  { -S, +S, +S },
  { +S, +S, +S },
  { +S, +S, -S },

  { -S, -S, -S },
  { -S, +S, -S },
  { -S, +S, +S },
  { -S, -S, +S },
  
  { +S, -S, -S },
  { +S, +S, -S },
  { +S, +S, +S },
  { +S, -S, +S },

  { -S, -S, -S },
  { +S, -S, -S },
  { +S, +S, -S },
  { -S, +S, -S },

  { -S, -S, +S },
  { +S, -S, +S },
  { +S, +S, +S },
  { -S, +S, +S },
};
#undef S

void update_weapon(unsigned etms) {
  float et = etms / 1000.0f;
  unsigned i;

  for (i = 0; i < MAX_WEAPONS; ++i) {
    if (weapons[i].is_alive) {
      weapons[i].x += et * weapons[i].vx;
      weapons[i].z += et * weapons[i].vz;

      if (fmaxf(fabsf(weapons[i].x), fabsf(weapons[i].z)) > MAP_MAX_COORD ||
          weapon_collides_with_person(weapons[i].x, weapons[i].z, RADIUS))
        /* Collided with person or out of bounds, die */
        weapons[i].is_alive = 0;
    }
  }
}

void draw_weapon(void) {
  unsigned i;

  glColor3f(1,1,0);
  for (i = 0; i < MAX_WEAPONS; ++i) {
    if (weapons[i].is_alive) {
      glPushMatrix();
      glTranslatef(weapons[i].x, weapons[i].y, weapons[i].z);
      draw_model(model, sizeof(model)/sizeof(model[0]));
      glPopMatrix();
    }
  }
}

void spawn_weapon(float x, float y, float z, float theta) {
  float tcos, tsin;
  unsigned i;

  /* Convert theta to radians */
  theta = theta / 180.0f * PI;

#ifdef HAVE_SINCOSF
  sincosf(theta, &tsin, &tcos);
#else
  tcos = cosf(theta);
  tsin = sinf(theta);
#endif

  /* Find first dead weapon */
  for (i = 0; i < MAX_WEAPONS && weapons[i].is_alive; ++i);

  /* Do nothing if none are free */
  if (i >= MAX_WEAPONS) return;

  /* OK, write properties */
  weapons[i].is_alive = 1;
  weapons[i].x = x;
  weapons[i].y = y;
  weapons[i].z = z;
  weapons[i].vx = tcos * SPEED;
  weapons[i].vz = tsin * SPEED;
}
