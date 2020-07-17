#ifndef __COLOR_H
#define __COLOR_H

#include <stdio.h>
#include <stdlib.h>

/*****************************/
/* COLOR STRUCT              */
/*****************************/
typedef struct color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} color;

/*****************************/
/* MATH UTILITIES            */
/*****************************/
int sign(int val);
int min(int a, int b);

/*****************************/
/* COLOR FUNCTIONS           */
/*****************************/
unsigned int color_to_hex(color *c);
int color_compare(color *a, color *b);
void color_rand(color *c, char max);
void color_step(color *src, color *dest, int size);
void color_from_string(color *dest, char *str);

#endif /* __COLOR_H */
