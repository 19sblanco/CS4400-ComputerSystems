/*******************************************
 * Solutions for the CS:APP Performance Lab
 ********************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following student struct 
 */
student_t student = {
  "Steven Blanco",     /* Full name */
  "u1375369@umail.utah.edu",  /* Email address */
};

/***************
 * COMPLEX KERNEL
 ***************/

/******************************************************
 * Your different versions of the complex kernel go here
 ******************************************************/

/* 
 * naive_complex - The naive baseline version of complex 
 */
char naive_complex_descr[] = "naive_complex: Naive baseline implementation";
__attribute__((always_inline)) void naive_complex(int dim, pixel *src, pixel *dest)
{
  int i, j;

  for(i = 0; i < dim; i++)
    for(j = 0; j < dim; j++)
    {

      dest[RIDX(dim - j - 1, dim - i - 1, dim)].red = ((int)src[RIDX(i, j, dim)].red +
						      (int)src[RIDX(i, j, dim)].green +
						      (int)src[RIDX(i, j, dim)].blue) / 3;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].green = ((int)src[RIDX(i, j, dim)].red +
							(int)src[RIDX(i, j, dim)].green +
							(int)src[RIDX(i, j, dim)].blue) / 3;
      
      dest[RIDX(dim - j - 1, dim - i - 1, dim)].blue = ((int)src[RIDX(i, j, dim)].red +
						       (int)src[RIDX(i, j, dim)].green +
						       (int)src[RIDX(i, j, dim)].blue) / 3;

    }
}


/* 
 * complex - Your current working version of complex
 * IMPORTANT: This is the version you will be graded on
 */
char complex_descr[] = "complex: Current working version";
__attribute__((always_inline)) void complex(int dim, pixel *src, pixel *dest)
{
  int i, j, ii, jj, temp, w;
 
  w = 8;
  for (i = 0; i < dim; i+=w) 
  {
   for (j = 0; j < dim; j+=w)
   {
     for (ii = i; ii < i+w; ii++)
     {
       for (jj = j; jj < j+w; jj++)
       {
         temp = ((int)src[RIDX(ii, jj, dim)].red +
     	  				      (int)src[RIDX(ii, jj, dim)].green +
     	  				      (int)src[RIDX(ii, jj, dim)].blue) / 3;
 
         dest[RIDX(dim - jj - 1, dim - ii - 1, dim)].red = temp;
         dest[RIDX(dim - jj - 1, dim - ii - 1, dim)].green = temp;      
         dest[RIDX(dim - jj - 1, dim - ii - 1, dim)].blue = temp;
       }
     }
   }
  }
}


/*********************************************************************
 * register_complex_functions - Register all of your different versions
 *     of the complex kernel with the driver by calling the
 *     add_complex_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

__attribute__((always_inline)) void register_complex_functions() {
  add_complex_function(&complex, complex_descr);
  add_complex_function(&naive_complex, naive_complex_descr);
}


/***************
 * MOTION KERNEL
 **************/

/***************************************************************
 * Various helper functions for the motion kernel
 * You may modify these or add new ones any way you like.
 **************************************************************/


/* 
 * weighted_combo - Returns new pixel value at (i,j) 
 */
