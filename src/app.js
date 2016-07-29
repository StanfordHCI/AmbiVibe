var VERSION = "1.1";
var DEBUG = true;
var NODE_SERVER_URL = 'ambivibe-511ab.firebaseio.com';

//var lastData = [];
//var numItems = 0;

/********************************** Helpers ***********************************/

function info(content) {
  console.log(content);
}

function debug(content) {
  if(DEBUG) info(content);
}

function get(url, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () { callback(this.responseText); };
  xhr.open('GET', url);
  xhr.send();
}

function post(url, json, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () { callback(this.responseText); };
  //xhr.open('POST', url); // This doesn't delete two entries at the same minute, but it does create an intermediate folder with a garbled identifier in Firebase
  xhr.open('PUT', url); // This doesn't create an intermediate folder with a garbled identifier in Firebase
  xhr.send(json);
};

function postWithDups(url, json, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () { callback(this.responseText); };
  xhr.open('POST', url); // This doesn't delete two entries at the same minute, but it does create an intermediate folder with a garbled identifier in Firebase
  xhr.send(json);
};

/******************************** PebbleKit Functions ********************************/


function fetchCurrentUserSettings(){
  var userID = Pebble.getAccountToken();
  var watchID = Pebble.getWatchToken();
  
  // Fetch Experiment Condition From Server
  var url = 'https://' + NODE_SERVER_URL + '/' + 'UserSettings' + '/' + 'WatchUniqueID(' + watchID + ')' +  '/' + 'WhichExperimentConditionIsUserIn' + '.json' + '?print=pretty';
  get(url, function(responseText) {
    console.log('Server response to GET request: ' + responseText);
    var returned_usersExperimentCondition = parseInt(responseText);
    Pebble.sendAppMessage({'AppKeyReturnedExperimentConditionValue': returned_usersExperimentCondition});
  }); 
  
  // Fetch Step Goal From Server
  url = 'https://' + NODE_SERVER_URL + '/' + 'UserSettings' + '/' + 'WatchUniqueID(' + watchID + ')' +  '/' + 'UsersStepGoal' + '.json' + '?print=pretty';
  get(url, function(responseText) {
    console.log('Server response to GET request: ' + responseText);
    var returned_usersStepGoal = parseInt(responseText);
    Pebble.sendAppMessage({'AppKeyReturnedStepGoalValue': returned_usersStepGoal});
  }); 
}



/******************************** PebbleKit JS ********************************/



Pebble.addEventListener('ready', function() {
  info('PebbleKit JS ready! Version ' + VERSION);
  fetchCurrentUserSettings();
  Pebble.sendAppMessage({'AppKeyJSReady': 0});
});

