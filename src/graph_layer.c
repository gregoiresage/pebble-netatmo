#include <pebble.h>

#include "graph_layer.h"

static void layer_update_callback(Layer *me, GContext* ctx) {
	GraphLayer* graph_layer = *(GraphLayer**)(layer_get_data(me));
	GRect layer_bounds = layer_get_bounds(me);

	uint16_t x_min = 10;
	uint16_t y_min = layer_bounds.size.h - 11;
	uint16_t y_max = 25;

	int16_t data_start = 0;
	if(graph_layer->minimum < 0 && graph_layer->maximum < 0){
		data_start = graph_layer->maximum + (graph_layer->maximum - graph_layer->minimum) * 10 / 100;
	}
	else if (graph_layer->minimum > 0 && graph_layer->maximum > 0){
		data_start = graph_layer->minimum - (graph_layer->maximum - graph_layer->minimum) * 10 / 100;
	}

	uint16_t startY = 0;
	if(graph_layer->maximum == 0 && graph_layer->minimum == 0){
		startY = (y_min + y_max) / 2;
	}
	else {
		startY = y_min - abs((graph_layer->minimum - data_start) * (y_max - y_min) / (graph_layer->maximum - data_start));
	}

	graphics_context_set_stroke_color(ctx, graph_layer->fgColor);
	if(data_start == 0){
		graphics_draw_line(ctx,GPoint(0,startY), GPoint(144,startY));
	}

	if(graph_layer->maximum != 0 || graph_layer->minimum != 0){
		graphics_context_set_fill_color(ctx, graph_layer->fgColor);
		for(size_t i=0; i<24; i++){
			int16_t data = graph_layer->data[i];
			
			uint16_t height = 0;
			if(graph_layer->maximum != data_start){
				height = abs((data - data_start) * (y_min - y_max) / (graph_layer->maximum - data_start));
				height = height * graph_layer->percent / 100;
			}
			
			if(data > data_start){
				graphics_fill_rect(ctx, GRect(x_min + i*(4+1), startY - height, 2, height), 0, GCornersAll);
			}
			else {
				graphics_fill_rect(ctx, GRect(x_min + i*(4+1), startY, 2, height), 0, GCornersAll);
			}
		}
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

void animationUpdate(struct Animation *animation, const uint32_t time_normalized){
	GraphLayer *graph_layer = (GraphLayer *)animation_get_context(animation);
	graph_layer->percent = time_normalized * 100 / ANIMATION_NORMALIZED_MAX;
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

	graph_layer->animation = animation_create();
	graph_layer->animImpl.update = animationUpdate;
	animation_set_handlers(graph_layer->animation, (AnimationHandlers) {
		.started = (AnimationStartedHandler) animation_started,
		.stopped = (AnimationStoppedHandler) animation_stopped,
	}, graph_layer);

	animation_set_duration(graph_layer->animation, 1400);
	animation_set_implementation(graph_layer->animation, &(graph_layer->animImpl));

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
	graph_layer->maximum = -32000;
	graph_layer->minimum = 32000;
	for(size_t i=0; i<24; i++){
		if(graph_layer->data[i] > graph_layer->maximum)
			graph_layer->maximum = graph_layer->data[i];
		if(graph_layer->data[i] < graph_layer->minimum)
			graph_layer->minimum = graph_layer->data[i];
	}
	graph_layer->percent = 0;
	animation_schedule(graph_layer->animation);
}
