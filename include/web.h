#ifndef __WEB_H
#define __WEB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>

#include "../lib/miniz.h"

#include "base64.h"
#include "ws.h"
#include "ansi.h"

#define BLOCK_SIZE 40

enum {
  evt_key = 6
};

char *web_usage();
int web(Display *dpy, int argc, char **argv);

#endif