__attribute__((always_inline)) static pixel weighted_combo(int dim, int i, int j, pixel *src) 
{
  int ii, jj;
  pixel current_pixel;

  int red, green, blue;
  red = green = blue = 0;

  int num_neighbors = 0;


  for(ii=0; ii < 3; ii++)
    for(jj=0; jj < 3; jj++) 
      if ((i + ii < dim) && (j + jj < dim)) 
      {
        num_neighbors++;
        red += (int) src[RIDX(i+ii,j+jj,dim)].red;
        green += (int) src[RIDX(i+ii,j+jj,dim)].green;
        blue += (int) src[RIDX(i+ii,j+jj,dim)].blue;
      }

  
  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);
  
  return current_pixel;
}
__attribute__((always_inline)) static pixel three_two(int dim, int i, int j, pixel *src) 
{
  pixel current_pixel;
  int red, green, blue;
  int num_neighbors = 6;

  red = (int) src[RIDX(i,j,dim)].red + (int) src[RIDX(i,j+1,dim)].red + (int) src[RIDX(i,j+2,dim)].red + (int) src[RIDX(i+1,j,dim)].red + (int) src[RIDX(i+1,j+1,dim)].red + (int) src[RIDX(i+1,j+2,dim)].red;

  green = (int) src[RIDX(i,j,dim)].green + (int) src[RIDX(i,j+1,dim)].green + (int) src[RIDX(i,j+2,dim)].green + (int) src[RIDX(i+1,j,dim)].green + (int) src[RIDX(i+1,j+1,dim)].green + (int) src[RIDX(i+1,j+2,dim)].green;
 
  blue = (int) src[RIDX(i,j,dim)].blue + (int) src[RIDX(i,j+1,dim)].blue + (int) src[RIDX(i,j+2,dim)].blue + (int) src[RIDX(i+1,j,dim)].blue + (int) src[RIDX(i+1,j+1,dim)].blue + (int) src[RIDX(i+1,j+2,dim)].blue;

  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);
  
  return current_pixel;
}
__attribute__((always_inline)) static pixel three_one(int dim, int i, int j, pixel *src) 
{
  pixel current_pixel;
  int red, green, blue;
  int num_neighbors = 3;
  
  red = (int) src[RIDX(i,j,dim)].red + (int) src[RIDX(i,j+1,dim)].red + (int) src[RIDX(i,j+2,dim)].red;

  green = (int) src[RIDX(i,j,dim)].green + (int) src[RIDX(i,j+1,dim)].green + (int) src[RIDX(i,j+2,dim)].green;

  blue = (int) src[RIDX(i,j,dim)].blue + (int) src[RIDX(i,j+1,dim)].blue + (int) src[RIDX(i,j+2,dim)].blue;

  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);
  
  return current_pixel;
}
__attribute__((always_inline)) static pixel two_three(int dim, int i, int j, pixel *src) 
{
  pixel current_pixel;
  int red, green, blue;
  int num_neighbors = 6;

  red = (int) src[RIDX(i,j,dim)].red + (int) src[RIDX(i,j+1,dim)].red + (int) src[RIDX(i+1,j,dim)].red + (int) src[RIDX(i+1,j+1,dim)].red + (int) src[RIDX(i+2,j,dim)].red + (int) src[RIDX(i+2,j+1,dim)].red;

  green = (int) src[RIDX(i,j,dim)].green + (int) src[RIDX(i,j+1,dim)].green + (int) src[RIDX(i+1,j,dim)].green + (int) src[RIDX(i+1,j+1,dim)].green + (int) src[RIDX(i+2,j,dim)].green + (int) src[RIDX(i+2,j+1,dim)].green;

  blue = (int) src[RIDX(i,j,dim)].blue + (int) src[RIDX(i,j+1,dim)].blue + (int) src[RIDX(i+1,j,dim)].blue + (int) src[RIDX(i+1,j+1,dim)].blue + (int) src[RIDX(i+2,j,dim)].blue + (int) src[RIDX(i+2,j+1,dim)].blue;

  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);
  
  return current_pixel;
}
__attribute__((always_inline)) static pixel one_three(int dim, int i, int j, pixel *src) 
{
  pixel current_pixel;
  int red, green, blue;
  int num_neighbors = 3;
  
  red = (int) src[RIDX(i,j,dim)].red + (int) src[RIDX(i+1,j,dim)].red + (int) src[RIDX(i+2,j,dim)].red;

  green = (int) src[RIDX(i,j,dim)].green + (int) src[RIDX(i+1,j,dim)].green + (int) src[RIDX(i+2,j,dim)].green;

  blue = (int) src[RIDX(i,j,dim)].blue + (int) src[RIDX(i+1,j,dim)].blue + (int) src[RIDX(i+2,j,dim)].blue;

  current_pixel.red = (unsigned short) (red / num_neighbors);
  current_pixel.green = (unsigned short) (green / num_neighbors);
  current_pixel.blue = (unsigned short) (blue / num_neighbors);
  
  return current_pixel;
}

