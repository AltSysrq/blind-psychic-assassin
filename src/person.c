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
/* Define to get cosf() and sinf() */
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif

/* Define to get sincosf() if available */
#if defined(HAVE_SINCOSF) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <SDL_opengl.h>
#include <SDL.h>

#include <math.h>
#include <stdlib.h>

#include "person.h"
#include "weapon.h"
#include "common.h"

#define NUM_PEOPLE (NUM_NPCS+1)
#define PERSON_SPEED 1.0f
#define PLAYER_SPEED 1.5f
#define MAX_VTHETA 360.0f
#define PI 3.141592653589793f
#define SLEEP_TIME 4.0f
#define SLEEP_TIME_OFF 1.5f
#define MOVE_TIME 10.0f
#define MOVE_TIME_OFF 2.5f

static float randf(void) {
  return rand() / (float)RAND_MAX;
}

static struct {
  int is_alive, is_moving;
  float x, z, theta, vtheta;
  float time_until_state_change;
} people[NUM_PEOPLE];

static unsigned target;
static unsigned player_forward, player_backward, player_left, player_right;
static int killed_nontarget;

static void update_person_xz(unsigned, float, float vmul);
static void update_person_theta(unsigned, float, float vel);
static void update_player(float);
static void update_person(unsigned, float);

void init_people(void) {
  unsigned i;

  for (i = 0; i < NUM_PEOPLE; ++i) {
    people[i].is_alive = 1;
    people[i].is_moving = randf() < 0.5f;
    people[i].x = (randf()*2.0f - 1.0f) * MAP_MAX_COORD;
    people[i].z = (randf()*2.0f - 1.0f) * MAP_MAX_COORD;
    people[i].theta = randf() * 360.0f;
    people[i].vtheta = 0;
    people[i].time_until_state_change = 0;
  }
}

void update_people(unsigned et) {
  unsigned i;

  update_player(et/1000.0f);
  for (i = 1; i < NUM_PEOPLE; ++i)
    update_person(i, et/1000.0f);
}

static void update_person_xz(unsigned ix, float et, float vmul) {
  float speed = (ix? PERSON_SPEED : PLAYER_SPEED) * vmul;
  float tcos, tsin, theta;
  theta = people[ix].theta / 360.0f * PI * 2.0f;
#ifdef HAVE_SINCOSF
  sincosf(theta, &tsin, &tcos);
#else
  tcos = cosf(theta);
  tsin = sinf(theta);
#endif

  people[ix].x += tcos * speed * et;
  people[ix].z += tsin * speed * et;
}

static void update_person_theta(unsigned ix, float et, float vel) {
  float theta = people[ix].theta;
  theta += vel * et;
  theta = fmod(theta, 360.0f);
  if (theta < 0)
    theta += 360.0f;
  people[ix].theta = theta;
}

static void update_player(float et) {
  float vtheta = MAX_VTHETA;
  update_person_xz(0, et, 1.0f*!!player_forward - 1.0f*!!player_backward);
  update_person_theta(0, et, vtheta*!!player_left - vtheta*!!player_right);
}

static void update_person(unsigned ix, float et) {
  if (!people[ix].is_alive) return;

  people[ix].time_until_state_change -= et;
  if (people[ix].time_until_state_change < 0) {
    if (people[ix].is_moving) {
      /* Sleep for awhile */
      people[ix].is_moving = 0;
      people[ix].time_until_state_change = randf()*SLEEP_TIME + SLEEP_TIME_OFF;
    } else {
      /* Move around a bit */
      people[ix].is_moving = 1;
      people[ix].time_until_state_change = randf()*MOVE_TIME + MOVE_TIME_OFF;
      people[ix].vtheta = (2.0f*randf() - 1.0f) * MAX_VTHETA;

      /* Direct toward centre if too far away */
      if (MAP_MAX_COORD*0.75f <
          fmaxf(fabsf(people[ix].x), fabsf(people[ix].z))) {
        people[ix].theta = -atan2f(people[ix].z, people[ix].x) * 180.0f / PI;
        if (people[ix].theta < 0)
          people[ix].theta += 360;

        people[ix].vtheta /= 8.0f;
      }
    }
  }

  if (people[ix].is_moving) {
    update_person_xz(ix, et, 1.0f);
    update_person_theta(ix, et, people[ix].vtheta);
  }
}

int game_over(void) {
  unsigned i;

  if (killed_nontarget)
    return 1;

  for (i = 1; i < NUM_PEOPLE; ++i)
    if (people[i].is_alive)
      return 1;

  return 0;
}

static void choose_target(void) {
  if (game_over()) {
    target = 0;
    return;
  }

  do {
    target = rand() % NUM_NPCS + 1;
  } while (!people[target].is_alive);
}

