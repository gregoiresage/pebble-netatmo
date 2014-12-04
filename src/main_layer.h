#pragma once
#include <pebble.h>
#include "dashboard_data.h"

Layer* create_main_layer(void);

void destroy_main_layer(void);

void update_main_layer(DashboardData data);