Pebble.addEventListener('appmessage', function(dict) {
  debug('Got appmessage: ' + JSON.stringify(dict.payload));

  // Post Step Data
  var steps = dict.payload['AppKeySteps'];
  var timestamp = dict.payload['AppKeyTimestamp'];
  var experimentCondition = dict.payload['AppKeyExperimentConditionSentForServer'];
  var causeOfSnapshot = dict.payload['AppKeyCauseOfSnapshot']; 
  var mostRecentSnapshot = dict.payload['AppKeyMostRecentSnapshot']; 
  var usersStepGoal = dict.payload['AppKeyUsersStepGoal'];
  
  //if(steps != undefined && timestamp != undefined && experimentCondition != undefined) {
  //if(steps != undefined && timestamp != undefined) {

    var userID = Pebble.getAccountToken();
    var watchID = Pebble.getWatchToken();
    var d = new Date(timestamp * 1000);
    var date = (d.getMonth() + 1) + '-' + d.getDate() + '-' + d.getFullYear();
    var timeOfDay = '';
    (d.getHours() < 10) ? timeOfDay += '0' + d.getHours() : timeOfDay += d.getHours();
    //var timeOfDay = d.getHours();
    (d.getMinutes() < 10) ? timeOfDay += ':0' + d.getMinutes() : timeOfDay += ':' + d.getMinutes();
    (d.getSeconds() < 10) ? timeOfDay += ':0' + d.getSeconds() : timeOfDay += ':' + d.getSeconds();
    
    var data = {};
    data['NumberOfStepsCompleted'] = steps;
    data['ExperimentConditionThatUserIsIn'] = experimentCondition;
    data['UsersStepGoal'] = usersStepGoal;
    
    var json = JSON.stringify(data);
    
    var url = 'https://' + NODE_SERVER_URL + '/' + 'AppOpened' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '/' + 'TimeOfDay(' + timeOfDay + ')' + '.json';
    if(causeOfSnapshot == 1){
      url = 'https://' + NODE_SERVER_URL + '/' + 'SideButtonPressed' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '/' + 'TimeOfDay(' + timeOfDay + ')' + '.json';
    } else if(causeOfSnapshot == 2){ 
      url = 'https://' + NODE_SERVER_URL + '/' + 'VibrationForTenPercentMilestoneAchieved' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '/' + 'TimeOfDay(' + timeOfDay + ')' + '.json';
    } else if(causeOfSnapshot == 3){ 
      url = 'https://' + NODE_SERVER_URL + '/' + 'RandomVibrationThroughoutDay' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '/' + 'TimeOfDay(' + timeOfDay + ')' + '.json';
    } else if(causeOfSnapshot == 4){
      url = 'https://' + NODE_SERVER_URL + '/' + 'AutomaticCheckinsThroughoutDay' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '/' + 'TimeOfDay(' + timeOfDay + ')' + '.json';
    } else if(causeOfSnapshot == 5){
      fetchCurrentUserSettings();
      return;
    } else if(causeOfSnapshot == 6){
      url = 'https://' + NODE_SERVER_URL + '/' + 'LandayRevisedCombinationConditionRandomVibrations' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '/' + 'TimeOfDay(' + timeOfDay + ')' + '.json';
    }
    info('Sending json: ' + json);
    post(url, json, function(responseText) {
        console.log('Server response: ' + responseText);
      });
    
    if(mostRecentSnapshot == 1){
      // Update the EndOfDay variable
      data['Timestamp'] = timeOfDay;
      var jsonWithTimestamp = JSON.stringify(data);
      info('Sending json: ' + jsonWithTimestamp);
      var url = 'https://' + NODE_SERVER_URL + '/' + 'EndOfDayResults' + '/' + 'WatchUniqueID(' + watchID + ')' + '/' + 'Date(' + date + ')' + '.json';
      post(url, jsonWithTimestamp, function(responseText) {
        console.log('Most Recent Snapshot/End of Day Server response: ' + responseText);
      });
      
      // Update MostRecentCallHome variable
      var dataWithoutSteps = {};
      dataWithoutSteps['1_UnixTime'] = timestamp;
      dataWithoutSteps['2_AmbivibeUniqueSoftwareID'] = watchID;
      dataWithoutSteps['3_TimeOfDay'] = timeOfDay;
      dataWithoutSteps['4_Date'] = date;
      dataWithoutSteps['5_ExperimentCondition'] = experimentCondition;
      dataWithoutSteps['6_StepCount'] = steps;
      var jsonWithDate = JSON.stringify(dataWithoutSteps);
      info('Sending json: ' + jsonWithDate);
      var url = 'https://' + NODE_SERVER_URL + '/' + 'MostRecentCallHome' + '/' + 'WatchUniqueID(' + watchID + ')' + '.json';
      post(url, jsonWithDate, function(responseText) {
        console.log('Most Recent Call Home: ' + responseText);
      });
    }
    
    // CSV Friendly Mega Table
    url = 'https://' + NODE_SERVER_URL + '/' + 'CSVFriendlyMegaTable' + '.json';
    var dataForMegaTable = {};
    dataForMegaTable['NumberOfStepsCompleted'] = steps;
    dataForMegaTable['ExperimentConditionThatUserIsIn'] = experimentCondition;
    dataForMegaTable['Timestamp'] = timeOfDay;
    dataForMegaTable['Date'] = date;
    dataForMegaTable['WatchID'] = watchID;
    dataForMegaTable['UsersStepGoal'] = usersStepGoal;
    
    if(causeOfSnapshot == 1){
      dataForMegaTable['CauseOfSnapshot'] = 'SideButtonPressed';
    } else if(causeOfSnapshot == 2){ 
      dataForMegaTable['CauseOfSnapshot'] = 'VibrationForTenPercentMilestoneAchieved';
    } else if(causeOfSnapshot == 3){ 
      dataForMegaTable['CauseOfSnapshot'] = 'OriginalCombinationConditionRandomVibrationThroughoutDay';
    } else if(causeOfSnapshot == 4){
      dataForMegaTable['CauseOfSnapshot'] = 'AutomaticCheckinsThroughoutDay';
    } else if(causeOfSnapshot == 5){
      fetchCurrentUserSettings();
      return;
    } else if(causeOfSnapshot == 6){
      dataForMegaTable['CauseOfSnapshot'] = 'LandayRevisedCombinationConditionVibrations';
    } else {
      dataForMegaTable['CauseOfSnapshot'] = '_';
    }
    var jsonForMegaTable = JSON.stringify(dataForMegaTable);
    info('Sending json: ' + jsonForMegaTable);
    postWithDups(url, jsonForMegaTable, function(responseText) {
        console.log('Server response: ' + responseText);
      });
    
  //}
  
  // Update user settings each time that we log data, in case it has changed
  fetchCurrentUserSettings();
});