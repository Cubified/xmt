/*
 * img.c: Set the display's wallpaper to a given image
 *
 * Usage: img [path]
 */

#include "../include/img.h"

char *img_usage(){
  return "img  [path]:              Set the display's wallpaper to an image.";
}

int img(Display *dpy, int argc, char **argv){
  Pixmap pix;
  XImage *img;
  int x, y;

  if(argc < 2 ||
     strcmp(argv[1], "help") == 0){
    printf(CYAN "xmt img: Set the display's wallpaper to a given image.\n");
    printf(YELLOW "Required arguments:\n");
    printf(GREEN "  - [path]: A path to an image file (can be in any common format)\n");

    return -1;
  }

  pix = XCreatePixmap(
    dpy,
    DefaultRootWindow(dpy),
    XWidthOfScreen(DefaultScreenOfDisplay(dpy)),
    XHeightOfScreen(DefaultScreenOfDisplay(dpy)),
    DefaultDepth(dpy, DefaultScreen(dpy))
  );
  img = XCreateImage(
    dpy,
    DefaultVisual(dpy, DefaultScreen(dpy)),
    DisplayPlanes(dpy, DefaultScreen(dpy)),
    ZPixmap,
    0,
    malloc(100*100*sizeof(int)),
    100, 100,
    8, 0
  );

  for(y=0;y<100;y++){
    for(x=0;x<100;x++){
      img->data[(y*4)*100+x*4+0] = (char)255;
      img->data[(y*4)*100+x*4+1] = (char)255;
      img->data[(y*4)*100+x*4+2] = 0;
      img->data[(y*4)*100+x*4+3] = 0;
    }
  }

  XFillRectangle(
    dpy,
    pix,
    DefaultGC(dpy, DefaultScreen(dpy)),
    0, 0,
    XWidthOfScreen(DefaultScreenOfDisplay(dpy)),
    XHeightOfScreen(DefaultScreenOfDisplay(dpy))
  );
  XPutImage(
    dpy,
    pix,
    DefaultGC(dpy, DefaultScreen(dpy)),
    img,
    0, 0,
    0, 0,
    XWidthOfScreen(DefaultScreenOfDisplay(dpy)),
    XHeightOfScreen(DefaultScreenOfDisplay(dpy))
  );

  XSetWindowBackgroundPixmap(
    dpy,
    DefaultRootWindow(dpy),
    pix
  );

  XClearWindow(
    dpy,
    DefaultRootWindow(dpy)
  );

  XFree(img->data);
  XFree(img);

  return 0;
}
