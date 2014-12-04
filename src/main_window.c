#include "main_window.h"
#include "main_layer.h"
#include "station_layer.h"
#include <pebble.h>

static Window *s_window;
static Layer  *s_main_layer;

static PropertyAnimation *prop_animation;

static bool toggle = false;

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
  if(toggle == toUp)
    return;

  toggle = toUp;
  GRect to_rect;
  if (toggle) {
    to_rect = GRect(0, 0, 144, 168);
  } else {
    to_rect = GRect(0, 168 / 2  + 10, 144, 168);
  }

  destroy_property_animation(&prop_animation);

  prop_animation = property_animation_create_layer_frame(s_main_layer, NULL, &to_rect);
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

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, true);
  // window_set_background_color(s_window, GColorBlack);
  window_set_click_config_provider(s_window, click_config_provider);

  layer_add_child(window_get_root_layer(s_window), create_station_layer());

  s_main_layer = create_main_layer();
  layer_set_frame(s_main_layer, GRect(0, 168 / 2 + 10, 144, 168));
  layer_add_child(window_get_root_layer(s_window), s_main_layer);
}

static void destroy_ui(void) {
  destroy_main_layer();
  destroy_station_layer();
  window_destroy(s_window);
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
