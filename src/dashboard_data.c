#include <pebble.h>
#include "dashboard_data.h"
#include "utils.h"

#define MAXIMUM_MODULES 6

static DashboardData outdoor_dashboard;
static DashboardData dashboards[MAXIMUM_MODULES]; //6 modules
static int numberOfDashboards = 0;

DashboardData* dashboard_data_create(){
	DashboardData* dashboard_data = (DashboardData*)malloc(sizeof(DashboardData));
	return dashboard_data;
}

void dashboard_data_destroy(DashboardData* dashboard_data){
	if(dashboard_data){
		free(dashboard_data);
		dashboard_data = NULL;
	}
}

void dashboard_data_print(DashboardData *dashboard_data){
	APP_LOG(APP_LOG_LEVEL_INFO, "%d %s %d.%d %d.%d %d.%d %d %d %d %d.%d",
		dashboard_data->type,
		dashboard_data->name,
		dashboard_data->temperature/10,abs(dashboard_data->temperature)%10,
		dashboard_data->temperature_min/10,abs(dashboard_data->temperature_min)%10,
		dashboard_data->temperature_max/10,abs(dashboard_data->temperature_max)%10,
		dashboard_data->humidity,
		dashboard_data->noise,
		dashboard_data->co2,
		dashboard_data->pressure/10,dashboard_data->pressure%10
		);
}
