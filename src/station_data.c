#include <pebble.h>
#include "dashboard_data.h"
#include "station_data.h"

StationData* station_data_create(char* name){
	StationData* station_data = (StationData*)malloc(sizeof(StationData));
	station_data->modules_count = 0;
	snprintf(station_data->name, sizeof(station_data->name), name);
	return station_data;
}

void station_data_destroy(StationData* station_data){
	for(uint8_t i=0; i<station_data->modules_count; i++){
		dashboard_data_destroy(station_data->modules_dashboard[i]);
	}
	free(station_data);
}

uint8_t	station_data_get_module_count(StationData* station_data){
	return station_data->modules_count;
}

DashboardData* station_data_get_module_data(StationData* station_data, uint8_t index){
	if(index < station_data->modules_count){
		return station_data->modules_dashboard[index];
	}
	return NULL;
}

void station_data_add_module_data(StationData* station_data, DashboardData *dashboard_data){
	if(station_data->modules_count < MAXIMUM_MODULES){
		station_data->modules_dashboard[station_data->modules_count] = dashboard_data_create();
		memcpy(station_data->modules_dashboard[station_data->modules_count], dashboard_data, sizeof(DashboardData));
		station_data->modules_count++;
	}
}
