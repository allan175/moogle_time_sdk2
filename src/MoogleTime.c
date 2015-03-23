/*

     <sprite>
+-----------------+
|                 |
|   HP   11/23    |
|   MP   30/59    |
+-----------------+

*/
#include <pebble.h>

static Window* window = NULL;
static TextLayer* textMinutesLayer = NULL;
static TextLayer* textHoursLayer = NULL;
static BitmapLayer* backgroundLayer = NULL;

int currentBackground = -1; //0=Awake, 1=Tired, 2=Asleep
GBitmap* background = NULL;
GFont* finalFantasyFont = NULL;

void maybeChangeBackground(int hour)
{
	if (currentBackground != 2 && (hour >= 0 && hour < 6)) {
		app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Dead\n");
		if (background != NULL)
			gbitmap_destroy(background);
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOG_DEAD);
		bitmap_layer_set_bitmap(backgroundLayer, background);
		currentBackground = 2;
	}
	else if (currentBackground != 1 && (hour >= 6 && hour < 9)) {
		app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Hurt\n");
		if (background != NULL)
			gbitmap_destroy(background);
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOG_HURT);
		bitmap_layer_set_bitmap(backgroundLayer, background);
		currentBackground = 1;
	}
	else if (currentBackground != 0 && (hour >= 9 && hour < 21)) {
		app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Ok\n");
		if (background != NULL)
			gbitmap_destroy(background);
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOG_OK);
		bitmap_layer_set_bitmap(backgroundLayer, background);
		currentBackground = 0;
	}
	else if (currentBackground != 1 && (hour >= 21 && hour < 24)) {
		app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Hurt\n");
		if (background != NULL)
			gbitmap_destroy(background);
		background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOG_HURT);
		bitmap_layer_set_bitmap(backgroundLayer, background);
		currentBackground = 1;
	}
}

static void tickHandler(struct tm* tickTime, TimeUnits unitsChanged)
{
	app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Tick\n");
	static char hoursText[] = "    00 00";
	static char minutesText[] = "    00 00";
	  
	char *minutesFormat;
	char *hoursFormat;
	
	minutesFormat = "MP  %M/59";
	if (clock_is_24h_style())
	  hoursFormat = "HP  %H/23";
	else
	  hoursFormat = "HP  %I/12";
	
	strftime(hoursText, sizeof(hoursText), hoursFormat, tickTime);
	strftime(minutesText, sizeof(minutesText), minutesFormat, tickTime);

	text_layer_set_text(textMinutesLayer, minutesText);
	text_layer_set_text(textHoursLayer, hoursText);

	maybeChangeBackground(tickTime->tm_hour);
}

static void windowLoad(Window* window)
{
	Layer* windowLayer = window_get_root_layer(window);
	
	finalFantasyFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FFVI_40));

	backgroundLayer = bitmap_layer_create(layer_get_frame(windowLayer));
	bitmap_layer_set_compositing_mode(backgroundLayer, GCompOpAssign);
	layer_add_child(windowLayer, bitmap_layer_get_layer(backgroundLayer));

	textMinutesLayer = text_layer_create(GRect(12, 100, 144-12, 168-100));
	text_layer_set_text_alignment(textMinutesLayer, GTextAlignmentCenter);
	text_layer_set_background_color(textMinutesLayer, GColorClear);
	text_layer_set_text_color(textMinutesLayer, GColorWhite);
	text_layer_set_font(textMinutesLayer, finalFantasyFont);
	layer_add_child(windowLayer, text_layer_get_layer(textMinutesLayer));
	
	textHoursLayer = text_layer_create(GRect(12, 65, 144-12, 168-65));
	text_layer_set_text_alignment(textHoursLayer, GTextAlignmentCenter);
	text_layer_set_background_color(textHoursLayer, GColorClear);
	text_layer_set_text_color(textHoursLayer, GColorWhite);
	text_layer_set_font(textHoursLayer, finalFantasyFont);
	layer_add_child(windowLayer, text_layer_get_layer(textHoursLayer));
}

static void windowUnload(Window* window)
{
	if (textMinutesLayer != NULL) {
		text_layer_destroy(textMinutesLayer);
		textMinutesLayer = NULL;
	}

	if (textHoursLayer != NULL) {
		text_layer_destroy(textHoursLayer);
		textHoursLayer = NULL;
	}

	if (backgroundLayer != NULL) {
		bitmap_layer_destroy(backgroundLayer);
		backgroundLayer = NULL;
	}
	
	if (finalFantasyFont != NULL) {
		fonts_unload_custom_font(finalFantasyFont);
		finalFantasyFont = NULL;
	}
}

static void init(void)
{
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = windowLoad,
		.unload = windowUnload,
	});

	const bool animated = true;
	window_stack_push(window, animated);

	tick_timer_service_subscribe(MINUTE_UNIT, tickHandler);
	
	time_t now = time(NULL);
	struct tm* nowTM = localtime(&now);
	tickHandler(nowTM, MINUTE_UNIT);
}

static void deinit(void)
{
	window_destroy(window);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}
