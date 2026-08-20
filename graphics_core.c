/*
 * Author: Aidan S
 * 
 * File for graphics-related commands
 *
 * Relies on oled_core for size stuff
 */

#include <stdbool.h>
#include <stdint.h>

#include "graphics_common.h"

#define ABS(x)	 ((x) < 0 ? (-x) : (x))

// naive and a bit slow due to the modulus operation
static int gcd(int a, int b) {
  // safeguard I guess
  if ((a == 0) || (b == 0)) {
    return 1;
  }

	int t1 = MIN(a,b), t2 = MAX(a,b), t;
	while (t1) {
		t2 %= t1;

		t = t2;
		t2 = t1;
		t1 = t;
	}
	return t2;
}

static bool is_past(int rise, int run, pos_t *p1, pos_t *p2) {
  bool ret;

  // handle x case first
  ret = (run < 0) ? (p1->x > p2->x) : (run == 0) ? true : (p1->x < p2->x);
  ret = ret && ((rise < 0) ? (p1->y > p2->y) : (rise == 0) ? true : (p1->y < p2->y));

  return !ret;
}

// Draw a line on the frame buffer
// TODO: do two segments of y = mx + b to hopefully achieve
// greater 'resolution'
// It will still look janky for lines whose slopes aren't even multiples
// of pi/4 rad, so maybe I should log a warning or something
// Or, since we have an FPU, I could use floats and then cast them
// to integers at the end
void screen_draw_line(pos_t *p1, pos_t *p2, uint8_t *arr, int size) {
  bool helper;
  pos_t p1_cpy = *p1;
  pos_t p2_cpy = *p2;

	int tot = 0, rise = p2_cpy.y - p1_cpy.y, run = p2_cpy.x - p1_cpy.x;

  // increment by 1 since we reference by 0 (?)
  run = (run < 0) ? run - 1 : (run == 0) ? run : run + 1;
  rise = (rise < 0) ? rise - 1 : (rise == 0) ? rise : rise + 1;

	int g_cd = gcd(ABS(run), ABS(rise));

  // TODO: find the midpoint to create two line segments

	// reduce these to find the smallest steps we have to walk in
  // each direction
  if (rise == 0) {
    run = 1;
  } else if (run == 0) {
    rise = 1;
  } else {
    rise /= g_cd;
    run /= g_cd;
  }
	
  // set the initial bit
  set_fb_pixel(p1_cpy.x, p1_cpy.y, arr);

  helper = (p1_cpy.x != p2_cpy.x) || (p1_cpy.y != p2_cpy.y);
  while (helper && (tot < size)) {
		// create the line here by stepping first horizontally, then vertically
		for (int i = 0; i < ABS(run); i++) {
			p1_cpy.x = (run < 0) ? (p1_cpy.x - 1) : (p1_cpy.x + 1);
      set_fb_pixel(p1_cpy.x, p1_cpy.y, arr);
		}

		for (int i = 0; i < ABS(rise); i++) {
			p1_cpy.y = (rise < 0) ? (p1_cpy.y - 1) : (p1_cpy.y + 1);
      set_fb_pixel(p1_cpy.x, p1_cpy.y, arr);
		}

    // recalculate helper
    helper = ((p1_cpy.x != p2_cpy.x) || (p1_cpy.y != p2_cpy.y)) && !is_past(rise, run, &p1_cpy, &p2_cpy);
		++tot;
	}

	// should be it
}
