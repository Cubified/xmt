#ifndef __FLOAT_H
#define __FLOAT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#include "ansi.h"

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

char *floatwm_usage();
int floatwm(Display *dpy, int argc, char **argv);

#endif
