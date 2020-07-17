/*
 * xfade.c: nicely fade between two background colors in X11
 *
 * Usage: xfade [run speed] [from] [to]
 *              (All arguments optional)
 */

#include "../include/fade.h"

/*****************************/
/* FADE                      */
/*****************************/
char *fade_usage(){
  return "fade [speed] [from] [to]  Fade the root window's background color.";
}

int fade(Display *dpy, int argc, char **argv){
  Window root;
  color from, to, current;
  int current_direction, direction, run_speed;

  /*
   * TODO: Refactor? The fall-through solution is
   * fairly elegant, but the double switch is not
   */
  switch(argc){
    case 4: /* Fall through */
      color_from_string(&to, argv[3]);
    case 3: /* Fall through */
      color_from_string(&from, argv[2]);
    case 2:
      if(strcmp(argv[1], "help") == 0){
        printf(CYAN "xmt fade: Fade the root window's background between two colors.\n");
        printf(YELLOW "Optional arguments:\n");
        printf(GREEN "  [speed]: The time (in microseconds) between individual steps of the color change\n");
        printf(MAGENTA "  [from]: An HTML-styled color triple (e.g. 0xabcdef, #ABCDEF, ABCdef)\n");
        printf(GREEN "  [to]: An HTML-styled color triple (e.g. 0xabcdef, #ABCDEF, ABCdef)\n" RESET);
        return -1;
      }

      sscanf(argv[1], "%i", &run_speed);
      /* run_speed = strtol(argv[1], (void*)argv[2], 10); */
      break;
    case 1:
      run_speed = 100000;
      break;
  }

  switch(argc){
    case 1: /* Fall through */
    case 2: /* Fall through */
      color_rand(&from, 0x55);
    case 3:
      color_rand(&to, 0x55);
      break;
  }

  srand(time(NULL));

  current = from;
  root = DefaultRootWindow(dpy);
  while(!color_compare(&current, &to)){
    XSetWindowBackground(
      dpy,
      root,
      color_to_hex(&current)
    );
    XClearWindow(dpy, root);
    XFlush(dpy);

    color_step(&current, &to, 0x1);

    usleep(run_speed);
  }

  return 0;
}
