#include <pebble.h>

#include "user_data.h"

void show_main_window(UserData* user_data);
void hide_main_window(void);
void refresh_window(uint8_t station_id);

void setError(char* error);
