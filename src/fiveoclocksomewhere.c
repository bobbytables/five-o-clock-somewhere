#include <pebble.h>
  
#define KEY_TZ_OFFSET 42

  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_label_layer;
static TextLayer *s_city_layer;

static int32_t s_timezone_offset = 0;

static char *std[] = {"London","Paris","Athens","Moscow","Abu Dhabi",
                         "Calcutta","Kathmandu","Bangkok","Singapore",
                         "Tokyo","Guam","Magadan","Fiji","Midway Island",
                         "Hawaii","Alaska","Los Angeles","Santa Fe","Chicago",
                         "New York","Barbados","Buenos Aires","Sandwich Island","Cabo Verde"};

static char *dst[] = {"Liberia","London","Brussels","Saint-Petersburg","Abu Dhabi",
                         "Pakistan","Bangladesh","Ho Chi Minh","Hong Kong",
                         "Seoul","Melbourne","Srednekolymsk","Wake Island","Fakaofo",
                         "Honolulu","Alaska","Adamstown","San Francisco","Salt Lake City",
                         "Mexico City","New York","Buenos Aires","Sandwich Islands","Cabo Verde"};


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_find(iterator, KEY_TZ_OFFSET);
  s_timezone_offset = t->value->int32;
}



static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  // Create a long-lived buffer
  static char buffer[] = "00:00";
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }


  int difference = 17 - ((*tick_time).tm_hour - s_timezone_offset);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Offset: %d", difference);
  int month = (*tick_time).tm_mon;
  int day = (*tick_time).tm_mday;
  bool is_dst = false;
  if((3<month&&month<10) || (month==3&&day>28) || (month==10&&day<25)){
    is_dst = true;
  }
  if(difference<0){
    difference+=24;
  }
  if(is_dst){
    text_layer_set_text(s_city_layer, dst[difference]);
  }else{
    text_layer_set_text(s_city_layer, std[difference]);
  }
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window) {
  s_time_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");

  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  s_label_layer = text_layer_create(GRect(0, 115, 144, 20));
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_color(s_label_layer, GColorBlack);
  text_layer_set_text(s_label_layer, "Its five o'clock in");

  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
  
  s_city_layer = text_layer_create(GRect(0, 135, 144, 30));
  text_layer_set_background_color(s_city_layer, GColorClear);
  text_layer_set_text_color(s_city_layer, GColorBlack);

  text_layer_set_font(s_city_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_city_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_label_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_city_layer));
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callback for JS
  app_message_register_inbox_received(inbox_received_callback);
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
