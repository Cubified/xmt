/*
 * img.c: Set the display's wallpaper to a given image
 *
 * Usage: img [path]
 */

#include "../include/img.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

char *img_usage(){
  return "img  [path]               Set the display's wallpaper to an image.";
}

int img(Display *dpy, int argc, char **argv){
  Pixmap pix;
  XImage *ximg;
  unsigned char *data;
  int x, y, w, h, n;

  if(argc < 2 ||
     strcmp(argv[1], "help") == 0){
    printf(CYAN "xmt img: Set the display's wallpaper to a given image.\n");
    printf(YELLOW "Required arguments:\n");
    printf(GREEN "  - [path]: A path to an image file (can be in any common format)\n" RESET);

    return -1;
  }

  data = stbi_load(argv[1], &w, &h, &n, 0);
  if(data == NULL){
    printf(RED "Error: Unable to open file \"%s\" for reading.\n", argv[1]);
    return 1;
  }

  pix = XCreatePixmap(
    dpy,
    DefaultRootWindow(dpy),
    XWidthOfScreen(DefaultScreenOfDisplay(dpy)),
    XHeightOfScreen(DefaultScreenOfDisplay(dpy)),
    DefaultDepth(dpy, DefaultScreen(dpy))
  );
  ximg = XCreateImage(
    dpy,
    DefaultVisual(dpy, DefaultScreen(dpy)),
    DisplayPlanes(dpy, DefaultScreen(dpy)),
    ZPixmap,
    0,
    malloc(w*h*sizeof(int)),
    w, h,
    8, 0
  );

  for(y=0;y<h;y++){
    for(x=0;x<w;x++){
      /* This may break for images with 1, 2, or 4 components */
      XPutPixel(
        ximg,
        x, y,
        data[(y*w*n)+(x*n)] << 16  |
        data[(y*w*n)+(x*n)+1] << 8 |
        data[(y*w*n)+(x*n)+2]
      );
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
    ximg,
    0, 0,
    0, 0,
    w, h
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

  stbi_image_free(data);
  XDestroyImage(ximg);

  return 0;
}
