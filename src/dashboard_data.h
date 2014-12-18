#pragma once

#include <pebble.h>

typedef enum {
	NAMain,
	NAModule1,
	NAModule4,
	NAModule3,
	NAPlug,
	NATherm1
} ModuleType;

typedef enum {
	Temperature,
	Humidity,
	Noise,
	CO2,
	Pressure,
	Rain,
} MeasureType;

typedef struct DashboardData {
	uint16_t	type;

	char		name[40];
	int16_t 	temperature;
	int16_t 	temperature_min;
	int16_t 	temperature_max;
	uint8_t 	humidity;
	uint8_t 	noise;
	uint16_t 	co2;
	uint16_t 	pressure;
	uint16_t 	rain;
	
	int16_t		data_temperature[24];
	int16_t		data_co2[24];
	int16_t		data_humidity[24];
	int16_t		data_pressure[24];
	int16_t		data_noise[24];
	int16_t		data_rain[24];

	MeasureType displayed_measure;

} DashboardData;

DashboardData* 	dashboard_data_create();
void 			dashboard_data_destroy(DashboardData* dashboard_data);

void 			dashboard_data_print(DashboardData *data);