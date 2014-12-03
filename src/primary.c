#include "primary.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_bitham_42_light;
static GFont s_res_gothic_18;
static TextLayer *s_main_temp;
static TextLayer *s_main_temp_max;
static TextLayer *s_main_temp_min;
static InverterLayer *s_inverterlayer_1;
static TextLayer *s_module_name;
static TextLayer *s_main_name;
static TextLayer *s_station_temp;
static TextLayer *s_station_temp_max;
static TextLayer *s_station_temp_min;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, true);
  
  s_res_bitham_42_light = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  // s_main_temp
  s_main_temp = text_layer_create(GRect(0, 11, 107, 43));
  text_layer_set_background_color(s_main_temp, GColorClear);
  text_layer_set_text(s_main_temp, "0°");
  text_layer_set_text_alignment(s_main_temp, GTextAlignmentRight);
  text_layer_set_font(s_main_temp, s_res_bitham_42_light);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_main_temp);
  
  // s_main_temp_max
  s_main_temp_max = text_layer_create(GRect(97, 19, 44, 18));
  text_layer_set_background_color(s_main_temp_max, GColorClear);
  text_layer_set_text(s_main_temp_max, "21.3°");
  text_layer_set_text_alignment(s_main_temp_max, GTextAlignmentRight);
  text_layer_set_font(s_main_temp_max, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_main_temp_max);
  
  // s_main_temp_min
  s_main_temp_min = text_layer_create(GRect(97, 34, 44, 18));
  text_layer_set_background_color(s_main_temp_min, GColorClear);
  text_layer_set_text(s_main_temp_min, "22.2°");
  text_layer_set_text_alignment(s_main_temp_min, GTextAlignmentRight);
  text_layer_set_font(s_main_temp_min, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_main_temp_min);
  
  // s_inverterlayer_1
  s_inverterlayer_1 = inverter_layer_create(GRect(4, 87, 136, 79));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_inverterlayer_1);
  
  // s_module_name
  s_module_name = text_layer_create(GRect(5, 146, 135, 18));
  text_layer_set_background_color(s_module_name, GColorClear);
  text_layer_set_text_color(s_module_name, GColorWhite);
  text_layer_set_text(s_module_name, "Outdoor");
  text_layer_set_font(s_module_name, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_module_name);
  
  // s_main_name
  s_main_name = text_layer_create(GRect(0, 0, 144, 16));
  text_layer_set_background_color(s_main_name, GColorBlack);
  text_layer_set_text_color(s_main_name, GColorWhite);
  text_layer_set_text(s_main_name, "Maison");
  text_layer_set_text_alignment(s_main_name, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_main_name);
  
  // s_station_temp
  s_station_temp = text_layer_create(GRect(5, 83, 99, 43));
  text_layer_set_background_color(s_station_temp, GColorClear);
  text_layer_set_text_color(s_station_temp, GColorWhite);
  text_layer_set_text(s_station_temp, "0°");
  text_layer_set_text_alignment(s_station_temp, GTextAlignmentRight);
  text_layer_set_font(s_station_temp, s_res_bitham_42_light);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_station_temp);
  
  // s_station_temp_max
  s_station_temp_max = text_layer_create(GRect(94, 90, 44, 18));
  text_layer_set_background_color(s_station_temp_max, GColorClear);
  text_layer_set_text_color(s_station_temp_max, GColorWhite);
  text_layer_set_text(s_station_temp_max, "23.9°");
  text_layer_set_text_alignment(s_station_temp_max, GTextAlignmentRight);
  text_layer_set_font(s_station_temp_max, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_station_temp_max);
  
  // s_station_temp_min
  s_station_temp_min = text_layer_create(GRect(94, 104, 44, 18));
  text_layer_set_background_color(s_station_temp_min, GColorClear);
  text_layer_set_text_color(s_station_temp_min, GColorWhite);
  text_layer_set_text(s_station_temp_min, "-5.0°");
  text_layer_set_text_alignment(s_station_temp_min, GTextAlignmentRight);
  text_layer_set_font(s_station_temp_min, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_station_temp_min);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_main_temp);
  text_layer_destroy(s_main_temp_max);
  text_layer_destroy(s_main_temp_min);
  inverter_layer_destroy(s_inverterlayer_1);
  text_layer_destroy(s_module_name);
  text_layer_destroy(s_main_name);
  text_layer_destroy(s_station_temp);
  text_layer_destroy(s_station_temp_max);
  text_layer_destroy(s_station_temp_min);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_primary(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_primary(void) {
  window_stack_remove(s_window, true);
}


#define KEY_MAIN_TEMP      0
#define KEY_MAIN_TEMP_MIN  1
#define KEY_MAIN_TEMP_MAX  2
#define KEY_MAIN_CO2       3
#define KEY_MAIN_HUMIDITY  4
#define KEY_MAIN_NOISE     5
#define KEY_MAIN_PRESSURE  6

#define KEY_STATION_TEMP      100
#define KEY_STATION_TEMP_MIN  101
#define KEY_STATION_TEMP_MAX  102
#define KEY_STATION_HUMIDITY  103

static char text_main_temp[8] = "";
static char text_main_temp_min[8] = "";
static char text_main_temp_max[8] = "";
static char text_main_co2[8] = "";
static char text_main_humidity[8] = "";
static char text_main_noise  [8] = "";
static char text_main_pressure[8] = "";
static char text_station_temp[8] = "";
static char text_station_temp_min[8] = "";
static char text_station_temp_max[8] = "";
static char text_station_humidity[8] = "";

static char steps_text[256] = "";

static void cb_in_received_handler(DictionaryIterator *iter, void *context) {

  Tuple *tuple = dict_find(iter, KEY_MAIN_TEMP);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_TEMP : %ld.%ld", val/10, val%10);
    snprintf(text_main_temp, sizeof(text_main_temp), "%ld.%ld°", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_MAIN_TEMP_MIN);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_TEMP_MIN : %ld.%ld", val/10, val%10);
    snprintf(text_main_temp_min, sizeof(text_main_temp_min), "%ld.%ld°", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_MAIN_TEMP_MAX);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_TEMP_MAX : %ld.%ld", val/10, val%10);
    snprintf(text_main_temp_max, sizeof(text_main_temp_max), "%ld.%ld°", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_MAIN_CO2);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_CO2 : %ld", val);
  }

  tuple = dict_find(iter, KEY_MAIN_HUMIDITY);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_HUMIDITY : %ld", val);
  }

  tuple = dict_find(iter, KEY_MAIN_NOISE);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_NOISE : %ld", val);
  }

  tuple = dict_find(iter, KEY_MAIN_PRESSURE);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_MAIN_PRESSURE : %ld.%ld", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_STATION_TEMP);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_STATION_TEMP : %ld.%ld", val/10, val%10);
    snprintf(text_station_temp, sizeof(text_station_temp), "%ld.%ld°", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_STATION_TEMP_MIN);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_STATION_TEMP_MIN : %ld.%ld", val/10, val%10);
    snprintf(text_station_temp_min, sizeof(text_station_temp_min), "%ld.%ld°", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_STATION_TEMP_MAX);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_STATION_TEMP_MAX : %ld.%ld", val/10, val%10);
    snprintf(text_station_temp_max, sizeof(text_station_temp_max), "%ld.%ld°", val/10, val%10);
  }

  tuple = dict_find(iter, KEY_STATION_HUMIDITY);
  if(tuple){
    int32_t val = tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_STATION_HUMIDITY : %ld", val);
  }

  layer_mark_dirty(window_get_root_layer(s_window));
}

int main(void) {
    // Register message handlers
  app_message_register_inbox_received(cb_in_received_handler);
  // Init buffers
  app_message_open(app_message_inbox_size_maximum(), 0);

  show_primary();

  text_layer_set_text(s_main_temp,text_main_temp);
  text_layer_set_text(s_main_temp_max,text_main_temp_max);
  text_layer_set_text(s_main_temp_min,text_main_temp_min);
  text_layer_set_text(s_station_temp,text_station_temp);
  text_layer_set_text(s_station_temp_max,text_station_temp_max);
  text_layer_set_text(s_station_temp_min,text_station_temp_min);

  app_event_loop();
  
}