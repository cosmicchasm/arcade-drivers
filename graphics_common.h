/*
 * Author: Aidan S
 *
 * Common header file for greater portability among
 * other projects
 */

#ifndef __GRAPHICS__
#define __GRAPHICS__

#include "graphics_core.h"
#include "oled_core.h"

/* Device-specific macros */
#define BYTE_FROM_XY(X,Y) XY2OL_BYTE(X,Y)
#define BIT_FROM_XY(X,Y)  (1<<(XY2OL_BIT(X,Y)))

#define SCREEN_LIMIT_X OLED_SIZE_W
#define SCREEN_LIMIT_Y OLED_SIZE_L
#define SCREEN_DIV 		 OLED_SCREEN_DIV

/* "Portable" graphic functions (may change with different core headers) */
__inline void set_fb_pixel(int x, int y, uint8_t *const fb) {
  if ((x < 0) || (x >= SCREEN_LIMIT_X) || 
      (y < 0) || (y >= SCREEN_LIMIT_Y)) {
    return;
  } else {
    fb[BYTE_FROM_XY(x,y)] |= BIT_FROM_XY(x,y);
  }
}

__inline void clr_fb_pixel(int x, int y, uint8_t *const fb) {
  if ((x < 0) || (x >= SCREEN_LIMIT_X) || 
      (y < 0) || (y >= SCREEN_LIMIT_Y)) {
    return;
  } else {
    fb[BYTE_FROM_XY(x,y)] &= ~BIT_FROM_XY(x,y);
  }
}
#endif
