#ifndef __GRAPHICS_CORE__
#define __GRAPHICS_CORE__

#include <stdint.h>
#include "oled_core.h"

/* Helper macro for graphics */
// This should help us reorder vanilla frame buffers into the format
// for the OLED screen. Note that this macro will only work in 
// horizontal addressing mode

/**
 * @brief Converts from Cartesian grid to OLED frame buffer
 * format
 * @details This macro takes the x- and y-position of an
 *          attribute as it would appear in a normal frame buffer
 *          and computes the converted position within the OLED's
 *          frame buffer. It returns the appropriate byte
 *          to write to (another macro needed for the bit)
 * @param X The x-position of the attribute
 * @param Y The y-position of the attribute
 * @return The byte number to write to
 */
// X position will give you column, Y will give you page
// page = (ARENA_TOP_POS - Y) / 8
#define XY2OL_BYTE(X,Y) (((((OLED_SIZE_L-1)-(Y)) / \
                             OLED_SCREEN_DIV)*OLED_SIZE_W) \
                             + (X&(OLED_SIZE_W-1)))

// Basically just 7 - (Y % 8) (7 - because we want to reverse)
#define XY2OL_BIT(X,Y) ((OLED_SCREEN_DIV-1)-((Y) & \
                             0X7))

/* Structure to define a point on a 2D plane */
typedef struct {
	int x;
	int y;
} pos_t;

/* Function declarations */
void screen_draw_line(pos_t *p1, pos_t *p2, uint8_t *arr, int size);

#endif
