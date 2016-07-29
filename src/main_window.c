#include <pebble.h>
#include "main_window.h"

static Window *s_window;
static TextLayer *s_time_layer, *s_steps_layer, *s_steps_word_layer, *s_low_battery_layer;

bool stepsAreCurrentlyBeingDisplayed = false;
int delayForSendingDataToServer = 0;
int delayForIntermittentlyPingingUser = 50;

static void toggleStepVisualDisplay(){
  if(stepsAreCurrentlyBeingDisplayed == false){
      int steps = data_get_steps();
      main_window_show_steps(steps);
      if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Select Pressed: Steps=%d", steps);
    } else {
      main_window_hide_steps();
    }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Experiment Condition that user is in: %d", experimentConditionThatUserIsIn);
  comm_send_data(CAUSE_OF_SNAPSHOT_WAS_SIDE_BUTTON_PRESSED);
  
  if(experimentConditionThatUserIsIn == VIBRATE_ONLY_CONDITION){
    int steps = data_get_steps();
    int percentileProgress = (int)(((float)steps/(float)usersStepGoal)*10);
    vibrateNTimes(percentileProgress);
  } else if(experimentConditionThatUserIsIn == LANDAY_REVISED_COMBINATION_CONDITION){
    if(stepsAreCurrentlyBeingDisplayed == false){
      main_window_show_only_percentage();
    } else {
      main_window_hide_steps();
    }
  } else {
    toggleStepVisualDisplay();
  }
}

static void resetMostRecentSnapshotVariable(){
  mostRecentSnapshot = 1;
}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  main_window_update_time(tick_time);
  main_window_hide_steps();
  
  resetMostRecentSnapshotVariable();
  
  // If in the UNASSIGNED condition, call home every minute until assigned to an experiment condition
  if(experimentConditionThatUserIsIn == UNASSIGNED_CONDITION){
    comm_send_data(CAUSE_OF_SNAPSHOT_WAS_CALLING_HOME_TO_BE_ASSIGNED_EXPERIMENTAL_CONDITION);
  }
  
  delayForSendingDataToServer++;
  if(delayForSendingDataToServer >= VIBRATE_ONLY_LOGGING_DATA_INTERVAL){
    comm_send_data(CAUSE_OF_SNAPSHOT_WAS_INTERVAL);
    delayForSendingDataToServer = 0;
  }
  
  if(experimentConditionThatUserIsIn == COMBINATION_CONDITION){
    delayForIntermittentlyPingingUser--;
    if(delayForIntermittentlyPingingUser <=0){
      if((tick_time->tm_hour >= QUIET_HOURS_END) && (tick_time->tm_hour < QUIET_HOURS_START)){
        int steps = data_get_steps();
        int percentileProgress = (int)(((float)steps/(float)usersStepGoal)*10);
        if(percentileProgress < 10){
          comm_send_data(CAUSE_OF_SNAPSHOT_WAS_RANDOM_VIBRATION);
          vibrateNLongTimes(1);
        }
      }
      // Random delay falls between 40 and 70 minutes
      int salt = rand() % 31;
      delayForIntermittentlyPingingUser = 40 + salt;
    }
  }
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(experimentConditionThatUserIsIn == LAB_TEST_USERS_VIBRATION_AWARENESS_CONDITION){
    labTestVibrateOrdered();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(experimentConditionThatUserIsIn == LAB_TEST_USERS_VIBRATION_AWARENESS_CONDITION){
    labTestVibrateRandomOrder();
  }
}


void main_window_update_time(struct tm *tick_time) {
  static char s_buffer[8];
  memset(s_buffer, 0x00, sizeof(s_buffer));
  strftime(s_buffer, sizeof(s_buffer), "%l:%M", tick_time);
  text_layer_set_text(s_time_layer,s_buffer + ((' ' == s_buffer[0]) ? 1 : 0));
}

void main_window_show_only_percentage(){
  stepsAreCurrentlyBeingDisplayed = true;
  
  int steps = data_get_steps();
  
  static char s_buffer[32];
    memset(s_buffer, 0x00, sizeof(s_buffer));
    int stepPercentage = (int)(((float)steps/(float)usersStepGoal)*100);
    
    // Make sure that percentage is not greater than 100
    if(stepPercentage > 100) stepPercentage = 100;
    
    snprintf(s_buffer, sizeof(s_buffer), "%d%%", stepPercentage);
    text_layer_set_text(s_steps_word_layer, s_buffer);
}

void main_window_show_steps(int steps) {
  
  if(experimentConditionThatUserIsIn != VIBRATE_ONLY_CONDITION){
    
    stepsAreCurrentlyBeingDisplayed = true;
    if(steps > 0) {
      static char s_buffer[32];
      memset(s_buffer, 0x00, sizeof(s_buffer));
      snprintf(s_buffer, sizeof(s_buffer), "%d", steps);
      text_layer_set_text(s_steps_layer, s_buffer);
    } else {
      text_layer_set_text(s_steps_layer, "0");
    }
    
    
    static char s_buffer[32];
    memset(s_buffer, 0x00, sizeof(s_buffer));
    int stepPercentage = (int)(((float)steps/(float)usersStepGoal)*100);
    
    // Make sure that percentage is not greater than 100
    if(stepPercentage > 100) stepPercentage = 100;
    
    snprintf(s_buffer, sizeof(s_buffer), "%d%%", stepPercentage);
    text_layer_set_text(s_steps_word_layer, s_buffer);
  }
}

void main_window_hide_steps(){
  stepsAreCurrentlyBeingDisplayed = false;
  text_layer_set_text(s_steps_layer, "");
  text_layer_set_text(s_steps_word_layer, "");
}

void main_window_show_low_battery(){
  text_layer_set_text(s_low_battery_layer, "Low Battery");
}

void main_window_hide_low_battery(){
  text_layer_set_text(s_low_battery_layer, "");
}


static TextLayer* make_text_layer(GRect bounds, GFont font) {
  TextLayer *this = text_layer_create(bounds);
  text_layer_set_background_color(this, GColorClear);
  text_layer_set_text_alignment(this, GTextAlignmentCenter);
  text_layer_set_font(this, font);
  return this;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_steps_layer = make_text_layer(grect_inset(bounds,
                                              GEdgeInsets(86, 5, 0, 5)),
                                  fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(s_steps_layer));
  
  s_time_layer = make_text_layer(grect_inset(bounds, GEdgeInsets(40, 5)),
                     fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  s_steps_word_layer = make_text_layer(grect_inset(bounds,
                                              GEdgeInsets(125, -5, 0, 5)),
                                  fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_steps_word_layer));
  text_layer_set_text(s_steps_word_layer, "");
  
  s_low_battery_layer = make_text_layer(grect_inset(bounds,
                                              GEdgeInsets(5, 5, 0, 5)),
                                  fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_low_battery_layer));
  text_layer_set_text(s_low_battery_layer, "Low Battery");
  
}

static void window_unload(Window *window) {
  text_layer_destroy(s_steps_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_steps_word_layer);
  text_layer_destroy(s_low_battery_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void initializeClock(void){
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  time_t now = time(NULL);
  struct tm *time_now = localtime(&now);
  tick_handler(time_now, MINUTE_UNIT); 
}

void main_window_push() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  
  window_set_click_config_provider(s_window, click_config_provider);
  initializeClock();
}

void main_window_deinit() {
  window_destroy(s_window);
}