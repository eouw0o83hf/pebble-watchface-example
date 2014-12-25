#include <pebble.h>
	
/// The main watchface window
static Window *s_main_window;
/// To show text on the screen
static TextLayer *s_time_layer;

// Oh right, declaration order matters in C.
// So the file's kind of written upside-down
// in terms of execution/dependencies
static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	// long-lived
	static char buffer[] = "00:00";
	
	// move to buffer
	if(clock_is_24h_style() == true) {
		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}
	
	// persist to UI
	text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}

/// Called when the window is loaded
static void main_window_load(Window *window) {
	// Be sure to _destroy this on _unload()
	s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
	
	// Text/UI initialization
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
		
	// Make it pretty
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

/// Called when the window is unloaded
static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layer);
}

static void init() {
	// SDK best practice: make sure _create() methods
	// in init() are always paired with _destroy()
	// methods in deinit()
	s_main_window = window_create();
	
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});
	
	window_stack_push(s_main_window, true);
	update_time();
	
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	window_destroy(s_main_window);
}

/// Pretty standard service wireup
int main(void) {
	init();
	app_event_loop();
	deinit();
}