#include <pebble.h>
#include "main_window.h"

#include "comm.h"
#include "config.h"
#include "data.h"
#include "vibration.h"

int currentPercentileProgressTowardsStepGoal = 0;
static int s_battery_level;

//Battery level indicator
static void battery_callback(BatteryChargeState state) {
  s_battery_level = state.charge_percent;
  if(s_battery_level <= 20){
    main_window_show_low_battery();
  } else {
    main_window_hide_low_battery();
  }
}

//Vibrate if we get to 10%
static void checkIfWeShouldPingUser(){
  
  int steps = data_get_steps();
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Health Handler: Steps=%d", steps);
  int percentileProgress = (int)(((float)steps/(float)usersStepGoal)*10);
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "percentileProgress: %d", percentileProgress);
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "currentPercentileProgressTowardsStepGoal: %d", currentPercentileProgressTowardsStepGoal);
  if(percentileProgress != currentPercentileProgressTowardsStepGoal){
    currentPercentileProgressTowardsStepGoal = percentileProgress;
    if(percentileProgress <= 10){
      if(experimentConditionThatUserIsIn == VIBRATE_ONLY_CONDITION){
        comm_send_data(CAUSE_OF_SNAPSHOT_WAS_VIBRATION_FOR_MILESTONE);
        vibrateNTimes(percentileProgress);
      } else if (experimentConditionThatUserIsIn == LANDAY_REVISED_COMBINATION_CONDITION){
        comm_send_data(CAUSE_OF_SNAPSHOT_WAS_VIBRATION_FOR_MILESTONE);
        vibrateNLongTimes(1);
        //main_window_show_only_percentage();
      }
    }
  }
}

//Check condition to decide if need to ping at 10%
static void health_handler(HealthEventType event, void *context) {
  if((experimentConditionThatUserIsIn == VIBRATE_ONLY_CONDITION) || (experimentConditionThatUserIsIn == LANDAY_REVISED_COMBINATION_CONDITION)){
    checkIfWeShouldPingUser();
  }
}

//Initialize Health Handler
static void intitializeHealth(void){
  if(!health_service_events_subscribe(health_handler, NULL)) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }
}

//Check Battery level
static void checkIfLowBattery(void){
  s_battery_level = battery_state_service_peek().charge_percent;
  if(s_battery_level <= 20){
    main_window_show_low_battery();
  } else {
    main_window_hide_low_battery();
  }
}

//Initialization
static void init(void) {
  comm_init(64, 64);
  // Shouldn't that function be called when the watch reconnects
  updateLocalVariablesFromPersistentStorageValues();
  main_window_push();
  intitializeHealth();
  battery_state_service_subscribe(battery_callback);
  checkIfLowBattery();
}

static void deinit(void) {
  main_window_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}