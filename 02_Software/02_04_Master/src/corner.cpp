
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
//static pthread_t receiveRxPacketsThreadId;
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
void updateCornerData(uart_rxPacket_t response) {
//static void updateCornerData(uart_rxPacket_t response) {

  // LOCK THE CORNER DATA STRUCT
//  pthread_mutex_lock(&cornerDataLock);

  corner_node_t *currentCornerNode = &sCorners[response.sourceAddress.cornerAddress];
  currentCornerNode->rssiTimestamp = time(NULL);
  currentCornerNode->rssi = response.rssi;

  // UNLOCK THE CORNER DATA STRUCT
//  pthread_mutex_unlock(&cornerDataLock);

  printf("Received packet:\n");
  printf("corner addr: %d\n", response.sourceAddress.cornerAddress);
  printf("timestamp addr: %ld\n", currentCornerNode->rssiTimestamp);
  printf("rssi: %d\n", currentCornerNode->rssi);
}

/*
//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static void *corner_receiveRxPackets(void*) {

  uart_result_t retv = UART_SUCCESS;

  uint32_t i = 0;
  while(true) {
    // Receive packet
    printf("%d\n", i);
    ++i;
    uart_rxPacket_t response;
    memset(&response, 0, sizeof(response));
    retv = uart_read(&response);
    if(retv == UART_FAILURE) {
      printf("Error: uart could not read correctly.\n");
    }
    else {
      if(retv == UART_SUCCESS) {
        updateCornerData(response);
      }
    }
  }

}
*/

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
//  int32_t retv = pthread_mutex_init(&cornerDataLock, NULL);
//  retv = pthread_create(&receiveRxPacketsThreadId, NULL, corner_receiveRxPackets, NULL);

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
  uint8_t rl = sCorners[REAR_LEFT].rssi;
  uint8_t rr = sCorners[REAR_RIGHT].rssi;
  uint8_t fl = sCorners[FRONT_LEFT].rssi;
  uint8_t fr = sCorners[FRONT_RIGHT].rssi;

  // UNLOCK THE CORNER DATA STRUCT
//  pthread_mutex_unlock(&cornerDataLock);

  uint8_t frontAvg = (fl + fr) / 2;
  uint8_t rearAvg = (rl + rr) / 2;
  uint8_t leftAvg = (rl + fl) / 2;
  uint8_t rightAvg = (rr + fr) / 2;

  int16_t frontBackDiff = frontAvg - rearAvg;
  int16_t leftRightDiff = leftAvg - rightAvg;
  
  location_field_t bicycleOrientation = NORTH;

  // Check if this is a case where it's in a corner position
  if( ((0.5*abs(frontBackDiff) < abs(leftRightDiff)) && (abs(leftRightDiff) < 1.5*abs(frontBackDiff))) ||
      ((0.5*abs(leftRightDiff) < abs(frontBackDiff)) && (abs(frontBackDiff) < 1.5*abs(leftRightDiff))) ) {
    if( (frontBackDiff > 0) && (leftRightDiff > 0) ) {
      bicycleOrientation = SOUTHEAST;
    }
    else if( (frontBackDiff > 0) && (leftRightDiff < 0) ) {
      bicycleOrientation = SOUTHWEST;
    }
    else if( (frontBackDiff < 0) && (leftRightDiff > 0) ) {
      bicycleOrientation = NORTHEAST;
    }
    else {
      bicycleOrientation = NORTHWEST;
    }
  }
  else {
    if(abs(frontBackDiff) > abs(leftRightDiff)) {
      if(frontBackDiff > 0) {
        bicycleOrientation = SOUTH;
      }
      else {
        bicycleOrientation = NORTH;
      }
    }
    else {
      if(leftRightDiff > 0) {
        bicycleOrientation = EAST;
      }
      else {
        bicycleOrientation = WEST;
      }
    }
  }

  return bicycleOrientation;
}
