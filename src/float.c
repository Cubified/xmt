/*
 * float.c: A minimal flowing window manager
 *
 * Not as extensible or featureful as it could be,
 * but highly functional, minimal, and in line
 * with the goals of the multitool.
 *
 * Usage: float
 */

#include "../include/float.h"

/* Heavily inspired by i3-sensible-terminal */
void sensible_terminal(){
  char *list[] = {
    getenv("TERMINAL"),
    "x-terminal-emulator",
    "urxvt",
    "rxvt",
    "termit",
    "terminator",
    "Eterm",
    "aterm",
    "uxterm",
    "xterm",
    "mlterm",
    "sakura",
    "gnome-terminal",
    "roxterm",
    "xfce4-terminal",
    "termite",
    "lxterminal",
    "mate-terminal",
    "terminology",
    "st",
    "qterminal",
    "lilyterm",
    "tilix",
    "konsole",
    "guake",
    "tilda",
    "alacritty",
    "hyper"
  };
  char *argv[2];
  char cmd[256];
  int retval;
  int i;

  for(i=0;i<LENGTH(list);i++){
    if(list[i] != NULL){
      sprintf(cmd, "command -v \"%s\" > /dev/null 2>&1", list[i]);
      if(system(cmd) == 0){
        break;
      }
    }
  }

  if(fork() == 0){
    argv[0] = list[i];
    argv[1] = NULL;
    if(execvp(list[i], argv) != 0){
      printf(RED "Failed to execute " MAGENTA "%s" RED ".\n" RESET, list[i]);
    }
  }
}

void x_shutdown(int signo){
  printf(CYAN "floatwm shutting down.\n" RESET);
  exit(0);
}

int x_on_error(Display *dpy, XErrorEvent *e){
  if(e->error_code == 10){
    printf(RED "Error: Another window manager is already running.\n" RESET);
    x_shutdown(0);
  } else {
    char text_buf[256];
    XGetErrorText(dpy, e->error_code, text_buf, sizeof(text_buf));
    printf(RED "Error: %s\n" RESET, text_buf);
  }
  return 0;
}

int x_kill(Display *dpy, Window win){
  int protocols_count = 0,
      exists = 0;
  Atom proto  = XInternAtom(dpy, "WM_PROTOCOLS", 0);
  Atom delete = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
  Atom *protocols;
  XEvent evt;

  XGetWMProtocols(dpy, win, &protocols, &protocols_count);

  if(protocols == NULL){
    return 0;
  }

  protocols_count--;

  while(!exists){
    exists = (protocols[protocols_count] == delete);
    protocols_count--;
  }

  XFree(protocols);

  if(exists){
    evt.type = ClientMessage;
    evt.xclient.window = win;
    evt.xclient.message_type = proto;
    evt.xclient.format = 32;
    evt.xclient.data.l[0] = delete;
    evt.xclient.data.l[1] = CurrentTime;
    XSendEvent(dpy, win, 0, NoEventMask, &evt);
  }

  return exists;
}

void x_setup_binds(Display *dpy, Window win){
  XSelectInput(
    dpy,
    win,
    EnterWindowMask |
    SubstructureRedirectMask |
    SubstructureNotifyMask
  );

  XGrabKey(
    dpy,
    XKeysymToKeycode(dpy, XK_Return),
    Mod1Mask,
    win,
    0,
    GrabModeAsync,
    GrabModeAsync
  );
  XGrabKey(
    dpy,
    XKeysymToKeycode(dpy, XK_q),
    Mod1Mask|ShiftMask,
    win,
    0,
    GrabModeAsync,
    GrabModeAsync
  );
  XGrabKey(
    dpy,
    XKeysymToKeycode(dpy, XK_e),
    Mod1Mask|ShiftMask,
    win,
    0,
    GrabModeAsync,
    GrabModeAsync
  );

  if(win != DefaultRootWindow(dpy)){
    XGrabButton(
      dpy,
      Button1,
      Mod1Mask,
      win,
      True,
      Button1MotionMask |
      ButtonReleaseMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      XCreateFontCursor(dpy, XC_arrow)
    );
    XGrabButton(
      dpy,
      Button3,
      Mod1Mask,
      win,
      True,
      Button3MotionMask |
      ButtonReleaseMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      XCreateFontCursor(dpy, XC_arrow)
    );
  }
}

void x_manage_existing(Display *dpy){
  XConfigureRequestEvent evt;
  XWindowAttributes attr;
  Window root,
         parent;
  Window *children;
  unsigned int children_count;
  int n;

  XQueryTree(
    dpy,
    DefaultRootWindow(dpy),
    &root,
    &parent,
    &children,
    &children_count
  );

  for(n=0;n<children_count;n++){
    evt.display = dpy;
    evt.window = children[n];

    XGetWindowAttributes(dpy, children[n], &attr);

    if(attr.map_state == IsViewable &&
       !attr.override_redirect){
      XMapWindow(dpy, children[n]);
      x_setup_binds(dpy, children[n]);
      if(n == 0){
        XRaiseWindow(
          dpy,
          children[n]
        );
      }
    }
  }

  XFree(children);
}

