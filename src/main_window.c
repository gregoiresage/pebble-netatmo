#include "main_window.h"
#include "main_layer.h"
#include "station_layer.h"
#include <pebble.h>

static Window *s_window;
static Layer  *s_main_layer;
static Layer  *s_station_layer;
static Layer  *s_error_layer;

static GFont  error_font;

static PropertyAnimation *prop_animation;

#define FULL_MODULE   0
#define FULL_STATION  1
#define HALF_HALF     2
static int layout = HALF_HALF;

static int current_station = 0;

static GBitmap *image_error;
static char text_error[20] = "";

static void animation_started(Animation *animation, void *data) {
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
}

static void destroy_property_animation(PropertyAnimation **prop_animation) {
  if (*prop_animation == NULL) {
    return;
  }

  if (animation_is_scheduled((Animation*) *prop_animation)) {
    animation_unschedule((Animation*) *prop_animation);
  }

  property_animation_destroy(*prop_animation);
  *prop_animation = NULL;
}

static void animate(bool toUp){
  if(layout == FULL_STATION && toUp)
    return;

  if(layout == FULL_MODULE && !toUp)
    return;

  GRect to_rect;
  if (layout == HALF_HALF && toUp) {
    to_rect = GRect(0, 0, 144, 168);
    layout = FULL_STATION;
  } else if (layout == HALF_HALF && !toUp) {
    to_rect = GRect(0, 168 - 15, 144, 168);
    layout = FULL_MODULE;
  }
  else {
    to_rect = GRect(0, 168 / 2  + 10, 144, 168);
    layout = HALF_HALF;
  }

  destroy_property_animation(&prop_animation);

  prop_animation = property_animation_create_layer_frame(s_station_layer, NULL, &to_rect);
  animation_set_duration((Animation*) prop_animation, 400);
  animation_set_curve((Animation*) prop_animation, AnimationCurveEaseInOut);

  animation_set_handlers((Animation*) prop_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) animation_started,
    .stopped = (AnimationStoppedHandler) animation_stopped,
  }, NULL /* callback data */);
  animation_schedule((Animation*) prop_animation);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  animate(true);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  animate(false);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(layout == HALF_HALF){
    current_station = (current_station + 1) % getNumberOfStations();
    update_station_layer(getStationData(current_station));
  }
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

void setError(char* error){
  if(error){
    snprintf(text_error, sizeof(text_error), "%s", error);
  }

  layer_set_hidden(s_error_layer,   error == NULL);
  layer_set_hidden(s_station_layer, error != NULL);
  layer_set_hidden(s_main_layer,    error != NULL);
}

static void cb_erro_draw(Layer *layer, GContext *g_ctx) {
  graphics_draw_bitmap_in_rect(g_ctx, image_error, GRect((144-64)/2, (168-64)/2, 64, 64));
  graphics_context_set_text_color(g_ctx,GColorBlack);
  graphics_draw_text(g_ctx,
    text_error,
    error_font,
    GRect(0, 140, 144, 20),
    GTextOverflowModeWordWrap,
    GTextAlignmentCenter,
    NULL);
}

static void initialise_ui(void) {
  error_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  image_error = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ERROR);

  s_window = window_create();
  window_set_fullscreen(s_window, true);
  // window_set_background_color(s_window, GColorBlack);
  window_set_click_config_provider(s_window, click_config_provider);

  s_main_layer = create_main_layer();
  layer_add_child(window_get_root_layer(s_window), s_main_layer);

  s_station_layer = create_station_layer();
  layer_set_frame(s_station_layer, GRect(0, 168 / 2 + 10, 144, 168));
  layer_add_child(window_get_root_layer(s_window), s_station_layer);

  s_error_layer = layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(s_window), s_error_layer);
  layer_set_update_proc(s_error_layer,cb_erro_draw);
  layer_set_hidden(s_error_layer,true);
}

static void destroy_ui(void) {
  destroy_main_layer();
  destroy_station_layer();
  layer_destroy(s_error_layer);
  window_destroy(s_window);
  gbitmap_destroy(image_error);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_main_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_main_window(void) {
  window_stack_remove(s_window, true);
}
