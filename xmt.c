/*
 * xmt.c: the X11 multitool
 *
 * Usage: xmt [command] [args]
 */

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>

#include "include/ansi.h"

#include "include/fade.h"
#include "include/bgd.h"
#include "include/tree.h"
#include "include/img.h"
#include "include/float.h"

/*****************************/
/* PREPROCESSOR              */
/*****************************/
#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

/*****************************/
/* TYPEDEFS AND STRUCTS      */
/*****************************/
typedef int (*func)();
typedef char *(*usage)();

struct tool {
  char *name;
  func  exec;
  usage help;
};

struct tool tools[] = {
  {"fade",  fade,    fade_usage},
  {"bgd",   bgd,     bgd_usage },
  {"tree",  tree,    tree_usage},
  {"img",   img,     img_usage},
  {"float", floatwm, floatwm_usage}
};

/*****************************/
/* MULTITOOL                 */
/*****************************/
struct tool *find_tool(char *str){
  int i;
  for(i=0;i<LENGTH(tools);i++){
    if(strcmp(tools[i].name, str) == 0){
      return &tools[i];
    }
  }
  return NULL;
}

int main(int argc, char **argv){
  Display *dpy;
  struct tool *cmd;
  int i, out;

  if(argc < 2 ||
     (cmd = find_tool(argv[1])) == NULL){
    printf(CYAN "xmt: The X11 multitool\n");
    printf(YELLOW "Usage: xmt [command] [args]\n");
    printf(MAGENTA "Available commands are:\n");
    for(i=0;i<LENGTH(tools);i++){
      if(i % 2 == 0) printf(GREEN "  - xmt %s\n", tools[i].help());
      else printf(BLUE "  - xmt %s\n", tools[i].help());
    }
    printf(YELLOW "All commands accept " CYAN "\"help\"" YELLOW " as their first argument for additional information.\n" RESET);
    return 0;
  }

  dpy = XOpenDisplay(NULL);
  if((argc < 3 || strcmp(argv[2], "help") != 0) &&
      dpy == NULL){
    printf(RED "Error: Failed to open display.\n" RESET);
    return 1;
  }

  out = cmd->exec(dpy, argc-1, &argv[1]);

  if(out > -1){
    XCloseDisplay(dpy);
  }

  return out;
}
