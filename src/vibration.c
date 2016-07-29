#include <pebble.h>
#include "vibration.h"

#define PRESIGNAL_VIBRATION_DURATION 700
#define PRESIGNAL_PAUSE_DURATION 400
#define FIVE_VIBRATION_DURATION 600
#define ONE_VIBRATION_DURATION 150
#define PAUSE_BETWEEN_VIBRATIONS_DURATION 200
#define PAUSE_BETWEEN_LAB_LOOPS_DURATION 2000

void vibrateNTimes(int n){
  
  //Initialize variables
  uint32_t segments[100];
  memset(segments, 0x00, sizeof(segments));
  int currIndex = 0;

  //Presignal, sent before remaining vibrations
  segments[currIndex] = PRESIGNAL_VIBRATION_DURATION;
  currIndex++;
  segments[currIndex] = PRESIGNAL_PAUSE_DURATION;
  currIndex++;

  // Jessica wants us to use 100% as our maximum
  if(n > 10) n = 10;

  // If we were to convert 'n' to a roman numeral that uses only V's and I's
  int numFives = n / 5;
  int numOnes = n % 5;

  for(int i = 0; i < numFives; i++){
    segments[currIndex] = FIVE_VIBRATION_DURATION;
    currIndex++;
    segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
    currIndex++;
  }

  for(int i = 0; i < numOnes; i++){
    segments[currIndex] = ONE_VIBRATION_DURATION;
    currIndex++;
    segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
    currIndex++;
  }

  VibePattern pat = {
    .durations = segments,
    .num_segments = currIndex,
  };
  vibes_enqueue_custom_pattern(pat); 
}

void vibrateNLongTimes(int n){
  //Initialize variables
  uint32_t segments[100];
  memset(segments, 0x00, sizeof(segments));
  int currIndex = 0;
    
  for(int i = 0; i < n; i++){
    segments[currIndex] = PRESIGNAL_VIBRATION_DURATION;
    currIndex++;
    segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
    currIndex++;
  }
  
  VibePattern pat = {
    .durations = segments,
    .num_segments = currIndex,
  };
  vibes_enqueue_custom_pattern(pat);  
}

void labTestVibrateOrdered(){
  //Initialize variables
  uint32_t segments[200];
  memset(segments, 0x00, sizeof(segments));
  int currIndex = 0;
  
  int hardcodedRandomVibrationsArray[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, '\0'};
  int HARDCODED_ARRAY_LENGTH = 20;
  
  for(int i = 0; i < HARDCODED_ARRAY_LENGTH; i++){
    int n = hardcodedRandomVibrationsArray[i];
    
    // If we were to convert 'n' to a roman numeral that uses only V's and I's
    int numFives = n / 5;
    int numOnes = n % 5;

    for(int i = 0; i < numFives; i++){
      segments[currIndex] = FIVE_VIBRATION_DURATION;
      currIndex++;
      segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
      currIndex++;
    }

    for(int i = 0; i < numOnes; i++){
      segments[currIndex] = ONE_VIBRATION_DURATION;
      currIndex++;
      segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
      currIndex++;
    }
    
    segments[currIndex - 1] = PAUSE_BETWEEN_LAB_LOOPS_DURATION;
    
  }
  
  VibePattern pat = {
    .durations = segments,
    .num_segments = currIndex,
  };
  vibes_enqueue_custom_pattern(pat); 
  
  /*
  //Initialize variables
  uint32_t segments[1000];
  int currIndex = 0;
  
  //for(int i = 0; i < n; i++){
    //for(int j = 1; j < 11; j++){
        int j = 1;
        // If we were to convert 'n' to a roman numeral that uses only V's and I's
        int numFives = j / 5;
        int numOnes = j % 5;
      
        
        for(int k = 0; k < numFives; k++){
          segments[currIndex] = FIVE_VIBRATION_DURATION;
          currIndex++;
          segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
          currIndex++;
        }
      
        for(int m = 0; m < numOnes; m++){
          segments[currIndex] = ONE_VIBRATION_DURATION;
          currIndex++;
          segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
          currIndex++;
        }
      
        // Encode a long pause between loops
        //segments[currIndex - 1] = PAUSE_BETWEEN_LAB_LOOPS_DURATION;
    //}
  //}
  
   VibePattern pat = {
    .durations = segments,
    .num_segments = currIndex,
  };
  vibes_enqueue_custom_pattern(pat); 
  */
  
}

void labTestVibrateRandomOrder(){
  //Initialize variables
  uint32_t segments[200];
  memset(segments, 0x00, sizeof(segments));
  int currIndex = 0;
  
  //int hardcodedRandomVibrationsArray[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  //int HARDCODED_ARRAY_LENGTH = 10;
  int hardcodedRandomVibrationsArray[] = {2, 7, 5, 1, 10, 10, 2, 5, 3, 4, 5, 1, 9, 4, 1, 10, 7, 9, 10, 5, 4, 3, 3, 9, 4, 7, 10, 2, 10, 8, '\0'};
  int HARDCODED_ARRAY_LENGTH = 30;
  
  for(int i = 0; i < HARDCODED_ARRAY_LENGTH; i++){
    int n = hardcodedRandomVibrationsArray[i];
    
    // If we were to convert 'n' to a roman numeral that uses only V's and I's
    int numFives = n / 5;
    int numOnes = n % 5;

    for(int i = 0; i < numFives; i++){
      segments[currIndex] = FIVE_VIBRATION_DURATION;
      currIndex++;
      segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
      currIndex++;
    }

    for(int i = 0; i < numOnes; i++){
      segments[currIndex] = ONE_VIBRATION_DURATION;
      currIndex++;
      segments[currIndex] = PAUSE_BETWEEN_VIBRATIONS_DURATION;
      currIndex++;
    }
    
    segments[currIndex - 1] = PAUSE_BETWEEN_LAB_LOOPS_DURATION;
    
  }
  
  VibePattern pat = {
    .durations = segments,
    .num_segments = currIndex,
  };
  vibes_enqueue_custom_pattern(pat); 
}