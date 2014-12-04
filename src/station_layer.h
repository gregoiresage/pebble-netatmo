#pragma once
#include <pebble.h>
#include "dashboard_data.h"

Layer* create_station_layer(void);

void destroy_station_layer(void);

void update_station_layer(DashboardData data);