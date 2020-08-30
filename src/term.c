/*
 * term.c: Forward the contents of the X display to a terminal
 *
 * Usage: term
 */

#include "../include/term.h"

Display *term_dpy;
Damage  root_dmg;
int w, h,
    tw, th,
    rw, rh,
    is_halting,
    dmg_evt, dmg_err;

/*
 * Help message
 */
void help(){
  printf("\x1b[36mxmt term: A terminal-based X server viewer.\n\x1b[38;2;0;200;0mIf this text is green, truecolor is supported by this terminal. If not, xmt term may not work properly.\n\x1b[0m");
  exit(0);
}

/*
 * Handle interrupt and shut down
 * gracefully
 */
void shutdown(int signo){
  if(!is_halting){
    is_halting = 1; /* Prevent double free due to multiple interrupts being fired */
    /* Disable mouse, show the cursor */
    printf("\x1b[?1003l\x1b[?25h");
    XCloseDisplay(term_dpy);
    exit(0);
  }
}

/* 
 * Disable echo (printing of typed characters)
 * and canonical mode (stdin being read one line at a time
 * rather than one byte at a time)
 */
void raw(){
  struct termios raw;
  tcgetattr(0, &raw);
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(0, TCSAFLUSH, &raw);
}

/*
 * Obtain the contents of the X display,
 * draw them to the terminal
 */
void draw(){
  int x, y;
  unsigned long p;
  XImage *img;

  img = XGetImage(
    term_dpy,
    DefaultRootWindow(term_dpy),
    0, 0,
    w, h,
    AllPlanes,
    ZPixmap
  );

  /* Enable mouse, hide the cursor, set the cursor position to the top left corner */
  printf("\x1b[?1003h\x1b[?25l\x1b[0;0H");
  for(y=0;y<th;y++){
    for(x=0;x<tw;x++){
      p = XGetPixel(img, rw*x, rh*y);
      /* Set background color using truecolor (RGB) */
      printf(
        "\x1b[48;2;%i;%i;%im ",
        (char)(p >> 16),
        (char)((p >> 8) & 0xff),
        (char)(p & 0xff)
      );
    }
  }
  
  XDestroyImage(img);
}

/*
 * Adjust view window on terminal resize
 */
void resize(int signo){
  struct winsize s;
  ioctl(1, TIOCGWINSZ, &s);
  tw = s.ws_col;
  th = s.ws_row;
  rw = (w/tw);
  rh = (h/th);
  draw(0, 0);
}

/*
 * Send a mouse event
 */
void mouse(int button, int state, int x, int y){
  /*
  char buf[256];
  sprintf(buf, "xdotool mousemove %i %i", rw*x, rh*y);
  system(buf);
  sprintf(buf, "xdotool %s %i", (state == 1 ? "mousedown" : "mouseup"), button);
  system(buf);
  */
}

/*
 * Send a keyboard event
 */
void keyboard(int state, char c){
  /*
  char buf[256];
  sprintf(buf, "xdotool key %c", c);
  system(buf);
  */
}

/*
 * Handle mouse and keyboard events
 * in a loop
 */
void loop(){
  int i, count;
  char c, buf[6];
  XEvent evt;
  /*XDamageNotifyEvent *dmg;*/
  XserverRegion region;
  XRectangle *area;/*, rect;*/

  /* Read from both the X display as well as stdin */
  int n_ready_fds;
  struct timeval tv;
  fd_set in_fds;

  FD_ZERO(&in_fds);
  FD_SET(0, &in_fds);
  FD_SET(ConnectionNumber(term_dpy), &in_fds);
  tv.tv_usec = 0;
  tv.tv_sec = 10;

  n_ready_fds = select(2, &in_fds, NULL, NULL, &tv);
  if(n_ready_fds > 0){
    if(FD_ISSET(0, &in_fds)){
      if((c = getchar()) == 0x1b){
        read(0, buf, 6);
        if(buf[1] == 0x5b &&
           buf[2] == 0x4d &&
           (buf[3] == 0x20 ||
            buf[3] == 0x22 ||
            buf[3] == 0x23)){
          mouse(buf[3] == 0x22, buf[3] == 0x20, buf[4], buf[5]);
          getchar();getchar();getchar();getchar();getchar(); /* This is a terrible, terrible hack */
        }
      } else if(c != '\0'){
        keyboard(1, c);
      }
    }
    if(evt.type == dmg_evt+XDamageNotify){
      /*dmg = (XDamageNotifyEvent*)&evt;*/
      region = XFixesCreateRegion(term_dpy, NULL, 0);
      XDamageSubtract(
        term_dpy,
        root_dmg,
        None,
        region
      );
      area = XFixesFetchRegion(term_dpy, region, &count);
      if(area){
        for(i=0;i<count;i++){
          /*rect = area[i];*/
          draw();
          /*draw(rect.x, rect.y);*/
        }
      }
      XFixesDestroyRegion(term_dpy, region);
      XFree(area);
      sleep(1);
    }
  } else if(n_ready_fds == 0){
    draw();
  }

  while(XPending(term_dpy)){
    XNextEvent(term_dpy, &evt);
  }
}

/*
 * Set up signal handlers (Ctrl+C, window resize)
 */
void sighandlers(){
  is_halting = 0;
  signal(SIGINT,  shutdown);
  signal(SIGKILL, shutdown);
  signal(SIGQUIT, shutdown);
  signal(SIGWINCH,  resize);
}

/*
 * Set up X
 */
int x(){
  w = XWidthOfScreen(DefaultScreenOfDisplay(term_dpy));
  h = XHeightOfScreen(DefaultScreenOfDisplay(term_dpy));

  XDamageQueryExtension(term_dpy, &dmg_evt, &dmg_err);
  root_dmg = XDamageCreate(
    term_dpy,
    DefaultRootWindow(term_dpy),
    XDamageReportNonEmpty
  );

  return 0;
}

char *term_usage(){
  return "term                      Forward the contents of the X display to a terminal";
}

int term(Display *dpy, int argc, char **argv){
  term_dpy = dpy;

  if(argc > 1){ help(); }
  if(x(term_dpy)){ return 1; }
  sighandlers();
  raw();
  resize(0);
  draw();
  while(1){ loop(); }

  /* Unreachable */
  shutdown(0);

  return 0;
}
