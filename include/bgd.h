#ifndef __BGD_H
#define __BGD_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>

#include "ansi.h"
#include "color.h"

char *bgd_usage();
int bgd(Display *dpy, int argc, char **argv);

#endif /* __BGD_H */
