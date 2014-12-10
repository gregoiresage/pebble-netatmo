#include <pebble.h>

#include "dashboard_layer.h"

static Window *s_window;

static DashboardLayer  *s_top_layer;
static DashboardLayer  *s_bottom_layer;

static Layer  *s_error_layer;
static GFont  error_font;

static PropertyAnimation *prop_animation;

#define TOP     0
#define BOTTOM  1
#define MIDDLE  2
static int layout = MIDDLE;

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
  if(layout == TOP && !toUp)
    return;

  if(layout == BOTTOM && toUp)
    return;

  GRect to_rect;
  if (layout == MIDDLE && toUp) {
    to_rect = GRect(0, 168 - 2, 144, 168);
    layout = TOP;
  } else if (layout == MIDDLE && !toUp) {
    to_rect = GRect(0, 0, 144, 168);
    layout = BOTTOM;
  }
  else {
    to_rect = GRect(0, 168 / 2, 144, 168);
    layout = MIDDLE;
  }

  dashboard_layer_set_graph_hidden(s_top_layer, layout != TOP);
  dashboard_layer_set_graph_hidden(s_bottom_layer, layout != BOTTOM);

  destroy_property_animation(&prop_animation);

  prop_animation = property_animation_create_layer_frame(dashboard_layer_get_layer(s_bottom_layer), NULL, &to_rect);
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
  if(layout == MIDDLE){
    current_station = (current_station + 1) % (dashboard_data_count());
    dashboard_layer_update_data(s_bottom_layer, dashboard_data_get(current_station));
  }
  else if(layout == BOTTOM){
    dashboard_layer_switch_graph(s_bottom_layer);
  }
  else if(layout == TOP){
    dashboard_layer_switch_graph(s_top_layer);
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
  layer_set_hidden(s_error_layer,                             error == NULL);
  layer_set_hidden(dashboard_layer_get_layer(s_top_layer), error != NULL);
  layer_set_hidden(dashboard_layer_get_layer(s_bottom_layer), error != NULL);
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
  window_set_click_config_provider(s_window, click_config_provider);

  s_top_layer = dashboard_layer_create(GRect(0, 0, 144, 168), GColorWhite);
  dashboard_layer_set_graph_hidden(s_top_layer, true);
  layer_add_child(window_get_root_layer(s_window), dashboard_layer_get_layer(s_top_layer));

  s_bottom_layer = dashboard_layer_create(GRect(0, 0, 144, 168), GColorBlack);
  dashboard_layer_set_graph_hidden(s_bottom_layer, true);
  layer_set_frame(dashboard_layer_get_layer(s_bottom_layer), GRect(0, 168 / 2, 144, 168));
  layer_add_child(window_get_root_layer(s_window), dashboard_layer_get_layer(s_bottom_layer));

  s_error_layer = layer_create(GRect(0, 0, 144, 168));
  layer_add_child(window_get_root_layer(s_window), s_error_layer);
  layer_set_update_proc(s_error_layer,cb_erro_draw);
  layer_set_hidden(s_error_layer,true);
}

static void destroy_ui(void) {
  dashboard_layer_destroy(s_top_layer);
  dashboard_layer_destroy(s_bottom_layer);
  layer_destroy(s_error_layer);
  gbitmap_destroy(image_error);
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

void refresh_window(void){
  dashboard_layer_update_data(s_top_layer,    dashboard_data_get_outdoor());
  dashboard_layer_update_data(s_bottom_layer, dashboard_data_get(0));
}