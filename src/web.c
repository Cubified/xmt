/*
 * web.c: Forward the contents of the X display to a browser
 *
 * Usage: web [port]
 */

#include "../include/web.h"

#define HTTPSERVER_IMPL
#include "httpserver.h"

Display *dpy_global;
Damage   root_dmg;
char html_global[4096];
int  html_global_size;
int  dmg_evt, dmg_err;

void x_click(int button, int state, int x, int y){
  char buf[256];
  /*
  XEvent evt;
  memset(&evt, 0, sizeof(evt));
  */

  printf(YELLOW "Received " MAGENTA "%s" YELLOW " mouse%s event at position " MAGENTA "(%i, %i)" YELLOW ".\n" RESET, (button == Button1 ? "left" : "right"), (state ? "down" : "up"), x, y);
/*
  XLockDisplay(dpy_global);
  XWarpPointer(
    dpy_global,
    None,
    DefaultRootWindow(dpy_global),
    0, 0,
    0, 0,
    x, y
  );
  XFlush(dpy_global);

  evt.type = ButtonPress;
  evt.xbutton.type = ButtonPress;
  evt.xbutton.button = button;
  evt.xbutton.same_screen = True;
  evt.xbutton.subwindow = DefaultRootWindow(dpy_global);
  while(evt.xbutton.subwindow){
    evt.xbutton.window = evt.xbutton.subwindow;
    XQueryPointer(dpy_global, evt.xbutton.window,
                  &evt.xbutton.root, &evt.xbutton.subwindow,
                  &evt.xbutton.x_root, &evt.xbutton.y_root,
                  &evt.xbutton.x, &evt.xbutton.y,
                  &evt.xbutton.state
    );
  }
  XSendEvent(
    dpy_global,
    PointerWindow,
    True,
    ButtonPressMask,
    &evt
  );
  XFlush(dpy_global);
  usleep(10);

  evt.type = ButtonRelease;
  evt.xbutton.type = ButtonRelease;
  XSendEvent(
    dpy_global,
    PointerWindow,
    True,
    ButtonPressMask,
    &evt
  );
  XFlush(dpy_global);
  usleep(10);

  XUnlockDisplay(dpy_global);
  */

  sprintf(buf, "xdotool mousemove %i %i", x, y);
  system(buf);
  sprintf(buf, "xdotool %s %i", (state ? "mousedown" : "mouseup"), button);
  system(buf);
}

void x_key(int code, int state){
  char buf[256];
  printf(YELLOW "Received " MAGENTA "%c" YELLOW " key%s event.\n" RESET, code, (state ? "down" : "up"));
  sprintf(buf, "xdotool %s %c", (state ? "keydown" : "keyup"), code+32);
  system(buf);
}

void x_send(int fd, int x, int y, int w, int h){
  unsigned long compr_len;
  int x_i, y_i;
  char *buf,
       set[128];/*,
       *compr = malloc(compressBound(BLOCK_SIZE*BLOCK_SIZE*4));*/
  XImage *ximg;

  sprintf(set, "set%i,%i", x%BLOCK_SIZE, y%BLOCK_SIZE);
  ws_sendframe(fd, set, false);

  XLockDisplay(dpy_global);
  for(y_i=y%BLOCK_SIZE;y_i<h/BLOCK_SIZE;y_i++){
    for(x_i=x%BLOCK_SIZE;x_i<w/BLOCK_SIZE;x_i++){
      ximg = XGetImage(
        dpy_global,
        DefaultRootWindow(dpy_global),
        x_i*BLOCK_SIZE, y_i*BLOCK_SIZE,
        BLOCK_SIZE, BLOCK_SIZE,
        AllPlanes,
        ZPixmap
      );
      /*printf("%i\n", compress(compr, &compr_len, ximg->data, (BLOCK_SIZE*BLOCK_SIZE*4)+(BLOCK_SIZE*4)+3));
      buf = base64_encode(compr, compr_len, NULL);
      printf("%s\n", buf);*/
      buf = base64_encode(ximg->data, BLOCK_SIZE*BLOCK_SIZE*4, NULL);
      ws_sendframe(fd, buf, false);
      XDestroyImage(ximg);
      free(buf);
    }
    ws_sendframe(fd, "newline", false);
  }
  XUnlockDisplay(dpy_global);
/*  free(compr);*/
}

