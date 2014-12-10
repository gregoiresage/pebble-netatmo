#include <pebble.h>
#include "dashboard_data.h"
#include "utils.h"

#define MAXIMUM_MODULES 6

static DashboardData outdoor_dashboard;
static DashboardData dashboards[MAXIMUM_MODULES]; //6 modules
static int numberOfDashboards = 0;

DashboardData dashboard_data_get(int index){
	return dashboards[index];
}

void dashboard_data_add(DashboardData dashboard_data){
	if(numberOfDashboards < MAXIMUM_MODULES){
		memcpy(&dashboards[numberOfDashboards], &dashboard_data, sizeof(DashboardData));
		numberOfDashboards++;
	}
}

DashboardData dashboard_data_get_outdoor(){
	for(int i=0; i<numberOfDashboards; i++){
		if(dashboards[i].type == NAMain){
			outdoor_dashboard.pressure = dashboards[i].pressure;
			memcpy(outdoor_dashboard.data_pressure, dashboards[i].data_pressure, 24 * sizeof(int16_t));
			break;
		}
	}
	return outdoor_dashboard;
}

void dashboard_data_set_outdoor(DashboardData dashboard_data){
	memcpy(&outdoor_dashboard, &dashboard_data, sizeof(DashboardData));
}

int dashboard_data_count(){
	return numberOfDashboards;
}

void dashboard_data_reset(){
	numberOfDashboards = 0;
}

void dashboard_data_print(DashboardData dashboard_data){
	APP_LOG(APP_LOG_LEVEL_INFO, "%d %s %d.%d %d.%d %d.%d %d %d %d %d.%d",
		dashboard_data.type,
		dashboard_data.name,
		dashboard_data.temperature/10,abs(dashboard_data.temperature)%10,
		dashboard_data.temperature_min/10,abs(dashboard_data.temperature_min)%10,
		dashboard_data.temperature_max/10,abs(dashboard_data.temperature_max)%10,
		dashboard_data.humidity,
		dashboard_data.noise,
		dashboard_data.co2,
		dashboard_data.pressure/10,dashboard_data.pressure%10
		);
}
