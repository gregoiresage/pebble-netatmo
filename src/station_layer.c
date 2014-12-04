#include "main_layer.h"
#include <pebble.h>
#include "dashboard_data.h"

static GFont s_res_bitham_42_light;
static GFont s_res_roboto_condensed_21;
static Layer *s_background;
static TextLayer *s_temp;
static TextLayer *s_temp_min_max;
static TextLayer *s_humidity;
static TextLayer *s_name;

static void initialise_ui(void) {  
  s_res_bitham_42_light = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  s_res_roboto_condensed_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
  // s_background
  s_background = layer_create(GRect(0, 0, 144, 168));
  
  // s_temp
  s_temp = text_layer_create(GRect(0, 0, 144, 45));
  text_layer_set_background_color(s_temp, GColorClear);
  text_layer_set_text_color(s_temp, GColorBlack);
  text_layer_set_text(s_temp, "-.-°");
  text_layer_set_text_alignment(s_temp, GTextAlignmentCenter);
  text_layer_set_font(s_temp, s_res_bitham_42_light);
  layer_add_child(s_background, (Layer *)s_temp);
  
  // s_temp_min_max
  s_temp_min_max = text_layer_create(GRect(0, 43, 144, 27));
  text_layer_set_background_color(s_temp_min_max, GColorClear);
  text_layer_set_text_color(s_temp_min_max, GColorBlack);
  text_layer_set_text(s_temp_min_max, "-.-°/-.-°");
  text_layer_set_text_alignment(s_temp_min_max, GTextAlignmentCenter);
  text_layer_set_font(s_temp_min_max, s_res_roboto_condensed_21);
  layer_add_child(s_background, (Layer *)s_temp_min_max);
      
  // s_humidity
  s_humidity = text_layer_create(GRect(0, 68, 72, 27));
  text_layer_set_background_color(s_humidity, GColorClear);
  text_layer_set_text_color(s_humidity, GColorBlack);
  text_layer_set_text(s_humidity, "--%");
  text_layer_set_text_alignment(s_humidity, GTextAlignmentCenter);
  text_layer_set_font(s_humidity, s_res_roboto_condensed_21);
  layer_add_child(s_background, (Layer *)s_humidity);

  // s_name
  s_name = text_layer_create(GRect(72, 68, 72, 27));
  text_layer_set_background_color(s_name, GColorClear);
  text_layer_set_text_color(s_name, GColorBlack);
  text_layer_set_text(s_name, "----");
  text_layer_set_text_alignment(s_name, GTextAlignmentCenter);
  text_layer_set_font(s_name, s_res_roboto_condensed_21);
  layer_add_child(s_background, (Layer *)s_name);
}

static void destroy_ui(void) {
  layer_destroy(s_background);
  text_layer_destroy(s_temp);
  text_layer_destroy(s_temp_min_max);
  text_layer_destroy(s_humidity);
  text_layer_destroy(s_name);
}
// END AUTO-GENERATED UI CODE

static char text_temp[8] = "";
static char text_temp_min_max[20] = "";
static char text_humidity[8] = "";
static char text_name[20] = "";

Layer* create_station_layer(void) {
  initialise_ui();
  return s_background;
}

void destroy_station_layer(void) {
  destroy_ui();
}

void update_station_layer(DashboardData data) {
  snprintf(text_temp, sizeof(text_temp), "%d.%d°", data.temperature/10, data.temperature%10);
  snprintf(text_temp_min_max, sizeof(text_temp_min_max), "%d.%d° / %d.%d°", data.temperature_min/10, data.temperature_min%10, data.temperature_max/10, data.temperature_max%10);
  snprintf(text_humidity, sizeof(text_humidity), "%d%%", data.humidity);
  snprintf(text_name, sizeof(text_name), "%s", data.name);

  text_layer_set_text(s_temp,text_temp);
  text_layer_set_text(s_temp_min_max,text_temp_min_max);
  text_layer_set_text(s_humidity,text_humidity);
  text_layer_set_text(s_name,text_name);
}
