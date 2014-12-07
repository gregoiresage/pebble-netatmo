#include <pebble.h>
#include "dashboard_data.h"
#include "main_window.h"
#include "main_layer.h"
#include "station_layer.h"

#define KEY_MODULE_DATA     0
#define KEY_STATION_1_DATA  1
#define KEY_STATION_2_DATA  2
#define KEY_STATION_3_DATA  3
#define KEY_STATION_4_DATA  4

static void cb_in_received_handler(DictionaryIterator *iter, void *context) {

  DashboardData data;

  Tuple *tuple = dict_find(iter, KEY_MODULE_DATA);
  if(tuple){
    memcpy(&data,&tuple->value->uint8,tuple->length);
    setModuleData(data);
    update_main_layer(data);
  }

  tuple = dict_find(iter, KEY_STATION_1_DATA);
  if(tuple){
    memcpy(&data,&tuple->value->uint8,tuple->length);
    setStationData(0, data);
    update_station_layer(data);
  }

  tuple = dict_find(iter, KEY_STATION_2_DATA);
  if(tuple){
    memcpy(&data,&tuple->value->uint8,tuple->length);
    setStationData(1, data);
  }

  tuple = dict_find(iter, KEY_STATION_3_DATA);
  if(tuple){
    memcpy(&data,&tuple->value->uint8,tuple->length);
    setStationData(2, data);
  }

  tuple = dict_find(iter, KEY_STATION_4_DATA);
  if(tuple){
    memcpy(&data,&tuple->value->uint8,tuple->length);
    setStationData(3, data);
  }
  
}

int main(void) {

  show_main_window();

  app_message_register_inbox_received(cb_in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), 0);

  app_event_loop();

  app_message_deregister_callbacks();
}