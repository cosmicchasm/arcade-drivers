/*
 * Author: Aidan S
 *
 * Common header file for greater portability among
 * other projects
 */

#ifndef __GRAPHICS__
#define __GRAPHICS__

#include <stdint.h>
#include "graphics_core.h"
#include "oled_core.h"

/* Device-specific macros */
#define BYTE_FROM_XY(X,Y) XY2OL_BYTE(X,Y)
#define BIT_FROM_XY(X,Y)  (1<<(XY2OL_BIT(X,Y)))

#define SCREEN_LIMIT_X OLED_SIZE_W
#define SCREEN_LIMIT_Y OLED_SIZE_L
#define SCREEN_DIV 		 OLED_SCREEN_DIV

#define SCREEN_SIZE OLED_SIZE

#define PIX_TYPE uint8_t

void set_fb_pixel(int x, int y, uint8_t *const fb);
void clr_fb_pixel(int x, int y, uint8_t *const fb);

/* glue functions for portability */
int write_display(const uint8_t *const fb, uint32_t sz);

#endif
