// Import custom modules
#include "xbee16A.h"

//------------------------------------------------------------------------------
// GLOBAL CONSTANT DEFINITIONS                                                 |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// STATIC VARIABLE DEFINITIONS                                                 |
//------------------------------------------------------------------------------
static int sUsb = -1;

//------------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS                                                     |
//------------------------------------------------------------------------------
static int32_t xbee16A_read_reverse(int usb, char buf[MAX_BUFFER_LENGTH], uint16_t length) {

  int32_t retv = 0;

  int32_t charsRead = 0;
  int32_t bufPos = length-1;
  char charBuf = '\0';

  do {
    charsRead = read(usb, &charBuf, 1);
    sprintf(&buf[bufPos], "%c", charBuf);
    bufPos -= charsRead;
    retv += charsRead;
  } while((charBuf != '\r') && (charsRead > 0) && (bufPos >= 0));

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
xbee16A_result_t xbee16A_writeBuffer(const char writeBuf[MAX_BUFFER_LENGTH], uint16_t numChars) {

  xbee16A_result_t retv = XBEE16A_SUCCESS;

  uint16_t charsWritten = 0;

  if(numChars > MAX_BUFFER_LENGTH) {
    printf("Error: Tried to write too many characters.\n");
    retv = XBEE16A_FAILURE;
  }
  else {
    printf("--------------\n");
    for(int i = 0; i < numChars; i++) {
      printf("0x%x\n", writeBuf[i]);
    }
    printf("--------------\n");
    charsWritten = write(sUsb, writeBuf, numChars);
  }

  if(charsWritten < numChars) {
    retv = XBEE16A_FAILURE;
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
xbee16A_result_t xbee16A_init() {

  xbee16A_result_t retv = XBEE16A_SUCCESS;

  // Open USB port for read/write
  sUsb = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
  if(sUsb < 0) {
    printf("Error: Invalid USB device.\n");
    retv = XBEE16A_FAILURE;
  }

  // Error handling
  struct termios tty;
  memset(&tty, 0, sizeof(tty));
  if(tcgetattr(sUsb, &tty) != 0) {
    printf("Error: tcgetattr returned error\n");
    retv = XBEE16A_FAILURE;
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
    retv = XBEE16A_FAILURE;
  }

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
xbee16A_result_t xbee16A_write(xbee16A_txPacket_t *command) {

  xbee16A_result_t retv = XBEE16A_SUCCESS;

  char writeBuf[MAX_BUFFER_LENGTH] = {0};
  uint16_t numChars = 8;

  writeBuf[0] = command->startDelimiter;
  writeBuf[1] = (command->length >> 8) & 0xFF;
  writeBuf[2] = (command->length) & 0xFF;
  writeBuf[3] = command->frameData.apiIdentifier;
  writeBuf[4] = command->frameData.commandData.frameId;
  writeBuf[5] = (command->frameData.commandData.destinationAddress >> 8) & 0xFF;
  writeBuf[6] = (command->frameData.commandData.destinationAddress) & 0xFF;
  writeBuf[7] = command->frameData.commandData.options;
  uint8_t checksum = 0;
  for(uint16_t i = 0; i < (command->length-5); i++) {
    writeBuf[8+i] = command->frameData.commandData.data[i];
    ++numChars;
    checksum += writeBuf[8+i];
  }
  checksum += writeBuf[3]+writeBuf[4]+writeBuf[5]+writeBuf[6]+writeBuf[7];
  checksum = 0xFF - checksum;

  writeBuf[numChars] = checksum;
  ++numChars;

  retv = xbee16A_writeBuffer(writeBuf, numChars);

  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
xbee16A_result_t xbee16A_read(xbee16A_rxPacket_t *response) {
  
  // Initialize variables
  xbee16A_result_t retv = XBEE16A_SUCCESS;
  uint16_t charsRead = 0;

  // Check the start delimiter
  uint16_t retries = 0;
  do {
    charsRead = read(sUsb, &(response->startDelimiter), sizeof(response->startDelimiter));
    ++retries;
  } while(((response->startDelimiter != 0x7E) || (charsRead != sizeof(response->startDelimiter))) && (retries < 100));

  if(retries >= 100) {
    printf("Error: invalid start byte\n");
    retv = XBEE16A_FAILURE;
  }

  // Get the length of the packet
  charsRead = xbee16A_read_reverse(sUsb, (char*)&(response->length), sizeof(response->length));
  if(charsRead != sizeof(response->length)) {
    printf("Error: invalid length bytes\n");
    retv = XBEE16A_FAILURE;
  }

  // Get the API identifier
  charsRead = read(sUsb, &(response->frameData.apiIdentifier), sizeof(response->frameData.apiIdentifier));
  if((response->frameData.apiIdentifier != 0x81) || (charsRead != sizeof(response->frameData.apiIdentifier))) {
    printf("Error: invalid api identifier byte.\n");
    retv = XBEE16A_FAILURE;
  }

  // Get the source address
  charsRead = xbee16A_read_reverse(sUsb, (char*)&(response->frameData.commandData.sourceAddress), sizeof(response->frameData.commandData.sourceAddress));
  if(charsRead != sizeof(response->frameData.commandData.sourceAddress)) {
    printf("Error: invalid source address bytes.\n");
    retv = XBEE16A_FAILURE;
  }

  // Get the rssi value
  charsRead = read(sUsb, &(response->frameData.commandData.rssi), sizeof(response->frameData.commandData.rssi));
  if(charsRead != sizeof(response->frameData.commandData.rssi)) {
    printf("Error: invalid rssi byte.\n");
    retv = XBEE16A_FAILURE;
  }

  // Get the options byte
  charsRead = read(sUsb, &(response->frameData.commandData.options), sizeof(response->frameData.commandData.options));
  if(charsRead != sizeof(response->frameData.commandData.rssi)) {
    printf("Error: invalid options byte.\n");
    retv = XBEE16A_FAILURE;
  }

  // Get the remaining data packet
  charsRead = read(sUsb, &(response->frameData.commandData.data), (response->length)-5);
  if(charsRead != ((response->length)-5)) {
    printf("Error: invalid response bytes.\n");
    retv = XBEE16A_FAILURE;
  }
 
  return retv;
}

//------------------------------------------------------------------------------
//    NAME |                                                                   |
//    ARGS |                                                                   |
// RETURNS |                                                                   |
// PURPOSE |                                                                   |
//------------------------------------------------------------------------------
void xbee16A_readResponseDataOut(xbee16A_rxPacket_t *response, xbee16A_nodePacket_t *nodePacket) {

  nodePacket->orientation = response->frameData.commandData.data[0];
  nodePacket->xAcceleration = response->frameData.commandData.data[1];
  nodePacket->yAcceleration = response->frameData.commandData.data[2];
  nodePacket->zAcceleration = response->frameData.commandData.data[3];

  return;
}
