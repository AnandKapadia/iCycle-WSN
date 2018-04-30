// Module includes
#include <signal.h>
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

    uart_result_t retv = UART_SUCCESS;

    location_field_t bicycleOrientation = corner_approximatePosition();
    display_update(bicycleOrientation);

    // Intialize packet to be transmitted
    uart_txPacket_t command;
    memset(&command, 0, sizeof(command));

    // Assign packet to be transmitted
    command.packetHeader = 0x7A;
    command.sourceAddress.vehicleType = AUTOMOBILE;
    command.sourceAddress.vehicleAddress = 0;
    command.sourceAddress.cornerAddress = MASTER;
    command.destinationAddress.vehicleType = AUTOMOBILE;
    command.destinationAddress.vehicleAddress = 0;
    command.packetLength = 1;
    command.data[0] = bicycleOrientation;

    // Transmit packet
    retv = uart_write(&command);
    if(retv != UART_SUCCESS) {
      printf("Error: uart could not write correctly.\n");
    }
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
