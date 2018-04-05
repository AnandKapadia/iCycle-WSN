// Import custom modules
#include "display.h"
#include "xbee16A.h"

int main() {

  xbee16A_result_t retv = XBEE16A_SUCCESS;

  // Perform initialization
  display_init();
  xbee16A_init();

  // Execute console
  const char writeBuf[MAX_BUFFER_LENGTH] = "INIT\r";
  retv = xbee16A_write(writeBuf, strlen(writeBuf));
  if(retv != XBEE16A_SUCCESS) {
    printf("Error: xbee could not read correctly.\n");
  }

  xbee16A_rxPacket_t response;// = {0};
  retv = xbee16A_read(&response);
  if(retv != XBEE16A_SUCCESS) {
    printf("Error: xbee could not read correctly.\n");
  }

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

  SDL_Delay(100);
  display_update(NORTH);
  SDL_Delay(100);
  display_update(NORTHEAST);
  SDL_Delay(100);
  display_update(EAST);
  SDL_Delay(100);
  display_update(SOUTHEAST);
  SDL_Delay(100);
  display_update(SOUTH);
  SDL_Delay(100);
  display_update(SOUTHWEST);
  SDL_Delay(100);
  display_update(WEST);
  SDL_Delay(100);
  display_update(NORTHWEST);
  SDL_Delay(500);

  // Clean up files
  display_destroy();

  return 0;
}
