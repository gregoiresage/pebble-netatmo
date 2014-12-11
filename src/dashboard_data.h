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

typedef struct {
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


void dashboard_data_reset();
int dashboard_data_count();

DashboardData dashboard_data_get_outdoor();
void dashboard_data_set_outdoor(DashboardData dashboard_data);

DashboardData dashboard_data_get(int index);
void dashboard_data_add(DashboardData dashboard_data);

void dashboard_data_print(DashboardData data);