#ifndef __FADE_H
#define __FADE_H

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "ansi.h"
#include "color.h"

/*****************************/
/* FADE                      */
/*****************************/
char *fade_usage();
int fade(Display *dpy, int argc, char **argv);

#endif /* __FADE_H */
