/*
 * tree.c: list the tree of all open windows
 *
 * Usage: tree (click)
 */

#include "../include/tree.h"

struct tree {
  Window win;
  Window root;
  Window parent;
  Window *children;
  unsigned int count;
};

struct tree *query_tree(Display *dpy, Window win){
  struct tree *wintree = malloc(sizeof(struct tree));
  wintree->win = win;

  XQueryTree(
    dpy,
    win,
    &wintree->root,
    &wintree->parent,
    &wintree->children,
    &wintree->count
  );

  return wintree;
}

char *fetch_name(Display *dpy, Window win){
  char *out;
  XFetchName(dpy, win, &out);
  return out;
}

void prepend(int depth){
  for(int i=0;i<depth;i++){
    printf("  ");
  }
}

void print_children(Display *dpy, Window win, Window sel, int depth){
  prepend(depth);
  char *name = fetch_name(dpy, win);
  if(name == NULL){
    if(win == sel){
      printf(RED "%#x (You are here):\n", win);
    } else {
      printf(GREEN "%#x:\n", win);
    }
  } else {
    if(win == sel){
      printf(RED "%#x (You are here): %s\n", win, name);
    } else {
      printf(BLUE "%#x: %s\n", win, name);
    }
  }
  struct tree *t = query_tree(dpy, win);
  for(int i=0;i<t->count;i++){
    print_children(dpy, t->children[i], sel, depth+1);
  }
  if(!t->count){
    prepend(depth+1);
    printf(YELLOW "(none)\n");
  }

  free(t);
}
void get_tree(Display *dpy, Window sel){
  Window root = DefaultRootWindow(dpy);
  printf(CYAN "Window tree from window %#x (root):\n", root);
  print_children(dpy, root, sel, 0);
}

Window find_window(Display *dpy){
  Window root,
         win;
  int i;
  unsigned int u;

  XQueryPointer(
    dpy,
    DefaultRootWindow(dpy),
    &root,
    &win,
    &i, &i,
    &i, &i,
    &u
  );

  return win;
}

char *tree_usage(){
  return "tree (click)              List the tree of all open windows.";
}

int tree(Display *dpy, int argc, char **argv){
  int should_grab = 0,
      clicked = 0;
  Window sel;
  XEvent evt;

  if(argc > 1){
    if(strcmp(argv[1], "click") != 0){
      printf(CYAN "xmt tree: List the tree of all open windows.\n");
      printf(YELLOW "Optional arguments:\n");
      printf(GREEN "  click: Use the mouse the select a visible window\n");
      return -1;
    } else {
      should_grab = 1;
    }
  }

  if(should_grab){
    printf(MAGENTA "Please click a window.\n");

    XAllowEvents(dpy, AsyncBoth, CurrentTime);

    XGrabPointer(
      dpy,
      DefaultRootWindow(dpy),
      1,
      ButtonPressMask,
      GrabModeAsync,
      GrabModeAsync,
      None,
      None,
      CurrentTime
    );

    while(!clicked){
      XNextEvent(dpy, &evt);
      
      sel = find_window(dpy);
      
      get_tree(dpy, sel);
      clicked = 1;
    }
  } else {
    get_tree(dpy, 0);
  }

  return 0;
}
