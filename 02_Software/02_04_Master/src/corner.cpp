
// Include modules
#include <pthread.h>
#include <stdlib.h>

// Include custom modules
#include "corner.h"
#include "params.h"
#include "uart.h"

//------------------------------------------------------------------------------
// GLOBAL CONSTANT DEFINITIONS                                                 |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// STATIC VARIABLE DEFINITIONS                                                 |
//------------------------------------------------------------------------------
static corner_node_t sCorners[NUM_CORNERS];
static pthread_t receiveRxPacketsThreadId;
//static pthread_mutex_t cornerDataLock;

//------------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS                                                     |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
int8_t getAverageRssi(corner_node_t *cornerNode) {

  int32_t sum = 0;
  int32_t numValidRssi = 0;
  time_t now = time(NULL);

  for(uint8_t i = 0; i < NUM_RSSI_VALUES; ++i) {
    if( (now - cornerNode->rssiData[i].rssiTimestamp) < FRESHNESS_TIME_DIFF ) {
      sum += cornerNode->rssiData[i].rssi;
      ++numValidRssi;
    }
  }

  int32_t averageRssi = 0;

  if(numValidRssi > 0) {
    averageRssi = sum / numValidRssi;
  }
  else {
    averageRssi = -100;
  }

  return averageRssi;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
uint8_t getMinRssiTimestampIndex(corner_node_t *cornerNode) {

  uint8_t minIndex = 0;
  time_t minTimestamp = cornerNode->rssiData[0].rssiTimestamp;

  for(uint8_t i = 1; i < NUM_RSSI_VALUES; ++i) {
    if(cornerNode->rssiData[i].rssiTimestamp < minTimestamp) {
      minIndex = i;
      minTimestamp = cornerNode->rssiData[i].rssiTimestamp;
    }
  }

  return minIndex;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
uint8_t getMaxRssiTimestampIndex(corner_node_t *cornerNode) {

  uint8_t maxIndex = 0;
  time_t maxTimestamp = cornerNode->rssiData[0].rssiTimestamp;

  for(uint8_t i = 1; i < NUM_RSSI_VALUES; ++i) {
    if(cornerNode->rssiData[i].rssiTimestamp > maxTimestamp) {
      maxIndex = i;
      maxTimestamp = cornerNode->rssiData[i].rssiTimestamp;
    }
  }

  return maxIndex;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
void updateCornerData(uart_rxPacket_t response) {
//static void updateCornerData(uart_rxPacket_t response) {

  if(response.sourceAddress.cornerAddress < NUM_CORNERS) {

    // LOCK THE CORNER DATA STRUCT
//    pthread_mutex_lock(&cornerDataLock);

    corner_node_t *currentCornerNode = &sCorners[response.sourceAddress.cornerAddress];
    uint8_t minRssiTimestampIndex = getMinRssiTimestampIndex(currentCornerNode);
    currentCornerNode->rssiData[minRssiTimestampIndex].rssiTimestamp = time(NULL);
    currentCornerNode->rssiData[minRssiTimestampIndex].rssi = response.rssi;

    // UNLOCK THE CORNER DATA STRUCT
//    pthread_mutex_unlock(&cornerDataLock);

    /*
    printf("-------------------------------\n");
    printf("Received packet:\n");
    printf("corner addr: %d\n", response.sourceAddress.cornerAddress);
    printf("timestamp addr: %ld\n", currentCornerNode->rssiData[minRssiTimestampIndex].rssiTimestamp);
    printf("rssi: %d\n", currentCornerNode->rssiData[minRssiTimestampIndex].rssi);
    printf("-------------------------------\n");
    */
  }
  else {
//    printf("Error: Invalid source corner address\n");
  }
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static void *corner_receiveRxPackets(void*) {

  uart_result_t retv = UART_SUCCESS;

  while(true) {
    // Receive packet
    uart_rxPacket_t response;
    memset(&response, 0, sizeof(response));
    retv = uart_read(&response);
    if(retv == UART_FAILURE) {
//      printf("Error: uart could not read correctly.\n");
    }
    else if(retv == UART_SUCCESS) {
      updateCornerData(response);
    }
  }

}

//------------------------------------------------------------------------------
// GLOBAL INTERFACE FUNCTIONS                                                  |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
int32_t corner_init() {

  int32_t retv = 0;

//  retv = pthread_mutex_init(&cornerDataLock, NULL);
  retv = pthread_create(&receiveRxPacketsThreadId, NULL, corner_receiveRxPackets, NULL);

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
location_field_t corner_approximatePosition() {

  // LOCK THE CORNER DATA STRUCT
//  pthread_mutex_lock(&cornerDataLock);

  // Check all the corner data
  int8_t fl = getAverageRssi(&sCorners[FRONT_LEFT]);
  int8_t fr = getAverageRssi(&sCorners[FRONT_RIGHT]);
  int8_t rl = getAverageRssi(&sCorners[REAR_LEFT]);
  int8_t rr = getAverageRssi(&sCorners[REAR_RIGHT]);

  // UNLOCK THE CORNER DATA STRUCT
//  pthread_mutex_unlock(&cornerDataLock);

  time_t flT = sCorners[FRONT_LEFT].rssiData[getMaxRssiTimestampIndex(&sCorners[FRONT_LEFT])].rssiTimestamp;
  time_t frT = sCorners[FRONT_RIGHT].rssiData[getMaxRssiTimestampIndex(&sCorners[FRONT_RIGHT])].rssiTimestamp;
  time_t rlT = sCorners[REAR_LEFT].rssiData[getMaxRssiTimestampIndex(&sCorners[REAR_LEFT])].rssiTimestamp;
  time_t rrT = sCorners[REAR_RIGHT].rssiData[getMaxRssiTimestampIndex(&sCorners[REAR_RIGHT])].rssiTimestamp;
  printf("----------------------\n");
  printf("TIMESTAMP: %ld\n", time(NULL));
  printf("FL (%d): %d (%ld)\n", FRONT_LEFT, fl, flT);
  printf("FR (%d): %d (%ld)\n", FRONT_RIGHT, fr, frT);
  printf("RL (%d): %d (%ld)\n", REAR_LEFT, rl, rlT);
  printf("RR (%d): %d (%ld)\n", REAR_RIGHT, rr, rrT);
  printf("----------------------\n");

  int8_t frontAvg = (fl + fr) / 2;
  int8_t rearAvg = (rl + rr) / 2;
  int8_t leftAvg = (rl + fl) / 2;
  int8_t rightAvg = (rr + fr) / 2;

  int16_t frontBackDiff = frontAvg - rearAvg;
  int16_t leftRightDiff = leftAvg - rightAvg;
  
  location_field_t bicycleOrientation = NORTH;

  // Check if this is a case where it's in a corner position
  if( ((0.2*abs(frontBackDiff) < abs(leftRightDiff)) && (abs(leftRightDiff) < 1.8*abs(frontBackDiff))) ||
      ((0.2*abs(leftRightDiff) < abs(frontBackDiff)) && (abs(frontBackDiff) < 1.8*abs(leftRightDiff))) ) {
    if( (frontBackDiff < 0) && (leftRightDiff < 0) ) {
      bicycleOrientation = SOUTHEAST;
    }
    else if( (frontBackDiff < 0) && (leftRightDiff > 0) ) {
      bicycleOrientation = SOUTHWEST;
    }
    else if( (frontBackDiff > 0) && (leftRightDiff < 0) ) {
      bicycleOrientation = NORTHEAST;
    }
    else {
      bicycleOrientation = NORTHWEST;
    }
  }
  else {
    if(abs(frontBackDiff) > abs(leftRightDiff)) {
      if(frontBackDiff < 0) {
        bicycleOrientation = SOUTH;
      }
      else {
        bicycleOrientation = NORTH;
      }
    }
    else {
      if(leftRightDiff < 0) {
        bicycleOrientation = EAST;
      }
      else {
        bicycleOrientation = WEST;
      }
    }
  }

  return bicycleOrientation;
}
