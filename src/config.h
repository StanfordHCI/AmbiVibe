#include <pebble.h>

#define DEBUG true

#define VISUAL_ONLY_CONDITION 1
#define VIBRATE_ONLY_CONDITION 2
#define COMBINATION_CONDITION 3
#define UNASSIGNED_CONDITION 4
#define LAB_TEST_USERS_VIBRATION_AWARENESS_CONDITION 5
#define LANDAY_REVISED_COMBINATION_CONDITION 6

#define CAUSE_OF_SNAPSHOT_WAS_SIDE_BUTTON_PRESSED 1
#define CAUSE_OF_SNAPSHOT_WAS_VIBRATION_FOR_MILESTONE 2
#define CAUSE_OF_SNAPSHOT_WAS_RANDOM_VIBRATION 3
#define CAUSE_OF_SNAPSHOT_WAS_INTERVAL 4
#define CAUSE_OF_SNAPSHOT_WAS_CALLING_HOME_TO_BE_ASSIGNED_EXPERIMENTAL_CONDITION 5
#define CAUSE_OF_SNAPSHOT_WAS_LANDAY_REVISED_RANDOM_VIBRATION 6

#define QUIET_HOURS_START 24
#define QUIET_HOURS_END 7

#define VIBRATE_ONLY_LOGGING_DATA_INTERVAL 15