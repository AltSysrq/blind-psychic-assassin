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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <SDL_opengl.h>
#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "person.h"
#include "weapon.h"
#include "floor.h"

int main(int argc, char** argv) {
  const SDL_VideoInfo* vidinfo;
  SDL_Surface* screen;
  float vheight;
  int running = 1;
  Uint32 last_update, now;
  char wasd[4] = { 'w', 'a', 's', 'd' };
  SDL_Event evt;
  unsigned i;

  /* Check arguments.
   * We only support one, which must be four characters long. On anything else
   * (but none, where we assume the default), print help and exit. This
   * conveniently includes "-h", "-help", "--help", "/?", and most other things
   * someone might be inclined to use to request usage.
   */
  if (argc > 2 || (argc == 2 && strlen(argv[1]) != sizeof(wasd))) {
    fprintf(stderr, "Usage: %s [WASD]\n", argv[0]);
    fprintf(stderr,
            "  If given, WASD is a four-character string specifying alternate\n"
            "  keybindings for up, left, down, and right, in that order.\n"
            "  These MUST be ASCII characters, and should be letters only,\n"
            "  though other characters may work as well.\n"
            "  Eg, if you used the Neo2 keyboard layout, you might run\n"
            "    %s VUIA\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  if (2 == argc)
    for (i = 0; i < sizeof(wasd); ++i)
      wasd[i] = tolower(argv[1][i]);
 
  /* Initialise */
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO)) {
    fprintf(stderr, "Unable to initialise SDL: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  /* Shut SDL down once the program completes */
  atexit(SDL_Quit);

  /* Set video mode up */
  vidinfo = SDL_GetVideoInfo();
  if (!vidinfo) {
    fprintf(stderr, "Unable to get current video mode info: %s\n",
            SDL_GetError());
    return EXIT_FAILURE;
  }

  vheight = vidinfo->current_h / (float)vidinfo->current_w;
  screen = SDL_SetVideoMode(vidinfo->current_w,
                            vidinfo->current_h,
                            0,
                            SDL_OPENGL | SDL_FULLSCREEN);
  if (!screen) {
    fprintf(stderr, "Unable to create SDL screen: %s\n",
            SDL_GetError());
    return EXIT_FAILURE;
  }

  /* Some WMs, such as early versions of kwin4, need to be reminded that
   * fullscreen windows get input.
   */
  SDL_WM_GrabInput(SDL_GRAB_ON);

  /* Set OpenGL up */
  glViewport(0, 0, vidinfo->current_w, vidinfo->current_h);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_DENSITY, 0.25f);
  glFogf(GL_FOG_START, 1.0f);
  glFogf(GL_FOG_END, 128.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-0.5, 0.5, -vheight/2, vheight/2, 0.5f, 64.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* Init game */
  srand(time(0));
  init_people();

  /* Loop until quit */
  last_update = SDL_GetTicks();
  while (running) {
    now = SDL_GetTicks();

    /* Update-draw cycle */
    clear_screen();
    if (!game_over()) {
      update_people(now - last_update);
      update_weapon(now - last_update);
      position_camera();
      draw_floor();
      draw_people();
      draw_weapon();
      last_update = now;
    }

    /* Update screen */
    glFinish();
    SDL_GL_SwapBuffers();

    /* Read events */
    while (SDL_PollEvent(&evt)) {
      switch (evt.type) {
      case SDL_QUIT:
        running = 0;
        break;

      case SDL_KEYDOWN:
      case SDL_KEYUP:
        /* Releasing ESC terminates program */
        if (evt.key.keysym.sym == SDLK_ESCAPE &&
            evt.type == SDL_KEYUP)
          running = 0;
        /* Pressing SPACE or BACKSPACE fire weapon */
        else if ((evt.key.keysym.sym == SDLK_SPACE ||
                  evt.key.keysym.sym == SDLK_BACKSPACE) &&
                 evt.type == SDL_KEYDOWN)
          player_fire_weapon();
        /* Directional controls */
        else if (evt.key.keysym.sym == SDLK_UP ||
                 evt.key.keysym.sym == (unsigned char)wasd[0])
          player_set_forward(evt.type == SDL_KEYDOWN);
        else if (evt.key.keysym.sym == SDLK_LEFT ||
                 evt.key.keysym.sym == (unsigned char)wasd[1])
          player_set_left(evt.type == SDL_KEYDOWN);
        else if (evt.key.keysym.sym == SDLK_DOWN ||
                 evt.key.keysym.sym == (unsigned char)wasd[2])
          player_set_backward(evt.type == SDL_KEYDOWN);
        else if (evt.key.keysym.sym == SDLK_RIGHT ||
                 evt.key.keysym.sym == (unsigned char)wasd[3])
          player_set_right(evt.type == SDL_KEYDOWN);

        break;
      }
    }

    /* Delay for 16ms to not use too much CPU */
    SDL_Delay(16);
  }

  return EXIT_SUCCESS;
}
