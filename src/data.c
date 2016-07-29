#include <pebble.h>
#include "data.h"

int experimentConditionThatUserIsIn = UNASSIGNED_CONDITION;
int usersStepGoal = 10000;

int data_get_steps() {
  HealthMetric metric = HealthMetricStepCount;
  HealthServiceAccessibilityMask result = health_service_metric_accessible(metric, time_start_of_today(), time(NULL));

  int steps = 0;
  if(result == HealthServiceAccessibilityMaskAvailable) {
    steps = (int)health_service_sum_today(metric);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health service not available");
  }
  return steps;
}

void updateLocalVariablesFromPersistentStorageValues(){
  if (persist_exists(PersistKeyFetchedExperimentConditionValue)) {
    // Read persisted value
    int read_experiment_condition = persist_read_int(PersistKeyFetchedExperimentConditionValue);
    experimentConditionThatUserIsIn = read_experiment_condition;
    if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Experiment Condition Read from Persistent Storage: %d", read_experiment_condition);
  } else {
    if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "No Experiment Condition Found In Persistent Storage");
  }

  if (persist_exists(PersistKeyFetchedStepGoalValue)) {
    // Read persisted value
    int read_step_goal = persist_read_int(PersistKeyFetchedStepGoalValue);
    usersStepGoal = read_step_goal;
    if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Step Goal Read from Persistent Storage: %d", read_step_goal);
  } else {
    if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "No Step Goal Found In Persistent Storage");
  }
}

int data_get_hourOfDay(){
  return (difftime(time(NULL), time_start_of_today())) / SECONDS_PER_HOUR;
}

bool data_quiet_hours_occurring_now(){
  if((data_get_hourOfDay() < QUIET_HOURS_END) || (data_get_hourOfDay() >= QUIET_HOURS_START)){
    return 1;
  } else {
    return 0;
  }
}

bool data_server_moratorium_occurring_now(){
  if((data_get_hourOfDay() < (QUIET_HOURS_END - 1)) || (data_get_hourOfDay() >= QUIET_HOURS_START)){
    return 1;
  } else {
    return 0;
  }
}