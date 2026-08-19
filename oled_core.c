/*
 * Author: Aidan Stanford
 */

 /* Zephyr includes */
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>

/* STDLIB includes */
#include <stdbool.h>

/* User includes */
#include "oled_core.h"
#include "oled_cmds.h"

// Register log module for Zephyr
LOG_MODULE_REGISTER(oled_core);

/* Static variables */
static oled_core_cfg_t oled_cfg;

static uint8 local_frame_buffer[1+OLED_SIZE];

// gotta turn the structs into pointers unfortunately
// returns true if the structs are bytewise equal

// Set the static cfg to reset values
void oled_init_cfg(void) {
  oled_cfg.mem_mode = PAGE_MODE;

  oled_cfg.cvert_mode.direction = 0;
  oled_cfg.cvert_mode.start_page_addr = 0;
  oled_cfg.cvert_mode.frame_interval = 0;
  oled_cfg.cvert_mode.end_page_addr = 0;
  oled_cfg.cvert_mode.vert_scroll_offs = 0;

  oled_cfg.scroll_area.no_rows_top_area = 0;
  oled_cfg.scroll_area.no_rows_scroll_area = 0;

  oled_cfg.col_start_end.col_start_addr = 0;
  oled_cfg.col_start_end.col_end_addr = 0;

  oled_cfg.page_start_end.page_start_addr = 0;
  oled_cfg.page_start_end.page_end_addr = 0;

  oled_cfg.screen_on = false;
  oled_cfg.chrgpmp_on = false;
}

