#pragma once

#include <pebble.h>
#include "station_data.h"

#define MAXIMUM_STATIONS 5

typedef struct UserData {
	StationData*	stations[MAXIMUM_STATIONS];
	uint8_t			stations_count;
} UserData;
