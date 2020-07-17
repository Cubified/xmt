#ifndef __TREE_H
#define __TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "ansi.h"

char *tree_usage();
int tree(Display *dpy, int argc, char **argv);

#endif
