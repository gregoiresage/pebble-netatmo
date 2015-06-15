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

#ifdef PBL_SDK_3
	dashboard_layer->main_font = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
#else
	dashboard_layer->main_font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
#endif

	resetDisplay(dashboard_layer);
	
	// s_background
	dashboard_layer->s_background = layer_create_with_data(frame, sizeof(DashboardLayer*));
	layer_set_update_proc(dashboard_layer->s_background, cb_background_draw);
	memcpy(layer_get_data(dashboard_layer->s_background), &dashboard_layer, sizeof(DashboardLayer*));
	
	// s_name
	dashboard_layer->s_name = text_layer_create(GRect(0, 4, 144, 20));
	text_layer_set_background_color(dashboard_layer->s_name, GColorClear);
	text_layer_set_font(dashboard_layer->s_name, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text_color(dashboard_layer->s_name, dashboard_layer->fgColor);
	text_layer_set_text(dashboard_layer->s_name, dashboard_layer->text_name);
	text_layer_set_text_alignment(dashboard_layer->s_name, GTextAlignmentCenter);
	layer_add_child(dashboard_layer->s_background, text_layer_get_layer(dashboard_layer->s_name));
	
	// s_main
#ifdef PBL_SDK_3
	dashboard_layer->s_main = text_layer_create(GRect(0, 19, 144, 45));
#else
	dashboard_layer->s_main = text_layer_create(GRect(0, 15, 144, 45));
#endif
	text_layer_set_font(dashboard_layer->s_main, dashboard_layer->main_font);
	text_layer_set_background_color(dashboard_layer->s_main, GColorClear);
	text_layer_set_text_color(dashboard_layer->s_main, dashboard_layer->fgColor);
	text_layer_set_text(dashboard_layer->s_main, dashboard_layer->text_main);
	text_layer_set_text_alignment(dashboard_layer->s_main, GTextAlignmentCenter);
	layer_add_child(dashboard_layer->s_background, text_layer_get_layer(dashboard_layer->s_main));

	// s_subtitle
	dashboard_layer->s_subtitle = text_layer_create(GRect(0, 60, 144, 27));
	text_layer_set_background_color(dashboard_layer->s_subtitle, GColorClear);
	text_layer_set_text_color(dashboard_layer->s_subtitle, dashboard_layer->fgColor);
	text_layer_set_text(dashboard_layer->s_subtitle, dashboard_layer->text_subtitle);
	text_layer_set_text_alignment(dashboard_layer->s_subtitle, GTextAlignmentCenter);
	text_layer_set_font(dashboard_layer->s_subtitle, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
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

#ifdef PBL_COLOR
static void animationUpdate(Animation *animation, const AnimationProgress progress) {
#else
static void animationUpdate(Animation *animation, const uint32_t progress) {
#endif
	DashboardLayer *dashboard_layer = (DashboardLayer *)animation_get_context(animation);
	int percent = progress * 100 / ANIMATION_NORMALIZED_MAX;
	DashboardData dashboard_data = dashboard_layer->dashboard_data;

	switch(dashboard_data.displayed_measure){
		case Temperature: 	
			;
			int16_t temperature = dashboard_data.temperature*percent/100;
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%s%d.%d°", 
				temperature/10 == 0 && temperature < 0 ? "-" : "",
				temperature/10, 
				abs(temperature)%10);
			break;
		case Humidity:		
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%d", dashboard_data.humidity*percent/100);break;
		case CO2:			
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%d", dashboard_data.co2*percent/100);break;
		case Noise:			
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%d", dashboard_data.noise*percent/100);break;
		case Pressure:
			;	
			uint16_t pressure = dashboard_data.pressure*percent/100;
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%d.%d", pressure/10, pressure%10);break;
		case Rain:
			;	
			uint16_t rain = dashboard_data.rain*percent/100;
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), 
				"%d.%03d", rain/1000, rain%1000); break;
		default:break;
	}
}

