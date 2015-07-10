#ifndef __PIXWT_HPP__
#define __PIXWT_HPP__

#include <cstdio>
#include <cmath>

/* round a floating point number to the nearest whole number */
float mwb_round(float x)
{
   if ( x < 0 ) return( (int)(x-0.5) );
   else return( (int)(x+0.5) );
}


/* cgetrng is called to determine how to iterate when integrating
 * over a circle.  The circle's center is at (xc,yc), and its radius is r.
 * For pixels with x-coordinate x, those in the intervals [y0,y1) and [y2,y3)
 * are on or near the circle.  Those in the interval [y1,y2) are definitely
 * inside; all others are definitely outside.
 *    Of course, the routine can be called to determine an interval for fixed
 * y by calling it as cgetrng(yc,xc,r,y,x0,x1,x2,x3).
 *    The appropriate way to integrate over a circle is therefore as follows:
 * cgetrng(xc,yc,r,mwb_round(xc),y0,y1,y2,y3);
 * for (y = y0; y <= y3-1; y=y+1) {
 *    cgetrng (yc, xc, r, y, x0, x1, x2, x3);
 *    for (x = x0; x <= x1-1; x=x+1) sum = sum + value(x,y)*pixwt(xc,yc,r,x,y);
 *    for (x = x1; x <= x2-1; x=x+1) sum = sum + value(x,y);
 *    for (x = x2; x <= x3-1; x=x+1) sum = sum + value(x,y)*pixwt(xc,yc,r,x,y);
 *    }
 */

/* rounded up to increase size of uncertain areas  */
#define sqrt2   1.414213563

void cgetrng(float xc, float yc, float r,   // Center and radius of the circle.
             int x,                         // X coordinate of the pixels.
             int *y0, int *y1, int *y2, int *y3) // Boundaries of circle in Y.
{
   float   a,b,outdsq,outd,ind;
   float   indsq=-1.0;

   if (r <= 0) /* then it misses completely */ outdsq = -1;
   else {
      a = r*r + 0.5 - (x-xc)*(x-xc);
      b = sqrt2 * r;
      outdsq = a + b;
      if(b < 1) /*then indsq would be invalid--say no interior*/ indsq= -1;
      else /* formula works */ indsq = a-b; }

   if (outdsq < 0) /* complete miss */ *y0 = *y1 = *y2 = *y3 = mwb_round(yc);
   else /* there is some intersection */ {
      outd = sqrt(outdsq);
      *y0 = ceil(yc-outd);
      *y3 = floor(yc+outd) + 1;
      if (indsq < 0) /* no interior */ *y1 = *y2 = mwb_round(yc);
      else /* there is a certain interior */ {
         ind = sqrt(indsq);
         *y1 = ceil(yc-ind);
         *y2 = floor(yc+ind) + 1;}
      }
}

/* compute the area within an arc of a circle.  The arc is defined by
 * the two points (x,y0) and (x,y1) in the following manner:  The circle
 * is of radius r and is positioned at the origin.  The origin and each
 * individual point define a line which intersect the circle at some
 * point.  The angle between these two points on the circle measured
 * from y0 to y1 defines the sides of a wedge of the circle.  The area
 * returned is the area of this wedge.  If the area is traversed clockwise
 * the the area is negative, otherwise it is positive. */

static float arc (float x,   // X coordinate of the two points.
                  float y0,  // Y coordinate of the first point.
                  float y1,  // Y coordinate of the second point.
                  float r)   // radius of the circle.
{
   return( 0.5 * r*r * (atan( y1/x) - atan( y0/x) ) );
}

/* compute the area of a triangle defined by the origin and two points,
 * (x,y0) and (x,y1).  This is a signed area.  If y1 > y0 then the area
 * will be positive, otherwise it will be negative.
 */

static float chord (float x,  // X coordinate of the two points.
                    float y0, // Y coordinate of the first point.
                    float y1) // Y coordinate of the second point.
{
   return( 0.5 * x * (y1-y0) );
}

/* Compute the area of intersection between a triangle and a circle.
 * The circle is centered at the origin and has a radius of r.  The
 * triangle has verticies at the origin and at (x,y0) and (x,y1).
 * This is a signed area.  The path is traversed from y0 to y1.  If
 * this path takes you clockwise the area will be negative.
 */

static float oneside (float x,  // X coordinate of the two points.
                      float y0, // Y coordinate of the first point.
                      float y1, // Y coordinate of the second point.
                      float r)  // radius of the circle
{
   float   yh;

   if (x == 0) return(0);
   else if ( fabs(x) >=  r ) return( arc(x,y0,y1,r) ); 
   else {
      yh = sqrt(r*r-x*x);
      if (y0 <= -yh) {
         if (y1 <= -yh) return( arc(x,y0,y1,r) );
         else if (y1 <= yh) return( arc(x,y0,-yh,r) + chord(x,-yh,y1) );
         else return( arc(x,y0,-yh,r) + chord(x,-yh,yh)
                                      + arc(x,yh,y1,r) ); }
      else if (y0 < yh) {
         if (y1 < -yh) return( chord(x,y0,-yh) + arc(x,-yh,y1,r) );
         else if (y1 <= yh) return( chord(x,y0,y1) );
         else return( chord(x,y0,yh) + arc(x,yh,y1,r) ); }
      else {
         if (y1<-yh) return( arc(x,y0,yh,r) + chord(x,yh,-yh)
                                            + arc(x,-yh,y1,r) );
         else if (y1 < yh) return( arc(x,y0,yh,r) + chord(x,yh,y1) );
         else return( arc(x,y0,y1,r) ); }
   }
}

/* Compute the area of overlap between a circle and a rectangle. */
float intarea (float xc, float yc, // Center of the circle
               float r,            // Radius of the circle
               float x0,           // x of corner 1 of rectangle
               float x1,           // x of opposite corner of rectangle
               float y0,           // y of corner 1 of rectangle
               float y1)           // y of opposite corner of rectangle
{

x0 -= xc;   y0 -= yc;  /* Shift the objects so that circle is at the orgin. */
x1 -= xc;   y1 -= yc;

return( oneside(x1,y0,y1,r) + oneside(y1,-x1,-x0,r)
        + oneside(-x0,-y1,-y0,r) + oneside(-y0,x0,x1,r) );

}

/* Compute the fraction of a unit pixel that is interior to a circle.
 * The circle has a radius r and is centered at (xc,yc).  The center of
 * the unit pixel (length of sides = 1) is at (x,y).
 */

float pixwt_c (float xc, float yc, float r, // Center and radius of circle.
             int   x,  int   y)           // Coordinates of pixel.
{
   return( intarea( xc, yc, r, x-0.5, x+0.5, y-0.5, y+0.5 ) );
}

#endif
