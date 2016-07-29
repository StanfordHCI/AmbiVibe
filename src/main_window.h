#pragma once

#include "comm.h"
#include "config.h"
#include "data.h"
#include "vibration.h"

extern int experimentConditionThatUserIsIn;
extern int usersStepGoal;
extern int mostRecentSnapshot;

void main_window_push();

void main_window_update_time(struct tm *tick_time);

void main_window_deinit();

void main_window_show_steps(int steps);

void main_window_show_only_percentage();

void main_window_hide_steps();

void main_window_show_low_battery();

void main_window_hide_low_battery();