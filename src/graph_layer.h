#pragma once

#include <pebble.h>

typedef struct {
	Layer       *layer;

	GColor fgColor;

	Animation* animation;
	AnimationImplementation animImpl;

	char 	min[20];
	char 	max[20];

	int16_t data[24];
	int16_t minimum;
	int16_t maximum;

	int	percent;
} GraphLayer;

GraphLayer* graph_layer_create(GRect frame, GColor fgColor);

void graph_layer_destroy(GraphLayer *graph_layer);

Layer* graph_layer_get_layer(GraphLayer *graph_layer);

void graph_layer_animate_to(GraphLayer *graph_layer, char* title, char* legend, int16_t *data);

