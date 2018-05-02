// Module includes
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Import custom modules
#include "display.h"
#include "corner.h"
#include "uart.h"

//------------------------------------------------------------------------------
// GLOBAL CONSTANT DEFINITIONS                                                 |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// STATIC VARIABLE DEFINITIONS                                                 |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS                                                     |
//------------------------------------------------------------------------------
static void intHandler(int sig);

//------------------------------------------------------------------------------
// GLOBAL INTERFACE FUNCTIONS                                                  |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
int main() {

  // Create signal handlers
  signal(SIGINT, intHandler);

  // Perform initialization
  display_init();
  corner_init();
  uart_init();

  // Execute console
  while(true) {

    //uart_result_t retv = UART_SUCCESS;

    /*
    uart_rxPacket_t response;
    memset(&response, 0, sizeof(response));
    retv = uart_read(&response);
    switch(retv) {
      case UART_FAILURE:
        break;
      case UART_TIMEOUT:
        break;
      case UART_SUCCESS:
        updateCornerData(response);
        break;
      default:
        break;
    }
    */

    location_field_t bicycleOrientation = corner_approximatePosition();
    display_update(bicycleOrientation);

    /*
    // Intialize packet to be transmitted
    uart_txPacket_t command;
    memset(&command, 0, sizeof(command));

    // Assign packet to be transmitted
    command.packetHeader = 0x7A;
    command.destinationAddress.vehicleType = BICYCLE;
    command.destinationAddress.vehicleAddress = 0x42;
    command.bicycleRelativeToVehicleOrientation = bicycleOrientation;

    // Transmit packet
    retv = uart_write(&command);
    if(retv != UART_SUCCESS) {
      printf("Error: uart could not write correctly.\n");
    }
    */
  }

  return 0;
}

//------------------------------------------------------------------------------
// SIGNAL HANDLERS                                                             |
//------------------------------------------------------------------------------
void intHandler(int sig) {

  // Ignore the signal
  signal(sig, SIG_IGN);

  // Perform system cleanup
  // display_cleanup();
  uart_cleanup();

  // Quit the program
  raise(SIGKILL);
}

