// Import modules
#include <termios.h>

// Import custom modules
#include "msg_structs.h"
#include "uart.h"

//------------------------------------------------------------------------------
// GLOBAL CONSTANT DEFINITIONS                                                 |
//------------------------------------------------------------------------------
static const uint16_t MAX_NUM_RX_RETRIES = 100;
static const uint8_t EXPECTED_PACKET_HEADER = 0x7A;
static const uint16_t MAX_UART_BUFFER_LENGTH = 20;

//------------------------------------------------------------------------------
// STATIC VARIABLE DEFINITIONS                                                 |
//------------------------------------------------------------------------------
static int32_t sUsb = -1;

//------------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS                                                     |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static uart_result_t uart_readUsb(int32_t usb, char buf[MAX_UART_BUFFER_LENGTH], uint16_t numChars) {

  uart_result_t retv = UART_SUCCESS;
  uint16_t charsRead = 0;

  // Try to read from the uart
  uint16_t retries = 0;
  do {
    charsRead = read(usb, buf, numChars);
    ++retries;
  } while((charsRead != numChars) && (retries < MAX_NUM_RX_RETRIES));

  // If we fail greater than the number of retries
  if(retries >= MAX_NUM_RX_RETRIES) {
    retv = UART_TIMEOUT;
  }
  // If we read a different number of bytes than we expected
  else if(charsRead != numChars) {
    printf("Error: invalid number of characters read\n");
    retv = UART_FAILURE;
  }

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
static uart_result_t uart_writeBuffer(const char writeBuf[MAX_UART_BUFFER_LENGTH], uint16_t numChars) {

  uart_result_t retv = UART_SUCCESS;

  uint16_t charsWritten = 0;

  if(numChars > MAX_UART_BUFFER_LENGTH) {
    printf("Error: Tried to write too many characters.\n");
    retv = UART_FAILURE;
  }
  else {
    charsWritten = write(sUsb, writeBuf, numChars);
  }

  if(charsWritten < numChars) {
    retv = UART_FAILURE;
  }

  return retv;
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
uart_result_t uart_init() {

  uart_result_t retv = UART_SUCCESS;

  // Open USB port for read/write
  sUsb = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
  if(sUsb < 0) {
    printf("Error: Invalid USB device.\n");
    retv = UART_FAILURE;
  }

  // Error handling
  struct termios tty;
  memset(&tty, 0, sizeof(tty));
  if(tcgetattr(sUsb, &tty) != 0) {
    printf("Error: tcgetattr returned error\n");
    retv = UART_FAILURE;
  }

  // Set baud rate
  cfsetospeed(&tty, (speed_t)B9600);
  cfsetispeed(&tty, (speed_t)B9600);

  // Set port to 8N1
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  tty.c_cflag    &= ~CRTSCTS;       // No flow control
  tty.c_cc[VMIN]  = 1;              // Read doesn't block
  tty.c_cc[VTIME] = 5;              // 0.5 second read timeout
  tty.c_cflag    |= CREAD | CLOCAL; // turn on read and ignore ctrl lines

  // Make raw
  cfmakeraw(&tty);

  // Flush port to apply attributes
  tcflush(sUsb, TCIFLUSH);
  if(tcsetattr(sUsb, TCSANOW, &tty) != 0) {
    printf("Error: tcsetattr returned error\n");
    retv = UART_FAILURE;
  }

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
uart_result_t uart_write(uart_txPacket_t *command) {

  uart_result_t retv = UART_SUCCESS;

  char writeBuf[MAX_UART_BUFFER_LENGTH] = {0};
  uint16_t bufferPosition = 0;

  // Move all values into the write buffer to be sent
  writeBuf[bufferPosition] = command->packetHeader;
  ++bufferPosition;

  writeBuf[bufferPosition] = command->destinationAddress.vehicleType;
  ++bufferPosition;

  writeBuf[bufferPosition] = command->destinationAddress.vehicleAddress;
  ++bufferPosition;

  writeBuf[bufferPosition] = command->destinationAddress.cornerAddress;
  ++bufferPosition;

  writeBuf[bufferPosition] = command->bicycleRelativeToVehicleLocation;
  ++bufferPosition; 

  // Calculate the checksum and put it into the write buffer
  uint8_t checksum = 0;
  for(uint16_t i = 0; i < bufferPosition; ++i) {
    checksum += writeBuf[i];
  }
  writeBuf[bufferPosition] = checksum;
  ++bufferPosition;

  // Write the write buffer out to the uart
  uint16_t bufferLength = bufferPosition;
  retv = uart_writeBuffer(writeBuf, bufferLength);

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
uart_result_t uart_read(uart_rxPacket_t *response) {
  
  // Initialize variables
  uart_result_t retvTotal = UART_SUCCESS;
  uart_result_t retv = UART_SUCCESS;

  // Get the packet header and validate its correctness
  retv = uart_readUsb(sUsb, (char*)&(response->packetHeader), sizeof(response->packetHeader));
  if( (retv != UART_SUCCESS) || (response->packetHeader != EXPECTED_PACKET_HEADER) ) {
    if(retv == UART_FAILURE) {
      printf("Error: invalid packet header\n");
    }
    retvTotal = retv;
  }
  else {
    // Get the source address
    retv = uart_readUsb(sUsb, (char*)&(response->sourceAddress), sizeof(response->sourceAddress));
    if(retv != UART_SUCCESS) {
      printf("Error: invalid source address bytes.\n");
      retvTotal = retv;
    }

    // Get the rssi
    retv = uart_readUsb(sUsb, (char*)&(response->rssi), sizeof(response->rssi));
    if(retv != UART_SUCCESS) {
      printf("Error: invalid rssi bytes.\n");
      retvTotal = retv;
    }

    // Get the bicycle message data
    retv = uart_readUsb(sUsb, (char*)&(response->bikeMessage), sizeof(response->bikeMessage));
    if(retv != UART_SUCCESS) {
      printf("Error: invalid bicycle message bytes.\n");
      retvTotal = retv;
    }

    // Get the checksum
    retv = uart_readUsb(sUsb, (char*)&(response->checksum), sizeof(response->checksum));
    if(retv != UART_SUCCESS) {
      printf("Error: invalid checksum.\n");
      retvTotal = retv;
    }

    //*****************************//
    // TODO: Validate the checksum //
    //*****************************//
  }

  return retvTotal;
}

uart_result_t uart_cleanup() {

  // Close the usb port
  close(sUsb);

  return UART_SUCCESS;
}
