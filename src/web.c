/*
 * web.c: Forward the contents of the X display to a browser
 *
 * Usage: web [port]
 */

#include "../include/web.h"

#define HTTPSERVER_IMPL
#include "httpserver.h"

int w, h;
XImage *ximg;

void ws_open(int fd){
  size_t throwaway;
  ws_sendframe(fd, base64_encode(ximg->data, w*h*ximg->depth, &throwaway), false);
}

void ws_close(int fd){
}

void ws_msg(int fd, const unsigned char *msg){
  /* Mouse events here, eventually */
}

void handle_request(struct http_request_s *req){
  struct http_response_s *res = http_response_init();
  http_response_status(res, 200);
  http_response_header(res, "Content-Type", "text/plain");
  http_response_body(res, "ayy lmao", 8);
  http_respond(req, res);
}

char *web_usage(){
  return "web  [port]               Forward the contents of the X display to a browser";
}

int web(Display *dpy, int argc, char **argv){
  int x, y, port = 8080;
  struct ws_events evs;
  struct http_server_s *server;

  if(argc > 1){
    if(strcmp(argv[1], "help") == 0){
      printf(CYAN "xmt web: Forward the contents of the X display to a browser\n");
      printf(YELLOW "Optional arguments:\n");
      printf(GREEN "  - [port]: The port over which to serve the HTTP server\n" RESET);
    } else {
      sscanf(argv[1], "%i", &port);
    }
  }

  w = XWidthOfScreen(DefaultScreenOfDisplay(dpy));
  h = XHeightOfScreen(DefaultScreenOfDisplay(dpy));
  ximg = XGetImage(
    dpy,
    DefaultRootWindow(dpy),
    0, 0,
    w, h,
    AllPlanes,
    ZPixmap
  );

  if(fork() == 0){
    evs.onopen = &ws_open;
    evs.onclose = &ws_close;
    evs.onmessage = &ws_msg;
    ws_socket(&evs, 9000);
  } else {
    server = http_server_init(port, handle_request);
    http_server_listen(server);
  }

  XDestroyImage(ximg);

  return 0;
}
