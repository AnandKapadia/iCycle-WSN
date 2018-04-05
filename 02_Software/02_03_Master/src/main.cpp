#include <stdlib.h>

// Import custom modules
#include "display.h"
#include "xbee16A.h"

#define XBEE_SUCCESS  0
#define XBEE_ERROR    0xff
#define XBEE_RX_EMPTY 1

#define NUM_CORNERS 4
#define MASTER_ADDRESS 0
#define REAR_LEFT_ADDRESS 1
#define REAR_RIGHT_ADDRESS 2
#define FRONT_LEFT_ADDRESS 3
#define FRONT_RIGHT_ADDRESS 4

// TODO Expand to master sotring a set of corner data for each in a list of bikes

// TODO add timestamp for checking data freshness
typedef struct cornerNode_t {
  uint8_t addr;
  uint8_t rssi;
} cornerNode_t;

cornerNode_t corners[NUM_CORNERS];

bool storeCornerData(uint8_t addr, uint8_t data) {

  for(uint8_t i = 0; i < NUM_CORNERS; i++) {
    if(corners[i].addr == addr) {
      corners[i].rssi = data;
      return true;
    }
  }
  return false;
}

uint8_t getCornerData(uint8_t addr) {
  for(uint8_t i = 0; i < NUM_CORNERS; i++) {
    if(corners[i].addr == addr) {
      return corners[i].rssi;
    }
  }
  //Something went wrong
  return 0;
}

int main() {

  xbee16A_result_t retv = XBEE16A_SUCCESS;

  // Perform initialization
  display_init();
  xbee16A_init();

  corners[0].addr = REAR_LEFT_ADDRESS;
  corners[1].addr = REAR_RIGHT_ADDRESS;
  corners[2].addr = FRONT_LEFT_ADDRESS;
  corners[3].addr = FRONT_RIGHT_ADDRESS;

  // Execute console
  int i = 0;
  while(i < 100) {

    // Receive packet
    xbee16A_rxPacket_t response;
    memset(&response, 0, sizeof(response));
    retv = xbee16A_read(&response);
    if(retv != XBEE16A_SUCCESS) {
      printf("Error: xbee could not read correctly.\n");
    }

    //--------------------------------------------

    uint8_t cornerRssi;

    // TODO: Reexamine this later....
    cornerRssi = response.frameData.commandData.data[2];

    // Invert RSSI as it's reported as negative decibels
    bool rv = storeCornerData(response.frameData.commandData.sourceAddress, cornerRssi);
    if(rv == false) { printf("DOH!\n"); }

    // Check all the coerner data
    uint8_t rl = getCornerData(REAR_LEFT_ADDRESS);
    uint8_t rr = getCornerData(REAR_RIGHT_ADDRESS);
    uint8_t fl = getCornerData(FRONT_LEFT_ADDRESS);
    uint8_t fr = getCornerData(FRONT_RIGHT_ADDRESS);

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
    display_update(bicycleOrientation);

    // Send packet
    xbee16A_txPacket_t command;
    memset(&command, 0, sizeof(command));
    command.startDelimiter = 0x7E;
    command.length = 6;
    command.frameData.apiIdentifier = 0x01;
    command.frameData.commandData.frameId = 0x00;
    command.frameData.commandData.destinationAddress = 0x0042;
    command.frameData.commandData.options = 0x00;
    command.frameData.commandData.data[0] = bicycleOrientation;
    retv = xbee16A_write(&command);
    if(retv != XBEE16A_SUCCESS) {
      printf("Error: xbee could not write correctly.\n");
    }

    //---------------------------------------------

    // Debug prints
    printf("start delimiter: 0x%x\n", response.startDelimiter);
    printf("length: 0x%x\n", response.length);
    printf("api identifier: 0x%x\n", response.frameData.apiIdentifier);
    printf("source address: 0x%x\n", response.frameData.commandData.sourceAddress);
    printf("rssi: %u\n", response.frameData.commandData.rssi);
    printf("options: 0x%x\n", response.frameData.commandData.options);
    printf("payload: ");
    for(uint16_t i = 0; i < (response.length)-5; i++) {
      printf("0x%x ", response.frameData.commandData.data[i]);
    }
    printf("\n");
    printf("rssi rl: %u\n", rl);
    printf("rssi rr: %u\n", rr);
    printf("rssi fl: %u\n", fl);
    printf("rssi fr: %u\n", fr);
    printf("orientation: %d\n", bicycleOrientation);

    ++i;
  }

  // Clean up files
  display_destroy();

  return 0;
}