// changing oled_set_cfg to use update flags (should make it faster)
core_err_t oled_set_cfg(oled_core_cfg_t *new_cfg, uint32 cfg_flags) {
  // make sure that the bus is ready
  if (!device_is_ready(shared_dev->bus)) {
    return ERR_I2C_NOTREADY;
  }

  // XXX: I know there's probably an elegant way to do this with
  // for loops or macros, but if i were to come up with it, the
  // code would be impossible to read
  // iterate through the config functions
  if (cfg_flags & SET_ADDR_MODE) {
    uint8 msg[3] = {SSD1306_ALL_CMD_BYTE, SSD1306_SET_ADDR_MODE,
      new_cfg->mem_mode};

    if (0 != i2c_write_dt(shared_dev, msg, 3)) {
      LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }

    // change the global config
    oled_cfg.mem_mode = new_cfg->mem_mode;
  }

  if (cfg_flags & SET_CVERT_MODE) {
    uint8 msg[7] = {SSD1306_ALL_CMD_BYTE, new_cfg->cvert_mode.direction,
      0X00, new_cfg->cvert_mode.start_page_addr, new_cfg->cvert_mode.frame_interval,
      new_cfg->cvert_mode.end_page_addr, new_cfg->cvert_mode.vert_scroll_offs};

    if (0 != i2c_write_dt(shared_dev, msg, 7)) {
      LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }

    // change the global config
    oled_cfg.cvert_mode = new_cfg->cvert_mode;
  }
  
  if (cfg_flags & SET_MUX_RATIO) {
    // do a check of the mux ratio rq
    if (new_cfg->mux_rat >= 64) {
      LOG_ERR("mux ratio %d exceeds limits, line %d function %s", new_cfg->mux_rat, __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }
    uint8 msg[3] = {SSD1306_ALL_CMD_BYTE, SSD1306_SET_MUX_RAT, new_cfg->mux_rat};
    if (0 != i2c_write_dt(shared_dev, msg, 3)) {
      LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }

    // change the global config
    oled_cfg.mux_rat = new_cfg->mux_rat;
  }

  if (cfg_flags & SET_DISP_OFFS) {
    // do a check of the offset rq
    if (new_cfg->disp_offs >= 64) {
      LOG_ERR("display offset %d exceeds limits, line %d function %s", new_cfg->disp_offs, __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }
    uint8 msg[3] = {SSD1306_ALL_CMD_BYTE, SSD1306_SET_DISPLAY_OFFSET, new_cfg->disp_offs};
    if (0 != i2c_write_dt(shared_dev, msg, 3)) {
      LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }

    // change the global config
    oled_cfg.disp_offs = new_cfg->disp_offs;
  }

  if (cfg_flags & SET_DISP_STLINE) {
    // do a check of the start line variable
    if (new_cfg->disp_line >= 64) {
      LOG_ERR("display offset %d exceeds limits, line %d function %s", new_cfg->disp_offs, __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }
    uint8 msg[2] = {SSD1306_NEXT_BYTE_IS_CMD, SSD1306_SET_DISPLAY_START(new_cfg->disp_line)};
    if (0 != i2c_write_dt(shared_dev, msg, 2)) {
      LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }

    // change the global config
    oled_cfg.disp_line = new_cfg->disp_line;
  }

  if (cfg_flags & SET_SCREEN_RAM_MOD) {
    // no check...?
    uint8 msg[2] = {SSD1306_NEXT_BYTE_IS_CMD, SSD1306_SET_SCREEN_MODE(new_cfg->screen_mod)};
    if (0 != i2c_write_dt(shared_dev, msg, 2)) {
      LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
      return ERR_I2C_GENERAL;
    }

    // change the global config
    oled_cfg.screen_mod = new_cfg->screen_mod;
  }

  // at least for the initial configuration recommended in the application note,
  // the rest are fine on reset, so we won't consider them (yet)
  return I2C_OLED_OK;
}

// Enable the charge pump
// arguments: none
core_err_t oled_enable_chrgpmp(void) {
  // make sure that the bus is ready
  if (!device_is_ready(shared_dev->bus)) {
    return ERR_I2C_NOTREADY;
  }

  // check if device screen is on--turn off if so
  // if (!oled_cfg.chrgpmp_on) {
  //     SET_OLED_OFF();
  //     oled_cfg.chrgpmp_on = true;
  // }

  // send the command here
  uint8 msg[3] = {SSD1306_ALL_CMD_BYTE, SSD1306_CHPMP_CMD, SSD1306_CHPMP_ENA};

  if (0 != i2c_write_dt(shared_dev, msg, 3)) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  oled_cfg.chrgpmp_on = true;

  return I2C_OLED_OK;
}

// Enable the charge pump
// arguments: none
core_err_t oled_disable_chrgpmp(void) {
  // make sure that the bus is ready
  if (!device_is_ready(shared_dev->bus)) {
    return ERR_I2C_NOTREADY;
  }

  // check if device screen is on--turn off if so

  // send the command here
  uint8 msg[3] = {SSD1306_ALL_CMD_BYTE, SSD1306_CHPMP_CMD, SSD1306_CHPMP_ENA};

  if (0 != i2c_write_dt(shared_dev, msg, 3)) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  // reset oled cfg
  oled_cfg.chrgpmp_on = false;

  return I2C_OLED_OK;
}

/* Initialize an I2C display here with defaults */
core_err_t oled_init(oled_core_cfg_t *new_cfg, uint32 cfg_flags) {
  // initialize the local frame buffer to all zeros
  for (int i = 0; i < sizeof(local_frame_buffer); i++) {
    local_frame_buffer[i] = 0X00;
  }

  // if (local_frame_buffer != memset((uint8_t *)local_frame_buffer, 0, sizeof(local_frame_buffer))) {
  //     LOG_ERR("memcpy failed on line %d function %s", __LINE__, __FUNCTION__);
  //     return ERR_I2C_GENERAL;
  // }

  // initialize the global oled cfg variable
  oled_init_cfg();

  // check to see if the bus is ready
  if (!device_is_ready(shared_dev->bus)) {
    return ERR_I2C_NOTREADY;
  }

  // initialize the global config--used for tracking
  if (NULL != new_cfg) {
    if (I2C_OLED_OK != oled_set_cfg(new_cfg, cfg_flags)) {
      return ERR_I2C_GENERAL;
    }
  } else {
    if (I2C_OLED_OK != oled_set_cfg(new_cfg, DEFAULT_CFG_FLAGS)) {
      return ERR_I2C_GENERAL;
    }
  }

  LOG_INF("I2C bus with address 0x%X ready", shared_dev->addr);

  // powerup sequence will be a little jank here
  // in serial mode there's no way to read! wtf?
  // we might be able to turn on the device, then check a pin?
  
  // do the reset operation if no auto-reset
#if (defined(HAS_AUTO_RESET) && HAS_AUTO_RESET)
  // reset code here
  k_sleep(K_USEC(1000));
#endif

  // wait a second
  k_sleep(K_MSEC(100));

  // send charge pump settings
  if (I2C_OLED_OK == oled_enable_chrgpmp()) {
    LOG_INF("Charge pump enabled for device at address 0X%X", shared_dev->addr);
  } else {
    return ERR_I2C_GENERAL;
  }

  if (I2C_OLED_OK == oled_pwrp_screen()) {
    LOG_INF("Screen enabled for device at address 0X%X", shared_dev->addr);
  } else {
    return ERR_I2C_GENERAL;
  }

  // send all zeros to black out the display
  if (I2C_OLED_OK != oled_send_bits(NULL, OLED_SIZE, 0)) {
    LOG_ERR("send_bits failed on line %d function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  return I2C_OLED_OK;
}

core_err_t oled_pwrp_screen(void) {
  if (!device_is_ready(shared_dev->bus)) {
    return ERR_I2C_NOTREADY;
  }

  uint8 msg[2] = {SSD1306_NEXT_BYTE_IS_CMD, SSD1306_DISP_ON};
  LOG_INF("Sending display-on command to screen");
  if (i2c_write_dt(shared_dev, msg, 2)) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  oled_cfg.screen_on = true;

  return I2C_OLED_OK;
}

core_err_t oled_pwrdn_screen(void) {
  // we basically just send the power off command
  // check again to make sure the device is still connected
  if (!device_is_ready(shared_dev->bus)) {
    return ERR_I2C_NOTREADY;
  }

  LOG_INF("Powering off screen");

  uint8_t msg[2] = {SSD1306_NEXT_BYTE_IS_CMD, SSD1306_DISP_OFF};
  if (0 != i2c_write_dt(shared_dev, msg, 2)) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  // reset global config
  oled_cfg.screen_on = false;

  return I2C_OLED_OK;
}

core_err_t oled_pwrdn(void) {
  if (I2C_OLED_OK != oled_disable_chrgpmp()) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  if (I2C_OLED_OK != oled_pwrdn_screen()) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  LOG_INF("I2C OLED turned off successfully");

	return I2C_OLED_OK;
}

// assumes horizontal addressing mode
core_err_t oled_send_bits(const uint8 *const fb, uint32 size_buffer, uint8 col_start) {

  // might be unwise, but i really think that we should transfer the
  // whole frame buffer in 'one' i2c transaction

  // check that screen and charge pump is on
  if (!(oled_cfg.chrgpmp_on && oled_cfg.screen_on)) {
    LOG_ERR("charge pump and screen must be enabled to send bits!");
    return ERR_I2C_GENERAL;
  }

	// check that the addressing mode is set to horizontal
	if (HORIZ_MODE != oled_cfg.mem_mode) {
		LOG_WRN("oled memmode not horizontal, unexpected results may occur when displaying");
	}

  // might have to do some heap fuckery here... or just stack it
  // do a memcpy to an offset (if we were smart)

  local_frame_buffer[0] = SSD1306_ALL_DAT_BYTE;

  if (fb != NULL) {
#if (OLED_USE_MEMCPY)
    memcpy(&local_frame_buffer[1], fb, size_buffer);
#else
    for (int i = 1; i <= size_buffer; i++) {
      local_frame_buffer[i] = fb[i-1];
    }
#endif
  } else {
    // just use current frame buffer
  }

  // then write it here
  if (0 != i2c_write_dt(shared_dev, local_frame_buffer, sizeof(local_frame_buffer))) {
    LOG_ERR("i2c_write_dt failed on line %d in function %s", __LINE__, __FUNCTION__);
    return ERR_I2C_GENERAL;
  }

  return I2C_OLED_OK;
}
