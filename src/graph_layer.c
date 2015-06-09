#include <pebble.h>

#include "graph_layer.h"

//I really need to improve this code...
static void layer_update_callback(Layer *me, GContext* ctx) {
	GraphLayer* graph_layer = *(GraphLayer**)(layer_get_data(me));
	GRect layer_bounds = layer_get_bounds(me);

	uint16_t x_min = 10;
	uint16_t y_min = layer_bounds.size.h - 15;
	uint16_t y_max = 20;

	int16_t data_start = 0;
	uint16_t startY = (y_min + y_max) / 2;
	if(graph_layer->minimum < 0 && graph_layer->maximum < 0){
		data_start = graph_layer->maximum;
		startY = y_max;
		y_max += 10;
	}
	else if (graph_layer->minimum > 0 && graph_layer->maximum > 0){
		data_start = graph_layer->minimum;
		startY = y_min;
		y_min -= 10;
	}
	else if (graph_layer->minimum == graph_layer->maximum){
		data_start = graph_layer->minimum;
		startY = (y_min + y_max) / 2;
	}
	else {
		data_start = 0;
		startY = y_min + graph_layer->minimum * (y_min - y_max) / (graph_layer->maximum - graph_layer->minimum);
	}

	graphics_context_set_stroke_color(ctx, graph_layer->fgColor);
	if(data_start == 0){
		graphics_draw_line(ctx,GPoint(0,startY), GPoint(144,startY));
	}

	graphics_context_set_fill_color(ctx, graph_layer->fgColor);

	int16_t data = 0;
	uint16_t height = 0;
	uint16_t start = 0;
	for(size_t i=0; i<24; i++){
		data = graph_layer->data[i];
		// data = i % 2 ? -data : data;
		
		height = 0;
		if(graph_layer->maximum != graph_layer->minimum)
			height = abs((data - data_start) * (y_min - y_max) / (graph_layer->maximum - graph_layer->minimum));
		if(data_start != 0)
			height += 10;
		height = height * graph_layer->percent / 100;
		
		start = 0;
		if(data > data_start){
			start = startY - height;
		}
		else if(data < data_start){
			start = startY;
		}
		else if(data_start == graph_layer->minimum){
			start = startY - height;
		}
		else if(data_start == graph_layer->maximum){
			start = startY;
		}

		graphics_fill_rect(ctx, GRect(x_min + i*(4+1), start, 2, height), 0, GCornersAll);

	}

	graphics_context_set_text_color(ctx, graph_layer->fgColor);

	graphics_draw_text(ctx,
      graph_layer->title,
      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
      GRect(0, 0, layer_bounds.size.w, 20),
      GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);

	graphics_draw_text(ctx,
      graph_layer->legend,
      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
      GRect(0, layer_bounds.size.h - 16, layer_bounds.size.w, 15),
      GTextOverflowModeWordWrap,
      GTextAlignmentCenter,
      NULL);
}

Layer* graph_layer_get_layer(GraphLayer *graph_layer){
	return graph_layer->layer;
}

#ifdef PBL_COLOR
static void animationUpdate(Animation *animation, const AnimationProgress progress) {
#else
static void animationUpdate(Animation *animation, const uint32_t progress) {
#endif
	GraphLayer *graph_layer = (GraphLayer *)animation_get_context(animation);
	graph_layer->percent = progress * 100 / ANIMATION_NORMALIZED_MAX;
	layer_mark_dirty(graph_layer->layer);
}

static void animation_started(Animation *animation, void *data) {
	GraphLayer *graph_layer = (GraphLayer *)data;
	graph_layer->percent = 0;
	layer_mark_dirty(graph_layer->layer);
}

void animation_stopped(Animation *animation, bool finished, void *data) {
	GraphLayer *graph_layer = (GraphLayer *)data;
	graph_layer->percent = 100;
	layer_mark_dirty(graph_layer->layer);
}

GraphLayer* graph_layer_create(GRect frame, GColor fgColor){
	GraphLayer* graph_layer = malloc(sizeof(GraphLayer));
	graph_layer->fgColor = fgColor;

	graph_layer->layer = layer_create_with_data(frame, sizeof(GraphLayer*));
	layer_set_update_proc(graph_layer->layer, layer_update_callback);
	memcpy(layer_get_data(graph_layer->layer), &graph_layer, sizeof(GraphLayer*));

	return graph_layer;
}

void graph_layer_destroy(GraphLayer *graph_layer){
	if(graph_layer->animation){
		animation_unschedule(graph_layer->animation);
		animation_destroy(graph_layer->animation);
	}
	layer_destroy(graph_layer->layer);
	free(graph_layer);
}


void graph_layer_animate_to(GraphLayer *graph_layer, char* title, char* legend, int16_t *data){
	memcpy(graph_layer->title, title, sizeof(graph_layer->title));
	memcpy(graph_layer->legend, legend, sizeof(graph_layer->legend));
	memcpy(graph_layer->data, data, sizeof(graph_layer->data));
	graph_layer->maximum = INT16_MIN;
	graph_layer->minimum = INT16_MAX;
	int16_t value = 0;
	for(size_t i=0; i<24; i++){
		value = graph_layer->data[i];
		// value = i % 2 ? -value : value;
		if(value  > graph_layer->maximum)
			graph_layer->maximum = value;
		if(value < graph_layer->minimum)
			graph_layer->minimum = value;
	}
	graph_layer->percent = 0;

	graph_layer->animation = animation_create();
	graph_layer->animImpl.update = animationUpdate;
	animation_set_handlers(graph_layer->animation, (AnimationHandlers) {
		.started = (AnimationStartedHandler) animation_started,
		.stopped = (AnimationStoppedHandler) animation_stopped,
	}, graph_layer);

	animation_set_duration(graph_layer->animation, 800);
	animation_set_implementation(graph_layer->animation, &(graph_layer->animImpl));
	animation_schedule(graph_layer->animation);
}