/******************************************************
 * Your different versions of the motion kernel go here
 ******************************************************/

__attribute__((always_inline)) static pixel get_col(int dim, int i, int j, pixel *src, int* r, int* g, int* b) 
{
  
  *r = (int) src[RIDX(i,j,dim)].red + (int) src[RIDX(i+1,j,dim)].red + (int) src[RIDX(i+2,j,dim)].red;

  *g = (int) src[RIDX(i,j,dim)].green + (int) src[RIDX(i+1,j,dim)].green + (int) src[RIDX(i+2,j,dim)].green;

  *b = (int) src[RIDX(i,j,dim)].blue + (int) src[RIDX(i+1,j,dim)].blue + (int) src[RIDX(i+2,j,dim)].blue;
}
/*
 * naive_motion - The naive baseline version of motion 
 */
char naive_motion_descr[] = "naive_motion: Naive baseline implementation";
void naive_motion(int dim, pixel *src, pixel *dst) 
{
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(i, j, dim)] = weighted_combo(dim, i, j, src);
}
/*
 * motion - Your current working version of motion. 
 * IMPORTANT: This is the version you will be graded on
 */
char motion_descr[] = "motion: Current working version";
void motion(int dim, pixel *src, pixel *dst) 
{
  int i, j, ii, jj;
  int fr, fg, fb, sr, sg, sb, tr, tg, tb;
  
  pixel current_pixel;
  for (i = 0; i < dim-2; i++)
  {
    get_col(dim, i, 0, src, &fr, &fg, &fb);
    get_col(dim, i, 1, src, &sr, &sg, &sb);
    get_col(dim, i, 2, src, &tr, &tg, &tb);

    current_pixel.red = (unsigned short) ((int)(fr + sr + tr) / 9);
    current_pixel.green = (unsigned short) ((int)(fg + sg + tg) / 9);
    current_pixel.blue = (unsigned short) ((int)(fb + sb + tb) / 9);
    dst[RIDX(i, 0, dim)] = current_pixel;

    for (j = 1; j < dim-2; j++)
    {
      fr = sr;
      fg = sg;
      fb = sb;

      sr = tr;
      sg = tg;
      sb = tb;

      get_col(dim, i, j+2, src, &tr, &tg, &tb);

      current_pixel.red = (unsigned short) ((int)(fr + sr + tr) / 9);
      current_pixel.green = (unsigned short) ((int)(fg + sg + tg) / 9);
      current_pixel.blue = (unsigned short) ((int)(fb + sb + tb) / 9);
      dst[RIDX(i, j, dim)] = current_pixel;
    }
  }


  ii = dim - 2;
  jj = dim - 2;

  for (i = ii; i < dim - 1; i++)
    for (j = 0; j < dim - 2; j++) 
      dst[RIDX(i, j, dim)] = three_two(dim, i, j, src);

  for (i = ii+1; i < dim; i++)
    for (j = 0; j < dim - 2; j++)
      dst[RIDX(i, j, dim)] = three_one(dim, i, j, src);

  for (i = 0; i < dim - 2; i++)
    for (j = jj; j < dim -1; j++) 
      dst[RIDX(i, j, dim)] = two_three(dim, i, j, src);

  for (i = 0; i < dim - 2; i++)
    for (j = jj+1; j < dim; j++)
      dst[RIDX(i, j, dim)] = one_three(dim, i, j, src);

  
  dst[RIDX(dim-2, dim-2, dim)] = weighted_combo(dim, dim-2, dim-2, src);
  dst[RIDX(dim-2, dim-1, dim)] = weighted_combo(dim, dim-2, dim-1, src);
  dst[RIDX(dim-1, dim-2, dim)] = weighted_combo(dim, dim-1, dim-2, src);
  dst[RIDX(dim-1, dim-1, dim)] = weighted_combo(dim, dim-1, dim-1, src);
}


/********************************************************************* 
 * register_motion_functions - Register all of your different versions
 *     of the motion kernel with the driver by calling the
 *     add_motion_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_motion_functions() {
  add_motion_function(&motion, motion_descr);
  add_motion_function(&naive_motion, naive_motion_descr);
}
