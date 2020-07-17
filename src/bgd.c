/*
 * bgd.c: set the root window's background color
 *
 * Usage: bgd [color]
 */

#include "../include/bgd.h"

char *bgd_usage(){
  return "bgd  [color]              Set the root window's background color.";
}

int bgd(Display *dpy, int argc, char **argv){
  color col;

  if(argc < 2){
    printf(RED "Usage: xmt bgd [color]\n" RESET);
    return 1;
  } else if(strcmp(argv[1], "help") == 0){
    printf(CYAN "xmt bgd: Set the root window's background color.\n");
    printf(YELLOW "Required arguments:\n");
    printf(GREEN "  [color]: An HTML-styled color triple (e.g. 0xabcdef, #ABCDEF, ABCdef)\n");
    printf(MAGENTA "           (Can be " CYAN "\"rand\"" MAGENTA " for a random color)\n" RESET);
    return -1;
  } else if(strcmp(argv[1], "rand") == 0){
    srand(time(NULL));

    color_rand(&col, 0x55);
  } else {
    color_from_string(&col, argv[1]);
  }

  XSetWindowBackground(
    dpy,
    DefaultRootWindow(dpy),
    color_to_hex(&col)
  );
  XClearWindow(
    dpy,
    DefaultRootWindow(dpy)
  );

  return 0;
}
