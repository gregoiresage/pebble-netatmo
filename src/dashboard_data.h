#pragma once

#include <pebble.h>

typedef struct {
	char		name[20];
	int16_t 	temperature;
	int16_t 	temperature_min;
	int16_t 	temperature_max;
	uint8_t 	humidity;
	uint8_t 	noise;
	uint16_t 	co2;
	uint16_t 	pressure;
	bool 		valid;
} DashboardData;

void printData(DashboardData data);