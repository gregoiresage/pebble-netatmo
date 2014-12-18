#pragma once

#include <pebble.h>
#include "dashboard_data.h"

#define MAXIMUM_MODULES 7

typedef struct StationData {
	char			name[40];
	DashboardData*	modules_dashboard[MAXIMUM_MODULES];
	uint8_t			modules_count;
} StationData;

StationData* 	station_data_create(char* name);
void 			station_data_destroy(StationData* station_data);

uint8_t			station_data_get_module_count(StationData* station_data);

DashboardData*	station_data_get_module_data(StationData* station_data, uint8_t index);
void 			station_data_add_module_data(StationData* station_data, DashboardData *dashboard_data);
