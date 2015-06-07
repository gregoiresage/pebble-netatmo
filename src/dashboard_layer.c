#include <pebble.h>

#include "graph_layer.h"
#include "dashboard_data.h"
#include "dashboard_layer.h"

static void cb_background_draw(Layer *layer, GContext *g_ctx) {
	DashboardLayer* dashboard_layer = *(DashboardLayer**)(layer_get_data(layer));
	GRect layer_bounds = layer_get_bounds(layer);

	graphics_context_set_fill_color(g_ctx,dashboard_layer->bgColor);
	layer_bounds.origin.x += 1;
	layer_bounds.origin.y += 1;
	layer_bounds.size.w -= 2;
	layer_bounds.size.h -= 1;
	graphics_fill_rect(g_ctx,layer_bounds,6,GCornersTop);
}

static void resetDisplay(DashboardLayer *dashboard_layer){
	snprintf(dashboard_layer->text_name, sizeof(dashboard_layer->text_name), "---");
	snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), "---");
	snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "---");
	dashboard_layer->dashboard_data.displayed_measure = Temperature;
}

DashboardLayer* dashboard_layer_create(GRect frame, GColor bgColor){
	DashboardLayer* dashboard_layer = malloc(sizeof(DashboardLayer));

	dashboard_layer->bgColor = bgColor;
	dashboard_layer->fgColor = gcolor_equal(bgColor,GColorBlack) ? GColorWhite : GColorBlack;

	dashboard_layer->s_res_bitham_42_light 	= fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
	dashboard_layer->s_res_gothic_18_bold 	= fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

	resetDisplay(dashboard_layer);
	
	// s_background
	dashboard_layer->s_background = layer_create_with_data(frame, sizeof(DashboardLayer*));
	layer_set_update_proc(dashboard_layer->s_background, cb_background_draw);
	memcpy(layer_get_data(dashboard_layer->s_background), &dashboard_layer, sizeof(DashboardLayer*));
	
	// s_name
	dashboard_layer->s_name = text_layer_create(GRect(0, 2, 144, 20));
	text_layer_set_background_color(dashboard_layer->s_name, GColorClear);
	text_layer_set_font(dashboard_layer->s_name, fonts_get_system_font(FONT_KEY_FONT_FALLBACK));
	text_layer_set_text_color(dashboard_layer->s_name, dashboard_layer->fgColor);
	text_layer_set_text(dashboard_layer->s_name, dashboard_layer->text_name);
	text_layer_set_text_alignment(dashboard_layer->s_name, GTextAlignmentCenter);
	layer_add_child(dashboard_layer->s_background, text_layer_get_layer(dashboard_layer->s_name));
	
	// s_main
	dashboard_layer->s_main = text_layer_create(GRect(0, 12, 144, 45));
	text_layer_set_background_color(dashboard_layer->s_main, GColorClear);
	text_layer_set_text_color(dashboard_layer->s_main, dashboard_layer->fgColor);
	text_layer_set_text(dashboard_layer->s_main, dashboard_layer->text_main);
	text_layer_set_text_alignment(dashboard_layer->s_main, GTextAlignmentCenter);
	text_layer_set_font(dashboard_layer->s_main, dashboard_layer->s_res_bitham_42_light);
	layer_add_child(dashboard_layer->s_background, text_layer_get_layer(dashboard_layer->s_main));

	// s_subtitle
	dashboard_layer->s_subtitle = text_layer_create(GRect(0, 57, 144, 27));
	text_layer_set_background_color(dashboard_layer->s_subtitle, GColorClear);
	text_layer_set_text_color(dashboard_layer->s_subtitle, dashboard_layer->fgColor);
	text_layer_set_text(dashboard_layer->s_subtitle, dashboard_layer->text_subtitle);
	text_layer_set_text_alignment(dashboard_layer->s_subtitle, GTextAlignmentCenter);
	text_layer_set_font(dashboard_layer->s_subtitle, dashboard_layer->s_res_gothic_18_bold);
	layer_add_child(dashboard_layer->s_background, text_layer_get_layer(dashboard_layer->s_subtitle));

	// s_graph_layer
	dashboard_layer->s_graph_layer = graph_layer_create(GRect(3, 80, 144 - 6, 168 - 80), dashboard_layer->fgColor);
	layer_add_child(dashboard_layer->s_background, graph_layer_get_layer(dashboard_layer->s_graph_layer));

	return dashboard_layer;	
}

