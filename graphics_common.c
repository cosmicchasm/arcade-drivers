/*
 * Author: Aidan S
 *
 * Common function definitions for graphics driver
 */

#include "graphics_common.h"

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
