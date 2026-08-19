#ifndef __OLED_CORE__
#define __OLED_CORE__

// just for definitions ugh

#include <zephyr/drivers/i2c.h>
#include <stdint.h>

/* macros */
#define OLED_SIZE_W (128U)
#define OLED_SIZE_L (64U)
#define OLED_SCREEN_DIV (8U) // we divide size_w by this for horizontal

#define OLED_SIZE (OLED_SIZE_W * OLED_SIZE_L / OLED_SCREEN_DIV)

#define uint8 uint8_t
#define uint32 uint32_t

#define SET_OLED_OFF(x) oled_pwrdn()
#define ARRAY_LEN(X) sizeof(X) / sizeof(X[0])

extern const struct i2c_dt_spec *shared_dev;

// config stuff
enum {
  ADDR_MOD = 0,
  CVERT_MOD,
  VERT_AREA,
  COL_START_END,
  PAGE_START_END,
  MUX_RATIO,
  DISP_OFFS,
  DISP_STLINE,
  SEG_REMAP,
  COM_SCAN_DIR,
  COM_CFGS,
  CONTRAST_CTRL,
  OSC_FREQ,
  SCREEN_RAM_MOD
};

#define SET_ADDR_MODE    (1U << ADDR_MOD)
#define SET_CVERT_MODE   (1U << CVERT_MOD)
#define SET_VERT_AREA    (1U << VERT_AREA)
#define SET_COL_START_END  (1U << COL_START_END)
#define SET_PAGE_START_END (1U << PAGE_START_END)
#define SET_MUX_RATIO    (1U << MUX_RATIO)
#define SET_DISP_OFFS    (1U << DISP_OFFS)
#define SET_DISP_STLINE  (1U << DISP_STLINE)
#define SET_SEG_REMAP    (1U << SEG_REMAP),
#define SET_COM_SCAN_DIR   (1U << COM_SCAN_DIR)
#define SET_COM_CFGS     (1U << COM_CFGS)
#define SET_CONTRAST_CTRL  (1U << CONTRAST_CTRL)
#define SET_OSC_FREQ     (1U << OSC_FREQ)
#define SET_SCREEN_RAM_MOD (1U << SCREEN_RAM_MOD)

#define DEFAULT_CFG_FLAGS  (0)

typedef enum {
  ERR_I2C_GENERAL  = -3,
  ERR_I2C_NOTREADY = -2,
  ERR_I2C_OLED_NOT_FOUND,
  I2C_OLED_OK
} core_err_t;

typedef enum {
  HORIZ_MODE = 0,
  VERTL_MODE,
  PAGE_MODE,
  MODE_END
} oled_mem_mode_t;

typedef struct {
   uint8 direction;
   uint8 start_page_addr;
   uint8 frame_interval;
   uint8 end_page_addr;
   uint8 vert_scroll_offs;
} oled_cvert_scroll_t;

typedef struct {
  uint8 no_rows_top_area;
  uint8 no_rows_scroll_area;
} oled_vert_scroll_area_t;

/* only for vertical/horizontal addressing modes */
typedef struct {
  uint8 col_start_addr;
  uint8 col_end_addr;
} oled_col_start_end_t;

typedef struct {
  uint8 page_start_addr;
  uint8 page_end_addr;
} oled_page_start_end_t;

typedef struct {
  bool screen_on;
  bool chrgpmp_on;
  uint8 mux_rat;
  uint8 disp_offs;
  uint8 disp_line;
  uint8 screen_mod;
  oled_mem_mode_t mem_mode;
  oled_cvert_scroll_t cvert_mode;
  oled_vert_scroll_area_t scroll_area;
  oled_col_start_end_t col_start_end;
  oled_page_start_end_t page_start_end;
} oled_core_cfg_t;

// Function type define (for configs)
// Thank you stack exchange
typedef void *(*cfg_func_t)(void *);

// other function definitions
void oled_init_cfg(void);
core_err_t oled_enable_chrgpmp(void);
core_err_t oled_set_cfg(oled_core_cfg_t *new_cfg, uint32 cfg_flags);
core_err_t oled_set_addr_mode(oled_mem_mode_t mode);
core_err_t oled_disable_chrgpmp(void);
core_err_t oled_init(oled_core_cfg_t *new_cfg, uint32 cfg_flags);
core_err_t oled_pwrp_screen(void);
core_err_t oled_pwrdn_screen(void);
core_err_t oled_pwrdn(void);
core_err_t oled_send_bits(const uint8 *const fb, uint32 size_buffer, uint8 col_start);

#endif