void dashboard_layer_destroy(DashboardLayer *dashboard_layer){
	layer_destroy(dashboard_layer->s_background);
	text_layer_destroy(dashboard_layer->s_name);
	text_layer_destroy(dashboard_layer->s_main);
	text_layer_destroy(dashboard_layer->s_subtitle);
	graph_layer_destroy(dashboard_layer->s_graph_layer);
	free(dashboard_layer);
}

Layer* dashboard_layer_get_layer(DashboardLayer *dashboard_layer){
	return dashboard_layer->s_background;
}

static void dashboard_layer_animate_graph(DashboardLayer *dashboard_layer){
	MeasureType displayed_measure = dashboard_layer->dashboard_data.displayed_measure;
	char title[40] = "";
	char legend[40] = "";
	int16_t *data = NULL; 
	switch(displayed_measure){
		case Temperature : 	
			data = dashboard_layer->dashboard_data.data_temperature; break;
		case Humidity : 	
			data = dashboard_layer->dashboard_data.data_humidity; break;
		case Pressure : 	
			data = dashboard_layer->dashboard_data.data_pressure; break;
		case CO2 : 			
			data = dashboard_layer->dashboard_data.data_co2; break;
		case Noise : 		
			data = dashboard_layer->dashboard_data.data_noise; break;	
		case Rain : 		
			data = dashboard_layer->dashboard_data.data_rain; break;
		default : break;
	}

	int16_t maximum = -32000;
	int16_t minimum =  32000;
	for(size_t i=0; i<24; i++){
		if(data[i] > maximum)
			maximum = data[i];
		if(data[i] < minimum)
			minimum = data[i];
	}

	switch(displayed_measure){
		case Temperature : 	
			snprintf(title, sizeof(title), "Temperature : %s%d.%d °", 
				dashboard_layer->dashboard_data.temperature/10 == 0 && dashboard_layer->dashboard_data.temperature < 0 ? "-" : "",
				dashboard_layer->dashboard_data.temperature/10,
				abs(dashboard_layer->dashboard_data.temperature)%10);
			snprintf(legend, sizeof(legend), "%s%d.%d / %s%d.%d (°)", 
				minimum/10 == 0 && minimum < 0 ? "-" : "",
				minimum/10,
				abs(minimum)%10,
				maximum == 0 && maximum < 0 ? "-" : "",
				maximum/10,
				abs(maximum)%10); break;
		case Humidity : 	
			snprintf(title, sizeof(title), "Humidity : %d %%", dashboard_layer->dashboard_data.humidity); 
			snprintf(legend, sizeof(legend), "%d / %d (%%)", minimum,maximum); break;
		case Pressure : 	
			snprintf(title, sizeof(title), "Pressure : %d.%d mb", dashboard_layer->dashboard_data.pressure/10,dashboard_layer->dashboard_data.pressure%10);
			snprintf(legend, sizeof(legend), "%d.%d / %d.%d (mb)", minimum/10,minimum%10,maximum/10,maximum%10); break;
		case CO2 : 			
			snprintf(title, sizeof(title), "CO² : %d ppm", dashboard_layer->dashboard_data.co2);
			snprintf(legend, sizeof(legend), "%d / %d (ppm)", minimum,maximum); break;
		case Noise : 		
			snprintf(title, sizeof(title), "Noise : %d dB", dashboard_layer->dashboard_data.noise);
			snprintf(legend, sizeof(legend), "%d / %d (dB)", minimum,maximum); break;
		case Rain : 		
			snprintf(title, sizeof(title), "Rain : %d.%03d mm", dashboard_layer->dashboard_data.rain/1000,dashboard_layer->dashboard_data.rain%1000);
			snprintf(legend, sizeof(legend), "%d.%03d / %d.%03d (mm)", minimum/1000,minimum%1000,maximum/1000,maximum%1000); break;
		default : break;
	}

	graph_layer_animate_to(dashboard_layer->s_graph_layer, title, legend, data);
}

