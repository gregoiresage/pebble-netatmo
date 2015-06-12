#pragma once

#include <pebble.h>
#include "dashboard_data.h"
#include "graph_layer.h"

typedef struct DashboardLayer {
	GFont main_font;
	// GFont s_res_gothic_18_bold;
	// GFont s_res_gothic_14_bold;

	GColor bgColor;
	GColor fgColor;

	char text_name[40];
	char text_main[10];
	char text_subtitle[20];

	DashboardData dashboard_data;
	
	Layer 		*s_background;
	
	TextLayer 	*s_name;
	TextLayer 	*s_main;
	TextLayer 	*s_subtitle;

	Animation* animation;
	AnimationImplementation animImpl;
	
	GraphLayer *s_graph_layer;
} DashboardLayer;

DashboardLayer* dashboard_layer_create(GRect frame, GColor bgColor);

void dashboard_layer_destroy(DashboardLayer *dashboard_layer);

Layer* dashboard_layer_get_layer(DashboardLayer *dashboard_layer);

void dashboard_layer_update_data(DashboardLayer *dashboard_layer, DashboardData *dashboard_data);

void dashboard_layer_switch_graph(DashboardLayer *dashboard_layer);

void dashboard_layer_set_graph_hidden(DashboardLayer *dashboard_layer, bool hidden);
