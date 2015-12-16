#include <pebble.h>
#include <ctype.h>
	
static MenuLayer *s_menu;
#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 4

static Window * window;

static AppTimer *subTimer;

static int lastSendItem = -1;
static int lastSendSec = -1;

typedef struct _LBsMenuItem {
    const char * title;
    const char * subtitle;
} LBsMenuItem;
	
static LBsMenuItem first_menu_items[13];
static LBsMenuItem second_menu_items[1];

//Method Headers: #######################################
void sendPiCommand(int number);
void sendCommand(int number);
//end methodHeaders


//Menu callbacks###############################
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
     break;
    default:
      return 0;
  }
}
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "ClockTurn");
     break;
     default:
     break;

  }
}
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  switch (cell_index->section) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, first_menu_items[cell_index->row].title, first_menu_items[cell_index->row].subtitle, NULL);
      break;
  }
}


static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
   //todo add other
   switch(cell_index->section){
      case 0:
      first_menu_items[cell_index->row].subtitle = "Sending...";
lastSendSec = 1;// again this is reversed now
lastSendItem = cell_index->row;
      switch(cell_index->row)
       {
case 0:
sendCommand(0x00);
break;
case 1:
sendCommand(0x01);
break;
case 2:
sendCommand(0x02);
break;
case 3:
sendCommand(0x03);
break;
}
layer_mark_dirty(menu_layer_get_layer(s_menu));
      break;
      
	}
  // Mark the layer to be updated
  layer_mark_dirty(menu_layer_get_layer(s_menu));
}


//End Menu#########################################



/************************************ App *************************************/

void resetSubs()
{
first_menu_items[0].subtitle = "Move Clock forward!";
first_menu_items[1].subtitle = "Normal Clockspeed!";
first_menu_items[2].subtitle = "Move Clock Backward!";
first_menu_items[3].subtitle = "Ignite whatever is connected";

layer_mark_dirty(menu_layer_get_layer(s_menu));
}

static void handle_timer(void *data) {
  resetSubs();
}	


void my_out_sent_handler(DictionaryIterator *sent, void *context) {
		first_menu_items[lastSendItem].subtitle = "Sent!";
 	    layer_mark_dirty(menu_layer_get_layer(s_menu));
	    subTimer = app_timer_register(1000,handle_timer,&lastSendItem);
   APP_LOG(APP_LOG_LEVEL_ERROR, "Sent");
}

void my_out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
// outgoing message failed

		first_menu_items[lastSendItem].subtitle = "Failed";
	layer_mark_dirty(menu_layer_get_layer(s_menu));
	subTimer = app_timer_register(1000 ,handle_timer, &lastSendItem);
}

void my_in_rcv_handler(DictionaryIterator *received, void *context) {
	  int cookie = 100;
      subTimer = app_timer_register(1000 ,handle_timer, &cookie);
   APP_LOG(APP_LOG_LEVEL_ERROR, "rec");
}

void my_in_drp_handler(AppMessageResult reason, void *context) {
   
}




void sendCommand(int number)
{
//send normal Command -starts with 0x00-
Tuplet value = TupletInteger(0x00,number);
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  if (iter == NULL)
    return;
  
  dict_write_tuplet(iter, &value);
  dict_write_end(iter);
  
  app_message_outbox_send();
}

// This initializes the menu upon window load
static void window_load(Window *me) {
   first_menu_items[0].subtitle = "Forward";
first_menu_items[1].subtitle = "Normal";
first_menu_items[2].subtitle = "Backward";
first_menu_items[3].subtitle = "Ignite";
   
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
	
  first_menu_items[0] = (LBsMenuItem){
    .title = "Forward",
	.subtitle = "Move Clock forward!",
  };
  first_menu_items[1] = (LBsMenuItem){
    .title = "Normal",
    .subtitle = "Normal Clockspeed!",
  };
  first_menu_items[2] = (LBsMenuItem){
    .title = "Backward",
    .subtitle = "Move Clock Backward!",
  };
  first_menu_items[3] = (LBsMenuItem){
    .title = "Ignite",
	.subtitle = "Ignite whatever is connected",
  };
  

// Create the menu layer
  s_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  #ifdef PBL_SDK_3
   menu_layer_set_normal_colors(s_menu, GColorCyan , GColorBlack);
   menu_layer_set_highlight_colors(s_menu,GColorOrange,GColorBlack);
 #endif
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu));
   
   
}

static void window_unload (Window *me)
{
     menu_layer_destroy(s_menu);
	//not needed basically
}


void handle_init() {
	
	  // Attach our desired button functionality
  window = window_create();

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
	
  window_stack_push(window, true /* Animated */);
	
	
	app_message_register_inbox_received(my_in_rcv_handler);
	app_message_register_inbox_dropped(my_in_drp_handler);
	app_message_register_outbox_failed(my_out_fail_handler);
	app_message_register_outbox_sent(my_out_sent_handler);
	app_message_open(64,64);
   


}

void handle_deinit() {
  window_destroy(window);
   

}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
