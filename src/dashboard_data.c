#include <pebble.h>
#include "dashboard_data.h"

void printData(DashboardData data){
	APP_LOG(APP_LOG_LEVEL_INFO, "%s %d.%d %d.%d %d.%d %d %d %d %d.%d",
		data.name,
		data.temperature/10,data.temperature%10,
		data.temperature_min/10,data.temperature_min%10,
		data.temperature_max/10,data.temperature_max%10,
		data.humidity,
		data.noise,
		data.co2,
		data.pressure/10,data.pressure%10
		);
}