void ws_open(int fd){
  int w, h, count, i;
  char buf[64];
  XEvent evt;
  XDamageNotifyEvent *dmg;
  XserverRegion region;
  XRectangle *area, rect;

  w = XWidthOfScreen(DefaultScreenOfDisplay(dpy_global));
  h = XHeightOfScreen(DefaultScreenOfDisplay(dpy_global));

  printf(GREEN "New WebSocket connection established.\n");

  sprintf(
    buf,
    "size%i,%i,%i",
    w, h, BLOCK_SIZE
  );
  ws_sendframe(fd, buf, false);

  x_send(fd, 0, 0, w, h);

  if(fork() == 0){
    while(1){
      XNextEvent(dpy_global, &evt);
      if(evt.type == dmg_evt+XDamageNotify){
        dmg = (XDamageNotifyEvent*)&evt;
        region = XFixesCreateRegion(dpy_global, NULL, 0);
        XDamageSubtract(
          dpy_global,
          root_dmg,
          None,
          region
        );
        area = XFixesFetchRegion(dpy_global, region, &count);
        if(area){
          for(i=0;i<count;i++){
            rect = area[i];
            x_send(fd, rect.x, rect.y, rect.width, rect.height);
          }
        }
        XFixesDestroyRegion(dpy_global, region);
        XFree(area);
        usleep(50000);
      }
    }
  }
}

void ws_close(int fd){
}

void ws_msg(int fd, const unsigned char *msg){
  int type, state, x, y;

  sscanf(msg, "%i,%i,%i,%i", &type, &state, &x, &y);

  switch(type){
    case Button1:
    case Button3:
      x_click(type, state, x, y);
      break;
    case evt_key:
      x_key(state, x);
      break;
  }
}

void handle_request(struct http_request_s *req){
  printf(BLUE "Received new HTTP request.\n" RESET);

  struct http_response_s *res = http_response_init();
  http_response_status(res, 200);
  http_response_header(res, "Content-Type", "text/html");
  http_response_body(res, html_global, html_global_size);
  http_respond(req, res);
}

char *web_usage(){
  return "web  [port]               Forward the contents of the X display to a browser";
}

int web(Display *dpy, int argc, char **argv){
  int x, y, port = 8080;
  FILE *fp;
  struct ws_events evs;
  struct http_server_s *server;

  printf(RED "This tool is a work in progress, it will most likely not work as expected.\n" RESET);

  if(argc > 1){
    if(strcmp(argv[1], "help") == 0){
      printf(CYAN "xmt web: Forward the contents of the X display to a browser\n");
      printf(YELLOW "Optional arguments:\n");
      printf(GREEN "  - [port]: The port over which to serve the HTTP server\n" RESET);
      return -1;
    } else {
      sscanf(argv[1], "%i", &port);
    }
  }

  if(XDamageQueryExtension(dpy, &dmg_evt, &dmg_err) == False){
    printf(RED "Warning: Xdamage extension disabled, browser view will not update automatically.\n" RESET);
  } else {
    root_dmg = XDamageCreate(
      dpy,
      DefaultRootWindow(dpy),
      XDamageReportNonEmpty
    );
  }

  dpy_global = dpy;
  fp = fopen("web/web.html", "r");
  if(fp == NULL){
    printf(RED "Failed to open " MAGENTA "web/web.html" RED " for reading.\n" RESET);
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  html_global_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  fread(html_global, html_global_size, 1, fp);
  fclose(fp);

  printf(CYAN "Starting WebSocket server on port " MAGENTA "9000" CYAN " and HTTP server on port " MAGENTA "%i" CYAN ".\n" RESET, port);

  if(fork() == 0){
    evs.onopen = &ws_open;
    evs.onclose = &ws_close;
    evs.onmessage = &ws_msg;
    ws_socket(&evs, 9000);
  } else {
    server = http_server_init(port, handle_request);
    http_server_listen(server);
  }

  return 0;
}
