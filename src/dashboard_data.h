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

typedef struct {
	
	char		name[20];
	int16_t 	temperature;
	int16_t 	temperature_min;
	int16_t 	temperature_max;
	uint8_t 	humidity;
	uint8_t 	noise;
	uint16_t 	co2;
	uint16_t 	pressure;
	uint16_t 	rain;
	
	uint8_t		type;

	bool 		valid;
} DashboardData;

void printData(DashboardData data);

DashboardData getModuleData();
void setModuleData(DashboardData data);

int getNumberOfStations();
DashboardData getStationData(int index);
void setStationData(int index, DashboardData data);