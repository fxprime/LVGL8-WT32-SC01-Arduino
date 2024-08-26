/*
    Simple Demo with WT32-SC01 + LovyanGFX + LVGL8.x
*/
#define LGFX_AUTODETECT // Autodetect board
#define LGFX_USE_V1     // set to use new version of library
//#define LV_CONF_INCLUDE_SIMPLE

/* Uncomment below line to draw on screen with touch */
//#define DRAW_ON_SCREEN

#include <LovyanGFX.hpp> // main library
static LGFX lcd; // declare display variable

#include <lvgl.h>
#include "lv_conf.h"
/*** Setup screen resolution for LVGL ***/
static const uint16_t screenWidth = 480;
static const uint16_t screenHeight = 320;

#define TFT_HOR_RES screenWidth
#define TFT_VER_RES screenHeight

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];


// Variables for touch x,y
#ifdef DRAW_ON_SCREEN
static int32_t x, y;
#endif

/*** Function declaration ***/
void display_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map);
void touchpad_read( lv_indev_t * indev, lv_indev_data_t * data );
// void lv_button_demo(void);
 void lv_example_style_7(void)
{
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_arc_color(&style, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_arc_width(&style, 4);

    /*Create an object with the new style*/
    lv_obj_t * obj = lv_arc_create(lv_screen_active());
    lv_obj_add_style(obj, &style, 0);
    lv_obj_center(obj);
}
 
void setup(void)
{

  Serial.begin(115200); /* prepare for possible serial debug */

  lcd.init(); // Initialize LovyanGFX
  lv_init();  // Initialize lvgl

  lv_tick_set_cb((lv_tick_get_cb_t)millis);  

  // Setting display to landscape
  if (lcd.width() < lcd.height())
    lcd.setRotation(lcd.getRotation() ^ 1);


  lv_display_t * disp; 
  disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
  lv_display_set_flush_cb(disp, display_flush);
  lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);


    /*Initialize the (dummy) input device driver*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER); /*Touchpad should have POINTER type*/
    lv_indev_set_read_cb(indev, touchpad_read);


  // /* LVGL : Setting up buffer to use for display */
  // lv_draw_buf_init(&draw_buf, screenWidth, 10, LV_COLOR_FORMAT_RGB565, 1, buf, screenWidth * 10);

  // /*** LVGL : Setup & Initialize the display device driver ***/
  // static lv_drv_t disp_drv;
  // lv_drv_init(&disp_drv);
  // disp_drv.hor_res = screenWidth;
  // disp_drv.ver_res = screenHeight;
  // disp_drv.flush_cb = display_flush;
  // disp_drv.draw_buf = &draw_buf;
  // lv_drv_register(&disp_drv);

  // /*** LVGL : Setup & Initialize the input device driver ***/
  // static lv_indev_drv_t indev_drv;
  // lv_indev_drv_init(&indev_drv);
  // indev_drv.type = LV_INDEV_TYPE_POINTER;
  // indev_drv.read_cb = touchpad_read;
  // lv_indev_drv_register(&indev_drv);

  /*** Create simple label and show LVGL version ***/
  String LVGL_Arduino = "WT32-SC01 with LVGL ";
  LVGL_Arduino += String('v') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  lv_obj_t *label = lv_label_create(lv_scr_act()); // full screen as the parent
  lv_label_set_text(label, LVGL_Arduino.c_str());  // set label text
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);      // Center but 20 from the top
  lv_example_style_7();
  // lv_button_demo();
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);

  int x, y;
  if (lcd.getTouch(&x, &y))
  {
    lcd.fillRect(x - 2, y - 2, 5, 5, TFT_RED);
    lcd.setCursor(380, 0);
    lcd.printf("Touch:(%03d,%03d)", x, y);
  }
}

  /*** Display callback to flush the buffer to screen ***/
  void display_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map)
  {
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    lcd.startWrite();
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    lcd.writePixels((lgfx::rgb565_t *)px_map, w * h);
    lcd.endWrite();


    lv_disp_flush_ready(disp);
  }

  /*** Touchpad callback to read the touchpad ***/
  void touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
  {
    uint16_t touchX, touchY;
    bool touched = lcd.getTouch(&touchX, &touchY);

    if (!touched)
    {
      data->state = LV_INDEV_STATE_RELEASED;
    }
    else
    {
      data->state = LV_INDEV_STATE_PRESSED;

      /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;

      Serial.printf("Touch (x,y): (%03d,%03d)\n",touchX,touchY );
    } 
  }
 