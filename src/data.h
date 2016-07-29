
#pragma once

#include "config.h"

typedef enum {
  PersistKeyFetchedExperimentConditionValue = 0,
  PersistKeyFetchedStepGoalValue = 1,
  PersistKeySnapshotIndexValue = 2,
  PersistKeySnapshotArray = 3
} PersistKey;

int data_get_steps();

void updateLocalVariablesFromPersistentStorageValues();

int data_get_hourOfDay();

bool data_quiet_hours_occurring_now();

bool data_server_moratorium_occurring_now();