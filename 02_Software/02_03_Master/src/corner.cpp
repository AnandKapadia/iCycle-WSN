
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

//------------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS                                                     |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static void updateCornerData(uart_rxPacket_t response) {
  corner_node_t *currentCornerNode = &sCorners[response.sourceAddress.cornerAddress];
  currentCornerNode->rssiTimestamp = response.timestamp;
  currentCornerNode->rssi = response.data[0];
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
      printf("Error: uart could not read correctly.\n");
    }
    else {
      // Check if the incoming packet is destined for us
      if( (response.destinationAddress.vehicleType    == AUTOMOBILE) &&
          (response.destinationAddress.vehicleAddress == MY_VEHICLE_ADDRESS) &&
          (response.destinationAddress.cornerAddress  == MASTER) ) {
        updateCornerData(response);
      }
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

  int32_t retv = pthread_create(&receiveRxPacketsThreadId, NULL, corner_receiveRxPackets, NULL);

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
uint8_t getCornerData(uint8_t addr) {
  for(uint8_t i = 0; i < NUM_CORNERS; i++) {
    if(sCorners[i].cornerAddress == addr) {
      return sCorners[i].rssi;
    }
  }
  //Something went wrong
  return 0;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
location_field_t corner_approximatePosition() {
  // Check all the coerner data
  uint8_t rl = getCornerData(REAR_LEFT);
  uint8_t rr = getCornerData(REAR_RIGHT);
  uint8_t fl = getCornerData(FRONT_LEFT);
  uint8_t fr = getCornerData(FRONT_RIGHT);

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
