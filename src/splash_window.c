#include <pebble.h>
#include "pebble_process_info.h"

static Window *s_window;
static BitmapLayer  *s_image_layer;
static GBitmap *image_splash;

extern const PebbleProcessInfo __pbl_app_info;
static TextLayer *s_version_layer;
static char version_text[30] = "";

static AppTimer *timer = NULL;

static bool hideMe = false;

static void timer_callback(void *data) {
  timer = NULL;
  if(hideMe)
    window_stack_pop(false);
}


static void initialise_ui(void) {
  image_splash = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SPLASH);

  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
#ifdef PBL_SDK_2
  window_set_fullscreen(s_window, true);
#endif

  s_image_layer = bitmap_layer_create(GRect(0, (168 - 101) / 2, 144, 101));
  bitmap_layer_set_bitmap(s_image_layer,image_splash);
  layer_add_child(window_get_root_layer(s_window), bitmap_layer_get_layer(s_image_layer));

  s_version_layer = text_layer_create(GRect(0, 148, 144, 20));
  snprintf(version_text, sizeof(version_text),
      "v%d.%d", __pbl_app_info.process_version.major, __pbl_app_info.process_version.minor );
  text_layer_set_text(s_version_layer,version_text);
  text_layer_set_font(s_version_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_version_layer, GTextAlignmentRight);
  text_layer_set_background_color(s_version_layer,GColorClear);
  text_layer_set_text_color(s_version_layer,GColorWhite);
  layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_version_layer));

  timer = app_timer_register(2000 /* milliseconds */, timer_callback, NULL);
}

static void destroy_ui(void) {
  bitmap_layer_destroy(s_image_layer);
  gbitmap_destroy(image_splash);
  window_destroy(s_window);
  text_layer_destroy(s_version_layer);
  s_window = NULL;
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_splash_window() {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_splash_window() {
  hideMe = true;
  if(!timer && s_window)
    window_stack_pop(false);
}
