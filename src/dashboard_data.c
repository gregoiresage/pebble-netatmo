#include <pebble.h>
#include "dashboard_data.h"
#include "utils.h"

static DashboardData module_data;
static DashboardData station_data[4];
static int numberOfStations = 0;

DashboardData getModuleData(){
	return module_data;
}
void setModuleData(DashboardData data){
	memcpy(&module_data, &data, sizeof(DashboardData));
}

DashboardData getStationData(int index){
	return station_data[index];
}
void setStationData(int index, DashboardData data){
	memcpy(&station_data[index], &data, sizeof(DashboardData));
	if(index + 1 > numberOfStations){
		numberOfStations = index + 1;
	}
}

int getNumberOfStations(){
	return numberOfStations;
}

void printData(DashboardData data){
	double dd = data.temperature;
	APP_LOG(APP_LOG_LEVEL_INFO, "%d %s %d.%d %d.%d %d.%d %d %d %d %d.%d",
		data.type,
		data.name,
		data.temperature/10,abs(data.temperature)%10,
		data.temperature_min/10,abs(data.temperature_min)%10,
		data.temperature_max/10,abs(data.temperature_max)%10,
		data.humidity,
		data.noise,
		data.co2,
		data.pressure/10,data.pressure%10
		);
}