void dashboard_layer_update_data(DashboardLayer *dashboard_layer, DashboardData *dashboard_data){
	if(dashboard_data){
		memcpy(&(dashboard_layer->dashboard_data), dashboard_data, sizeof(DashboardData));
	
		snprintf(dashboard_layer->text_name, sizeof(dashboard_layer->text_name), "%s", dashboard_data->name);
	
		if(dashboard_data->type == NAMain || dashboard_data->type == NAModule4){
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%s%d.%d°", 
				dashboard_data->temperature/10 == 0 && dashboard_data->temperature < 0 ? "-" : "",
				dashboard_data->temperature/10, 
				abs(dashboard_data->temperature)%10);
			snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "%d%%  %dppm", dashboard_data->humidity, dashboard_data->co2);
			dashboard_layer->dashboard_data.displayed_measure = Temperature;
		}
		else if(dashboard_data->type == NAModule1){
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%s%d.%d°", 
				dashboard_data->temperature/10 == 0 && dashboard_data->temperature < 0 ? "-" : "",
				dashboard_data->temperature/10, 
				abs(dashboard_data->temperature)%10);
			snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "%d%% %d.%dmb", dashboard_data->humidity, dashboard_data->pressure/10, dashboard_data->pressure%10);
			dashboard_layer->dashboard_data.displayed_measure = Temperature;
		}
		else if(dashboard_data->type == NAModule3){
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), "%d.%03d", dashboard_data->rain/1000, dashboard_data->rain%1000);
			snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "mm");
			dashboard_layer->dashboard_data.displayed_measure = Rain;
		}
		else {
		  snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), " ");
		  snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), " ");
		  dashboard_layer->dashboard_data.displayed_measure = Temperature;
		}
	
		dashboard_layer_animate_graph(dashboard_layer);
	}
	else {
		resetDisplay(dashboard_layer);
	}

	layer_mark_dirty(dashboard_layer->s_background);
}

void dashboard_layer_switch_graph(DashboardLayer *dashboard_layer){
	ModuleType type = dashboard_layer->dashboard_data.type;
	MeasureType displayed_measure = dashboard_layer->dashboard_data.displayed_measure;
	switch(type){
		case NAMain :
			switch(displayed_measure){
				case Temperature : 	displayed_measure = Humidity; break;
				case Humidity : 	displayed_measure = CO2; break;
				case CO2 : 			displayed_measure = Noise; break;
				case Noise : 		displayed_measure = Temperature; break;	
				default : break;
			}
			break;
		case NAModule1 :
			switch(displayed_measure){
				case Temperature : 	displayed_measure = Pressure; break;
				case Pressure : 	displayed_measure = Humidity; break;
				case Humidity : 	displayed_measure = Temperature; break;
				default : break;
			}
			break;
		case NAModule4 :
			switch(displayed_measure){
				case Temperature : 	displayed_measure = Humidity; break;
				case Humidity : 	displayed_measure = CO2; break;
				case CO2 : 			displayed_measure = Temperature; break;
				default : break;
			}
			break;
		default:
			break;
	}

	if(dashboard_layer->dashboard_data.displayed_measure != displayed_measure){
		dashboard_layer->dashboard_data.displayed_measure = displayed_measure;
		dashboard_layer_animate_graph(dashboard_layer);
	}	
}

void dashboard_layer_set_graph_hidden(DashboardLayer *dashboard_layer, bool hidden){
	layer_set_hidden(graph_layer_get_layer(dashboard_layer->s_graph_layer),hidden);
}