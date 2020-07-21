/*
 * web.c: Forward the contents of the X display to a browser
 *
 * Usage: web [port]
 */

#include "../include/web.h"

#define HTTPSERVER_IMPL
#include "httpserver.h"

Display *dpy_global;

void ws_open(int fd){
  unsigned long compr_len;
  int x, y, w, h;
  char *buf = malloc(64),
       *compr = malloc(compressBound(40*40*4));
  XImage *ximg;

  w = XWidthOfScreen(DefaultScreenOfDisplay(dpy_global));
  h = XHeightOfScreen(DefaultScreenOfDisplay(dpy_global));

  printf(GREEN "New connection established.\n");

  sprintf(
    buf,
    "size%i,%i,%i",
    w, h, BLOCK_SIZE
  );
  ws_sendframe(fd, buf, false);
  free(buf);

  for(y=0;y<h/BLOCK_SIZE;y++){
    for(x=0;x<w/BLOCK_SIZE;x++){
      ximg = XGetImage(
        dpy_global,
        DefaultRootWindow(dpy_global),
        x*BLOCK_SIZE, y*BLOCK_SIZE,
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
  /*free(compr);*/
}

void ws_close(int fd){
}

void ws_msg(int fd, const unsigned char *msg){
  int x, y;
  XEvent evt;

  sscanf(msg, "click%i,%i", &x, &y);

  printf(YELLOW "Received click event at position " MAGENTA "(%i, %i)" YELLOW ".\n" RESET, x, y);

  XWarpPointer(
    dpy_global,
    None,
    DefaultRootWindow(dpy_global),
    0, 0,
    0, 0,
    x, y
  );
  XFlush(dpy_global);

  memset(&evt, 0, sizeof(evt));
  evt.type = ButtonPress;
  evt.xbutton.type = ButtonPress;
  evt.xbutton.button = Button1;
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
}

void handle_request(struct http_request_s *req){
  struct http_response_s *res = http_response_init();
  http_response_status(res, 200);
  http_response_header(res, "Content-Type", "text/plain");
  http_response_body(res, "hello world", 8);
  http_respond(req, res);
}

char *web_usage(){
  return "web  [port]               Forward the contents of the X display to a browser";
}

int web(Display *dpy, int argc, char **argv){
  int x, y, port = 8080;
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

  dpy_global = dpy;

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
