#include <pebble.h>
#include "dashboard_data.h"
#include "main_window.h"

#define KEY_DASHBOARD_DATA  0
#define KEY_RESET_DATA      1
#define KEY_ERROR           1000

static void cb_in_received_handler(DictionaryIterator *iter, void *context) {
  DashboardData data;

  Tuple *tuple = dict_find(iter, KEY_RESET_DATA);
  if(tuple){
    dashboard_data_reset();
  }

  tuple = dict_find(iter, KEY_DASHBOARD_DATA);
  if(tuple){
    memcpy(&data,&tuple->value->uint8,tuple->length);
    if(data.type == NAModule1){
      dashboard_data_set_outdoor(data);
    }
    else {
      dashboard_data_add(data);
    } 
  }

  tuple = dict_find(iter, KEY_ERROR);
  if(tuple){
    APP_LOG(APP_LOG_LEVEL_INFO, "Error %s", tuple->value->cstring);
    setError(tuple->value->cstring);
  }
  else {
    setError(0);
  }

  refresh_window();
}

int main(void) {

  show_main_window();

  app_message_register_inbox_received(cb_in_received_handler);
  app_message_open(app_message_inbox_size_maximum(), 0);

  app_event_loop();

  app_message_deregister_callbacks();
}