static void dashboard_layer_animate_graph(DashboardLayer *dashboard_layer){
	MeasureType displayed_measure = dashboard_layer->dashboard_data.displayed_measure;
	char max_t[20] = "";
	char min_t[20] = "";
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
		case WindStrength : 		
			data = dashboard_layer->dashboard_data.data_wind_strength; break;
		case GustStrength : 		
			data = dashboard_layer->dashboard_data.data_gust_strength; break;
		default : break;
	}

	int16_t maximum = INT16_MIN;
	int16_t minimum = INT16_MAX;
	for(size_t i=0; i<24; i++){
		if(data[i] > maximum)
			maximum = data[i];
		if(data[i] < minimum)
			minimum = data[i];
	}

	switch(displayed_measure){
		case Temperature : 	
			snprintf(max_t, sizeof(max_t), "%s%d.%d °", 
				maximum == 0 && maximum < 0 ? "-" : "",
				maximum/10,
				abs(maximum)%10);
			snprintf(min_t, sizeof(min_t), "%s%d.%d °", 
				minimum/10 == 0 && minimum < 0 ? "-" : "",
				minimum/10,
				abs(minimum)%10); 
			break;
		case Humidity : 	
			snprintf(max_t, sizeof(max_t), "%d %%", maximum); 
			snprintf(min_t, sizeof(min_t), "%d %%", minimum); 
			break;
		case Pressure : 	
			snprintf(max_t, sizeof(max_t), "%d.%d mb", maximum/10,maximum%10); 
			snprintf(min_t, sizeof(min_t), "%d.%d mb", minimum/10,minimum%10); 
			break;
		case CO2 : 
			snprintf(max_t, sizeof(max_t), "%d ppm", maximum); 
			snprintf(min_t, sizeof(min_t), "%d ppm", minimum); break;
		case Noise : 		
			snprintf(max_t, sizeof(max_t), "%d dB", maximum); 
			snprintf(min_t, sizeof(min_t), "%d dB", minimum); break;
		case Rain : 		
			snprintf(max_t, sizeof(max_t), "%d.%03d mm", maximum/1000,maximum%1000); 
			snprintf(min_t, sizeof(min_t), "%d.%03d mm", minimum/1000,minimum%1000); break;
		case WindStrength : 		
			snprintf(max_t, sizeof(max_t), "%d km/h", maximum); 
			snprintf(min_t, sizeof(min_t), "%d km/h", minimum); break;
		case GustStrength : 		
			snprintf(max_t, sizeof(max_t), "%d km/h", maximum); 
			snprintf(min_t, sizeof(min_t), "%d km/h", minimum); break;
		default : break;
	}

	graph_layer_animate_to(dashboard_layer->s_graph_layer, max_t, min_t, data);
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
			snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Rain (mm)");
			dashboard_layer->dashboard_data.displayed_measure = Rain;
		}
		else if(dashboard_data->type == NAModule2){
			snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), "%d", dashboard_data->wind_strength);
			snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Gust : %d km/h", dashboard_data->gust_strength);
			dashboard_layer->dashboard_data.displayed_measure = WindStrength;
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

static void animation_stopped(Animation *animation, bool finished, void *data) {
#ifndef PBL_SDK_3
	animation_destroy(animation);
#endif
}

void dashboard_layer_switch_graph(DashboardLayer *dashboard_layer){
	DashboardData dashboard_data 	= dashboard_layer->dashboard_data;
	MeasureType displayed_measure 	= dashboard_data.displayed_measure;
	switch(dashboard_data.type){
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
		case NAModule2 :
			switch(displayed_measure){
				case WindStrength : displayed_measure = GustStrength; break;
				case GustStrength : displayed_measure = WindStrength; break;
				default : break;
			}
			break;
		case NAModule3 :
			switch(displayed_measure){
				case Rain : 		dashboard_layer->dashboard_data.displayed_measure = Temperature; displayed_measure = Rain; break;
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

	if(dashboard_layer->dashboard_data.displayed_measure != displayed_measure)
	{
		dashboard_layer->dashboard_data.displayed_measure = displayed_measure;

		switch(displayed_measure){
			case Temperature: 	
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Temperature (°)"); 
				break;
			case Humidity:		
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Humidity (%%)"); break;
			case CO2:			
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "CO² (ppm)"); break;
			case Noise:			
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Noise (dB)"); break;
			case Pressure:		
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Pressure (mb)"); break;
			case Rain:			
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Rain (mm)"); break;
			case WindStrength:			
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Wind (km/h)"); break;
			case GustStrength:			
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "Gust (km/h)"); break;
			default:	
				snprintf(dashboard_layer->text_main, sizeof(dashboard_layer->text_main), "000");		
				snprintf(dashboard_layer->text_subtitle, sizeof(dashboard_layer->text_subtitle), "---"); break;
		}

		dashboard_layer->animation = animation_create();
		dashboard_layer->animImpl.update = animationUpdate;
		animation_set_handlers(dashboard_layer->animation, (AnimationHandlers) {
			.stopped = animation_stopped,
		}, dashboard_layer);

		animation_set_duration(dashboard_layer->animation, 500);
		animation_set_implementation(dashboard_layer->animation, &(dashboard_layer->animImpl));
		animation_schedule(dashboard_layer->animation);

		dashboard_layer_animate_graph(dashboard_layer);
	}	
}

void dashboard_layer_set_graph_hidden(DashboardLayer *dashboard_layer, bool hidden){
	if(hidden){
		dashboard_layer_update_data(dashboard_layer, &dashboard_layer->dashboard_data);
	}
	else {
		dashboard_layer_switch_graph(dashboard_layer);
	}
	layer_set_hidden(graph_layer_get_layer(dashboard_layer->s_graph_layer),hidden);
}