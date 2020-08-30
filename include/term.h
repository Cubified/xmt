#ifndef __TERM_H
#define __TERM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>

char *term_usage();
int term(Display *dpy, int argc, char **argv);

#endif