void position_camera(void) {
  if (!target || !people[target].is_alive)
    choose_target();

  if (target) {
    glLoadIdentity();
    glTranslatef(-people[target].x, -0.5f, -people[target].z);
    glRotatef(people[target].theta, 0, 1, 0);
  }
}

void clear_screen(void) {
  if (killed_nontarget)
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  else
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


static const float body[][3] = {
  { -0.1f, 0.0f, -0.1f },
  { -0.1f, 0.0f, +0.1f },
  { -0.1f, 1.0f, +0.1f },
  { -0.1f, 1.0f, -0.1f },

  { +0.1f, 0.0f, -0.1f },
  { +0.1f, 0.0f, +0.1f },
  { +0.1f, 1.0f, +0.1f },
  { +0.1f, 1.0f, -0.1f },

  { -0.1f, 0.0f, -0.1f },
  { +0.1f, 0.0f, -0.1f },
  { +0.1f, 1.0f, -0.1f },
  { -0.1f, 1.0f, -0.1f },

  { -0.1f, 0.0f, +0.1f },
  { +0.1f, 0.0f, +0.1f },
  { +0.1f, 1.0f, +0.1f },
  { -0.1f, 1.0f, +0.1f },
};

static const float gun[][3] = {
  { -0.25f, 0.475f, 0.00f },
  { -0.25f, 0.475f, 0.75f },
  { -0.25f, 0.525f, 0.75f },
  { -0.25f, 0.525f, 0.00f },

  { +0.25f, 0.475f, 0.00f },
  { +0.25f, 0.475f, 0.75f },
  { +0.25f, 0.525f, 0.75f },
  { +0.25f, 0.525f, 0.00f },

  { -0.25f, 0.475f, 0.00f },
  { +0.25f, 0.475f, 0.00f },
  { +0.25f, 0.475f, 0.75f },
  { -0.25f, 0.475f, 0.75f },

  { -0.25f, 0.525f, 0.00f },
  { +0.25f, 0.525f, 0.00f },
  { +0.25f, 0.525f, 0.75f },
  { -0.25f, 0.525f, 0.75f },

  { -0.25f, 0.475f, 0.75f },
  { +0.25f, 0.475f, 0.75f },
  { +0.25f, 0.525f, 0.75f },
  { -0.25f, 0.525f, 0.75f },
};

static void draw_model(const float (*model)[3], unsigned cnt) {
  unsigned i;

  glBegin(GL_QUADS);
  for (i = 0; i < cnt; ++i)
    glVertex3fv(model[i]);
  glEnd();
}

static void position_person(unsigned ix) {
  glTranslatef(people[ix].x, 0, people[ix].z);
  glRotatef(people[ix].theta, 0, 1, 0);
}

static void draw_person(unsigned ix) {
  if (ix == target) return;

  if (ix == 0)
    glColor3f(1.0f, 0.0f, 0.0f);
  else
    glColor3f(1.0f, 1.0f, 1.0f);

  glPushMatrix();
  position_person(ix);
  draw_model(body, sizeof(body) / sizeof(body[0]));
  if (ix == 0) {
    glColor3f(0.3f, 0.3f, 0.3f);
    draw_model(gun, sizeof(gun) / sizeof(gun[0]));
  }
  glPopMatrix();
}

void draw_people(void) {
  unsigned i;

  for (i = 0; i < NUM_PEOPLE; ++i)
    draw_person(i);
}

void player_fire_weapon(void) {
  spawn_weapon(people[0].x, 0.5f, people[0].z, people[0].theta);
}

static void adjust_ctl(unsigned* ctl, int incr) {
  if (!incr && !*ctl) return;

  *ctl += (incr? +1 : -1);
}

void player_set_forward(int i) {
  adjust_ctl(&player_forward, i);
}

void player_set_backward(int i) {
  adjust_ctl(&player_backward, i);
}

void player_set_left(int i) {
  adjust_ctl(&player_left, i);
}

void player_set_right(int i) {
  adjust_ctl(&player_right, i);
}

int weapon_collides_with_person(float x, float z, float r) {
  unsigned i;
  float dx, dz, d;

  /* Add person radius to r, then square to get maximum square distance */
  r += 0.1f;
  r *= r;

  for (i = 1; i < NUM_PEOPLE; ++i) {
    if (people[i].is_alive) {
      dx = x - people[i].x;
      dz = z - people[i].z;
      d = dx*dx + dz*dz;

      if (d < r) {
        /* Collision */
        /* TODO: End game if this isn't the target */
        people[i].is_alive = 0;
        return 1;
      }
    }
  }

  return 0;
}
