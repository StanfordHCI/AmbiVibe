#include <pebble.h>
#include "comm.h"

typedef struct Snapshot {
  time_t timestamp;
  unsigned short steps;
  //int experimentCondition;
  unsigned short causeOfSnapshot;
} Snapshot;

int MAX_NUMBER_OF_SNAPSHOTS = 192;
int MAX_SNAPSHOT_INDEX = 191;
Snapshot snapshots[192];
int snapshotIndex = -1;
int mostRecentSnapshot = 1;

static void message() {
  if(snapshotIndex < 0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Tried to send message with an empty queue");
    return;
  }

  if(!connection_service_peek_pebble_app_connection()) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Bluetooth connection unavailable.");
    return;
  }

  DictionaryIterator *out;
  if(app_message_outbox_begin(&out) == APP_MSG_OK) {
    Snapshot snapshot = snapshots[snapshotIndex];
    time_t timestamp = snapshot.timestamp;
    int steps = snapshot.steps;
    //int experimentCondition = snapshot.experimentCondition;
    int causeOfSnapshot = (int)snapshot.causeOfSnapshot;
    APP_LOG(APP_LOG_LEVEL_INFO, "Cause of snapshot: %d", causeOfSnapshot);
    
    dict_write_int(out, AppKeyMostRecentSnapshot, &mostRecentSnapshot, sizeof(int), true);
    if(mostRecentSnapshot == 1){
      mostRecentSnapshot = 0;
    }
    
    dict_write_int(out, AppKeyTimestamp, &timestamp, sizeof(int), true);
    dict_write_int(out, AppKeySteps, &steps, sizeof(int), true);
    dict_write_int(out, AppKeyCauseOfSnapshot, &causeOfSnapshot, sizeof(int), true);
    dict_write_int(out, AppKeyExperimentConditionSentForServer, &experimentConditionThatUserIsIn, sizeof(int), true);
    dict_write_int(out, AppKeyUsersStepGoal, &usersStepGoal, sizeof(int), true);
    
    if(app_message_outbox_send() != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending message");
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error beginning message");
  }
}

static void saveToPersistentStorage(){
  // Save snapshotArray and index to persistent storage
  int snapshotIndexToBeWritten = 30;
  
  if(snapshotIndex < 30){
     snapshotIndexToBeWritten = snapshotIndex;
  }
  
  persist_write_int(PersistKeySnapshotIndexValue, snapshotIndexToBeWritten);
  //persist_write_int(PersistKeySnapshotIndexValue, snapshotIndex);
  
  persist_write_data(PersistKeySnapshotArray, snapshots, ((snapshotIndexToBeWritten + 1) * ((sizeof(Snapshot)))));
  //persist_write_data(PersistKeySnapshotArray, snapshots, ((snapshotIndex + 1) * ((sizeof(Snapshot)))));
}

static void queue_message(time_t timestamp, int steps, int experimentCondition, unsigned short causeOfSnapshot) {
  
  snapshotIndex++;
  
  // Check Array Bounds
  if(snapshotIndex > MAX_SNAPSHOT_INDEX){
    snapshotIndex = MAX_SNAPSHOT_INDEX;
    message();
    return;
  }
  
  // Snapshot s = {timestamp, steps, experimentCondition, sideButtonPressed};
  Snapshot s = {timestamp, steps, causeOfSnapshot};
  
  snapshots[snapshotIndex] = s;
  
  // Null Terminate the snapshots array
  //snapshots[snapshotIndex + 1] = '\0';
  memset(&(snapshots[snapshotIndex + 1]), 0x00, sizeof(Snapshot));
  
  saveToPersistentStorage();
  
  message();
  APP_LOG(APP_LOG_LEVEL_ERROR, "Snapshot Index: %d (Queue)", snapshotIndex);
  
}

void comm_send_data(unsigned short causeOfSnapshot) {
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Comm_Send_Steps called");
  time_t now = time(NULL);
  int steps = data_get_steps();
  
  if(!data_server_moratorium_occurring_now()){
    queue_message(now, steps, experimentConditionThatUserIsIn, causeOfSnapshot);
  }
}

static void outbox_sent_handler(DictionaryIterator *iter, void *context) {
  // Last message was successful
  snapshotIndex--;
  // Null Terminate the snapshots array
  memset(&(snapshots[snapshotIndex + 1]), 0x00, sizeof(Snapshot));
  saveToPersistentStorage();
  
  APP_LOG(APP_LOG_LEVEL_ERROR, "Snapshot Index: %d (Sent handler)", snapshotIndex);
  if(snapshotIndex > -1) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Snapshot Index > 1");
    message();
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Upload complete!");
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Inbox Received Handler Reached");
  
  Tuple *js_ready_t = dict_find(iter, AppKeyJSReady);
  if(js_ready_t) {
    comm_send_data(0);
  }
  
  // Receive Fetched Experiment Condition
  Tuple *experimentCondition_returned_tuple = dict_find(iter, AppKeyReturnedExperimentConditionValue);
  if(experimentCondition_returned_tuple){
    if(experimentCondition_returned_tuple->value->int32){
      if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Experiment Condition found in tuple: %d", (int)(experimentCondition_returned_tuple->value->int32));
      experimentConditionThatUserIsIn = experimentCondition_returned_tuple->value->int32;
      persist_write_int(PersistKeyFetchedExperimentConditionValue, (int)(experimentCondition_returned_tuple->value->int32));
      if(experimentConditionThatUserIsIn == 2){
        main_window_hide_steps();
      }
    }
  }
  
  // Receive Fetched Step Goal
  Tuple *stepGoal_returned_tuple = dict_find(iter, AppKeyReturnedStepGoalValue);
  if(stepGoal_returned_tuple){
    if(stepGoal_returned_tuple->value->int32){
      if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Step Goal found in tuple: %d", (int)(stepGoal_returned_tuple->value->int32));
      usersStepGoal = stepGoal_returned_tuple->value->int32;
      persist_write_int(PersistKeyFetchedStepGoalValue, (int)(stepGoal_returned_tuple->value->int32));
    }
  }
}

void comm_init(int inbox, int outbox) {
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_outbox_sent(outbox_sent_handler);
  app_message_open(inbox, outbox);
  
  // Nullily Snapshots to start with
  memset(snapshots, 0x00, sizeof(snapshots));
  
  if (persist_exists(PersistKeySnapshotIndexValue)) {
    snapshotIndex = persist_read_int(PersistKeySnapshotIndexValue);
    if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Persistent Snapshot index found: %d", snapshotIndex);
  }
  
  if (persist_exists(PersistKeySnapshotArray)) {
    persist_read_data(PersistKeySnapshotArray, snapshots, (persist_get_size(PersistKeySnapshotArray)));
    if (DEBUG) APP_LOG(APP_LOG_LEVEL_INFO, "Persistent Snapshot Array found");
    // Error Checking
    if((unsigned int)(persist_get_size(PersistKeySnapshotArray)) != ((snapshotIndex + 1) * (sizeof(Snapshot)))){
      if (DEBUG) APP_LOG(APP_LOG_LEVEL_ERROR, "Persistent Snapshot Array Size does not match Persistent Snapshot Index");
    } 
  }
}