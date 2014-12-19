#include <pebble.h>
#include "dashboard_data.h"
#include "station_data.h"
#include "user_data.h"
#include "main_window.h"

#define KEY_DASHBOARD_DATA  0
#define KEY_RESET_DATA      1
#define KEY_ERROR           1000

UserData user_data;

static void cb_in_received_handler(DictionaryIterator *iter, void *context) {
  DashboardData data;

  Tuple *tuple = dict_find(iter, KEY_RESET_DATA);
  if(tuple){
    for(int i=0; i<user_data.stations_count; i++){
      station_data_destroy(user_data.stations[i]);
    }
    user_data.stations_count = 0;
    for(int i=0; i<tuple->length && user_data.stations_count < MAXIMUM_STATIONS; i+=STATION_NAME_MAX_LENGTH){
      user_data.stations[user_data.stations_count] = station_data_create((char*)(&tuple->value->uint8 + i));
      user_data.stations_count++;
    }
  }

  tuple = dict_find(iter, KEY_DASHBOARD_DATA);
  if(tuple){
    uint16_t stationId = tuple->value->uint16;
    memcpy(&data,&tuple->value->uint8 + 2,tuple->length - 2);
    station_data_add_module_data(user_data.stations[stationId], &data);
    refresh_window(stationId);
  }

  tuple = dict_find(iter, KEY_ERROR);
  if(tuple){
    APP_LOG(APP_LOG_LEVEL_INFO, "Error %s", tuple->value->cstring);
    setError(tuple->value->cstring);
  }
  else {
    setError(0);
  }
}

int main(void) {
  user_data.stations_count = 0;
  show_main_window(&user_data);

  app_message_register_inbox_received(cb_in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), 0);

  app_event_loop();

  app_message_deregister_callbacks();

  for(int i=0; i<user_data.stations_count; i++){
    station_data_destroy(user_data.stations[i]);
  }
}