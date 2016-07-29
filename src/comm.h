#pragma once

#include "config.h"
#include "data.h"
#include "main_window.h"

extern int experimentConditionThatUserIsIn;
extern int usersStepGoal;

typedef enum {
  AppKeySteps,
  AppKeyTimestamp,
  AppKeyJSReady,
  AppKeyReturnedExperimentConditionValue,
  AppKeyReturnedStepGoalValue,
  AppKeyExperimentCondition,
  AppKeyCauseOfSnapshot,
  AppKeyMostRecentSnapshot,
  AppKeyExperimentConditionSentForServer,
  AppKeyUsersStepGoal
} AppKey;

void comm_init(int inbox, int outbox);

void comm_send_data(unsigned short causeOfSnapshot);