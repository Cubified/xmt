#ifndef __IMG_H
#define __IMG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "ansi.h"

char *img_usage();
int img(Display *dpy, int argc, char **argv);

#endif /* __IMG_H */
