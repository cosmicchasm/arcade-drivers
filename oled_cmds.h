/*
 * Author: Aidan Stanford
 * If we wanted to do pong, we could use the different scrolling modes!
 */

#ifndef __OLED_CMDS__
#define __OLED_CMDS__

/* Macros */

// Definitions of data/command bytes

/* configure scrolling */
#define SSD1306_RHS  (0X26) // right horizontal scroll
#define SSD1306_LHS  (0X27) // left horizontal scroll

#define SSD1306_VRHS (0x29) // vertical and right horizontal scroll
#define SSD1306_VLHS (0X2A) // vertical and left horizontal scroll

/* start scrolling */
#define SSD1306_START_SCROLL (0X2F)

// ram must be rewritten after this
#define SSD1306_STOP_SCROLL (0X2E) // stop scrolling from 26/27/29/2A

#define SSD1306_NEXT_BYTE_IS_CMD (0X00) // designates next byte as cmd
#define SSD1306_NEXT_BYTE_IS_DAT (0XC0) // designates next byte as data
#define SSD1306_ALL_DAT_BYTE     (0X40) // designates all following bytes as data
#define SSD1306_ALL_CMD_BYTE     (0X00) // designates all following bytes as cmds

// Definitions of commands
#define SSD1306_ALL_DISP_ON_PERS (0XA4) // whole display on, persist ram contents
#define SSD1306_ALL_DISP_ON    (0XA5) // whole display on, no ram persistence

// 0 -> ram mode, 1 -> all on
#define SSD1306_SET_SCREEN_MODE(X) (((X) == 0) ? SSD1306_ALL_DISP_ON_PERS : SSD1306_ALL_DISP_ON)

#define SSD1306_SET_DISP_NRM   (0XA6) // set display mode normal
#define SSD1306_SET_DISP_INV   (0XA7) // set display mode normal

#define SSD1306_DISP_ON       (0XAF)
#define SSD1306_DISP_OFF      (0XAE)

#define SSD1306_CHPMP_CMD     (0X8D) // charge pump command
#define SSD1306_CHPMP_ENA     (0X14) // enable charge pump
#define SSD1306_CHPMP_DIS     (0X10) // disable charge pump

enum {
  STARTEND_ADDR_PAGE_0 = 0,
  STARTEND_ADDR_PAGE_1,
  STARTEND_ADDR_PAGE_2,
  STARTEND_ADDR_PAGE_3,
  STARTEND_ADDR_PAGE_4,
  STARTEND_ADDR_PAGE_5,
  STARTEND_ADDR_PAGE_6,
  STARTEND_ADDR_PAGE_7 = 7
};

enum {
  FRAME_INTERVAL_5_SET = 0,
  FRAME_INTERVAL_64_SET,
  FRAME_INTERVAL_128_SET,
  FRAME_INTERVAL_256_SET,
  FRAME_INTERVAL_3_SET,
  FRAME_INTERVAL_4_SET,
  FRAME_INTERVAL_25_SET,
  FRAME_INTERVAL_2_SET = 7
};

/* Addressing mode commands */
// Not currently implementing page addressing mode--don't see a use case rn

#define SSD1306_SET_ADDR_MODE (0X20)
#define SSD1306_SET_COL_ADDR_HV  (0X21) // only for horizontal/vertical addressing
#define SSD1306_SET_PAG_ADDR_HV  (0X22) // send page start/end addresses
// #define SSD1306_SET_PAG_ADDR_PG  (0X) B0-B7

/* Display-related commands */
// |x| <= 63, sets the horizontal line to start scanning
// reset value: 0X00
#define SSD1306_SET_DISPLAY_START(x) (0X40 | (x))
#define SSD1306_SET_SEGMAP_SEG0      (0XA0) // sets column address 0 to SEG0
#define SSD1306_SET_SEGMAP_SEG127    (0XA1) // sets column address 127 to SEG0
#define SSD1306_SET_MUX_RAT      	 (0XA8) // must be followed with ratio value
#define SSD1306_SET_COM_SCANDIR_NRM  (0XC0) // scans from COM0 to COM[n-1] where
                      // n = multiplex ratio
#define SSD1306_SET_DISPLAY_OFFSET   (0XD3) // must be followed by offset value

/* Timing-related commands */
#define SSD1306_SET_OSC_FREQ     (0XD5) // must be followed by 8-bit divider

#define SSD1306_NOP          (0XE3)

#define SSD1306

#endif
