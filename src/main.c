#include <pebble.h>
#include "primary.h"
#include "dashboard_data.h"
#include "main_window.h"
#include "main_layer.h"
#include "station_layer.h"

#define KEY_MODULE_DATA     0
#define KEY_STATION_1_DATA  1
#define KEY_STATION_2_DATA  2
#define KEY_STATION_3_DATA  3

static DashboardData module_data;
static DashboardData station_data[3];

static void cb_in_received_handler(DictionaryIterator *iter, void *context) {

  Tuple *tuple = dict_find(iter, KEY_MODULE_DATA);
  if(tuple){
    memcpy(&module_data,&tuple->value->uint8,tuple->length);
    printData(module_data);
    update_main_layer(module_data);
  }
  module_data.valid = tuple != 0;

  tuple = dict_find(iter, KEY_STATION_1_DATA);
  if(tuple){
    memcpy(&station_data[0],&tuple->value->uint8,tuple->length);
    printData(station_data[0]);
    update_station_layer(station_data[0]);
  }
  station_data[0].valid = tuple != 0;

  tuple = dict_find(iter, KEY_STATION_2_DATA);
  if(tuple){
    memcpy(&station_data[1],&tuple->value->uint8,tuple->length);
    printData(station_data[1]);
  }
  station_data[1].valid = tuple != 0;

  tuple = dict_find(iter, KEY_STATION_3_DATA);
  if(tuple){
    memcpy(&station_data[2],&tuple->value->uint8,tuple->length);
    printData(station_data[2]);
  }
  station_data[2].valid = tuple != 0;
}

int main(void) {
  app_message_register_inbox_received(cb_in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), 0);

  show_main_window();

  app_event_loop();

  app_message_deregister_callbacks();
}