void x_set_env(Display *dpy, Window root){
  XSetErrorHandler(&x_on_error);

  XChangeProperty(
    dpy,
    root,
    XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", 0),
    XA_WINDOW,
    32,
    PropModeReplace,
    (unsigned char *)&root,
    1
  );
  XChangeProperty(
    dpy,
    root,
    XInternAtom(dpy, "_NET_WM_NAME", 0),
    XA_STRING,
    8,
    PropModeReplace,
    (unsigned char *)"xmt float",
    10
  );

  XDefineCursor(
    dpy,
    root,
    XCreateFontCursor(dpy, XC_arrow)
  );

  signal(SIGINT,  x_shutdown);
  signal(SIGTERM, x_shutdown);
  signal(SIGQUIT, x_shutdown);
}

char *floatwm_usage(){
  return "float                     A minimal floating window manager.";
}

int floatwm(Display *dpy, int argc, char **argv){
  int i, throwaway;
  int running = 1;
  int start_x = -1;
  int start_y = -1;
  int start_w = -1;
  int start_h = -1;
  int action = -1;
  XWindowChanges wc;
  XEvent evt;
  Window root;
  Window focused = 0;

  if(argc > 1){
    printf(CYAN "xmt float: A minimal floating window manager.\n");
    printf(YELLOW "Keybinds:\n");
    printf(GREEN "  - Alt+Enter:              Start a terminal emulator (can be changed using the " MAGENTA "$TERMINAL" GREEN " environment variable)\n");
    printf(BLUE  "  - Alt+Shift+Q:            Close the currently focused window\n");
    printf(GREEN "  - Alt+Shift+E:            Exit the window manager\n");
    printf(BLUE  "  - Alt+Left Mouse Button:  Drag/move a window\n");
    printf(GREEN "  - Alt+Right Mouse Button: Resize a window\n" RESET);
    return -1;
  }

  printf(CYAN "floatwm starting up.\n" RESET);

  root = DefaultRootWindow(dpy);
  x_set_env(dpy, root);
  x_setup_binds(dpy, root);
  x_manage_existing(dpy);

  while(running){
    XNextEvent(dpy, &evt);
    switch(evt.type){
      case MapRequest:
        focused = evt.xmaprequest.window;
        XMapWindow(dpy, focused);
        XSetInputFocus(
          dpy,
          focused,
          RevertToParent,
          CurrentTime
        );
        x_setup_binds(dpy, focused);
        break;
      case UnmapNotify:
        if(focused == evt.xunmap.window){
          focused = root;
        }
        break;
      case ConfigureRequest:
        wc.x = evt.xconfigurerequest.x;
        wc.y = evt.xconfigurerequest.y;
        wc.width = evt.xconfigurerequest.width;
        wc.height = evt.xconfigurerequest.height;
        XConfigureWindow(
          dpy,
          evt.xconfigurerequest.window,
          CWX|CWY|CWWidth|CWHeight,
          &wc
        );
        break;
      case EnterNotify:
        if(focused != root &&
           focused != 0 &&
           focused != evt.xcrossing.window){
          XLowerWindow(
            dpy,
            focused
          );
        }
        focused = evt.xcrossing.window;
        XSetInputFocus(
          dpy,
          focused,
          RevertToParent,
          CurrentTime
        );
        XRaiseWindow(
          dpy,
          focused
        );
        break;
      case KeyPress:
        /* Potentially bad: These keycodes assume a US layout
         *
         * (I cannot say I suspect people will use this software,
         *  much less people with non-US layouts in particular)
         */
        switch(evt.xkey.keycode){
          case 36: /* Enter */
            if(evt.xkey.state == Mod1Mask){
              sensible_terminal();
            }
            break;
          case 24: /* Q */
            if(evt.xkey.state == (Mod1Mask|ShiftMask) &&
               focused != root &&
               focused != 0){
              XUnmapWindow(dpy, focused);
              if(!x_kill(dpy, focused)){
                XGrabServer(dpy);
                XSetCloseDownMode(dpy, DestroyAll);
                XKillClient(dpy, focused);
                XSync(dpy, 0);
                XUngrabServer(dpy);
              }
            }
            break;
          case 26: /* E */
            if(evt.xkey.state == (Mod1Mask|ShiftMask)){
              running = 0;
            }
            break;
        }
        break;
      case MotionNotify:
        if(action == Button1){
          wc.x = evt.xbutton.x_root-start_x;
          wc.y = evt.xbutton.y_root-start_y;
          XConfigureWindow(
            dpy,
            focused,
            CWX|CWY,
            &wc
          );
        } else if(action == Button3){
          wc.width = evt.xbutton.x+start_w-start_x;
          wc.height = evt.xbutton.y+start_h-start_y;
          XConfigureWindow(
            dpy,
            focused,
            CWWidth|CWHeight,
            &wc
          );
        }
        break;
      case ButtonPress:
        if(action == -1){
          focused = evt.xbutton.window;
          start_x = evt.xbutton.x;
          start_y = evt.xbutton.y;
          if(evt.xbutton.button == Button3){
            XGetGeometry(
              dpy,
              focused,
              (Window*)&throwaway,
              &throwaway, &throwaway,
              &start_w, &start_h,
              &throwaway,
              &throwaway
            );
          }
          action = evt.xbutton.button;
        }
        break;
      case ButtonRelease:
        if(action == evt.xbutton.button){
          start_x = -1;
          start_y = -1;
          start_w = -1;
          start_h = -1;
          action = -1;
        }
        break;
    }
  }

  x_shutdown(0);

  return 0;
}
