/*
 * Author: Aidan S
 *
 * Common function definitions for graphics driver
 */

#include <stdint.h>

#include "graphics_common.h"

void set_fb_pixel(int x, int y, PIX_TYPE *const fb) {
  if ((x < 0) || (x >= SCREEN_LIMIT_X) || 
      (y < 0) || (y >= SCREEN_LIMIT_Y)) {
    return;
  } else {
    fb[BYTE_FROM_XY(x,y)] |= BIT_FROM_XY(x,y);
  }
}

void clr_fb_pixel(int x, int y, PIX_TYPE *const fb) {
  if ((x < 0) || (x >= SCREEN_LIMIT_X) || 
      (y < 0) || (y >= SCREEN_LIMIT_Y)) {
    return;
  } else {
    fb[BYTE_FROM_XY(x,y)] &= ~BIT_FROM_XY(x,y);
  }
}

int write_display(const PIX_TYPE *const fb, uint32_t sz) {
	/* will guard this based on display type later */
	return oled_send_bits(fb, sz, 0);
}
