/*
 * color.c: color utilities used in several tools
 */

#include "../include/color.h"

/*****************************/
/* MATH UTILITIES            */
/*****************************/
int sign(int val){
  if(val == 0){
    return 0;
  }
  return abs(val)/val;
}

int min(int a, int b){
  return (a < b ? a : b);
}

/*****************************/
/* COLOR FUNCTIONS           */
/*****************************/
unsigned int color_to_hex(color *c){
  return (
    c->r << 16 |
    c->g << 8  |
    c->b
  );
}

int color_compare(color *a, color *b){
  return (
    a->r == b->r &&
    a->g == b->g &&
    a->b == b->b
  );
}

void color_rand(color *c, char max){
  c->r = rand()%max;
  c->g = rand()%max;
  c->b = rand()%max;
}

void color_step(color *src, color *dest, int size){
  src->r += sign(dest->r-src->r)*min(size, abs(dest->r-src->r));
  src->g += sign(dest->g-src->g)*min(size, abs(dest->g-src->g));
  src->b += sign(dest->b-src->b)*min(size, abs(dest->b-src->b));
}

void color_from_string(color *dest, char *str){
  /* unsigned int temp = strtol(str, (void*)str+6, 16); */
  unsigned int temp;
  sscanf(str, "%x", &temp);
  dest->r = temp >> 16;
  dest->g = temp >> 8 & 0xff;
  dest->b = temp & 0xff;
}
