/**
 * @file main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include <SDL.h>

#include "hal/hal.h"
#include "lv_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OS != LV_OS_FREERTOS

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  sdl_hal_init(800, 480);

  lv_ui_init(4);

  UpdateScreenPort(1, SCREEN_PORT_FIELD_NAME, SCREEN_COLOR_WHITE, "TPS");
  UpdateScreenPort(2, SCREEN_PORT_FIELD_NAME, SCREEN_COLOR_WHITE, "MTPS");
  UpdateScreenPort(3, SCREEN_PORT_FIELD_NAME, SCREEN_COLOR_WHITE, "THW");
  UpdateScreenPort(4, SCREEN_PORT_FIELD_NAME, SCREEN_COLOR_WHITE, "Empty");
  UpdateScreenPort(1, SCREEN_PORT_FIELD_STATUS, SCREEN_COLOR_YELLOW, "Active");
  UpdateScreenPort(2, SCREEN_PORT_FIELD_STATUS, SCREEN_COLOR_BLUE, "Stabilizing");
  UpdateScreenPort(3, SCREEN_PORT_FIELD_STATUS, SCREEN_COLOR_WHITE, "Idle");
  UpdateScreenPanel(SCREEN_PANEL_TEST, SCREEN_TEST_PANEL_FIELD_TITLE, SCREEN_COLOR_WHITE, "Transient Test");
  UpdateScreenPanel(SCREEN_PANEL_TEST, SCREEN_TEST_PANEL_FIELD_STAGE, SCREEN_COLOR_YELLOW, "Heating");
  UpdateScreenPanel(SCREEN_PANEL_TEST, SCREEN_TEST_PANEL_FIELD_INFO, SCREEN_COLOR_WHITE, "Cooling will begin\nautomatically after\ntest.");
  UpdateScreenHeader(SCREEN_HEADER_FIELD_TEMP, SCREEN_COLOR_WHITE, "23.4");
  UpdateScreenPanelInfo(true);
  UpdateScreenPanelProgressShow(true);
  UpdateScreenPanelProgressAuto(0, 100, 10000);

  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    uint32_t sleep_time_ms = lv_timer_handler();
    if(sleep_time_ms == LV_NO_TIMER_READY){
	sleep_time_ms =  LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif
  }

  return 0;
}


#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

