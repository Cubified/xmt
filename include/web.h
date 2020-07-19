#ifndef __WEB_H
#define __WEB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "base64.h"
#include "ws.h"
#include "ansi.h"

char *web_usage();
int web(Display *dpy, int argc, char **argv);

#endif
