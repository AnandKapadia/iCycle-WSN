/**
 *
 *
 */
 
#include <XBee.h>

#define XBEE_SUCCESS  0
#define XBEE_ERROR    0xff
#define XBEE_RX_EMPTY 1

#define NUM_CORNERS 2
#define MASTER_ADDRESS 0
#define REAR_LEFT_ADDRESS 1
#define REAR_RIGHT_ADDRESS 2


// TODO Expand to master sotring a set of corner data for each in a list of bikes

// TODO add timestamp for checking data freshness
typedef struct cornerNode_t {
  uint8_t addr;
  uint8_t rssi;
} cornerNode_t;

cornerNode_t corners[NUM_CORNERS];

XBee xbee = XBee(); 

/*
 * Packet setup for reception from a bike node
 */

//XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
static Rx16Response rx16 = Rx16Response();
//ModemStatusResponse msr = ModemStatusResponse();


/*
 * Packet setup for transmission from this node to the master
 */


int txLed = 8;
int rxLed = 9;
int rearLeftCornerLed = 6;
int rearRightCornerLed = 7;
int rearLeftRxLed = 2;
int rearRightRxLed = 4;
int errorLed = 13;

/* 
 * Helper functions 
 */

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

//Retrieve a received packet and store it in rx16
uint8_t retrievePacket(){
  xbee.readPacket();
    
  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      // got a rx packet
      xbee.getResponse().getRx16Response(rx16);
      return XBEE_SUCCESS;
    } else {
      // not something we were expecting
      return XBEE_ERROR;    
    }
  } else if (xbee.getResponse().isError()) {
    return XBEE_ERROR;
  } else {
    return XBEE_RX_EMPTY;
  }
}

// Helper for sending an XBee Packet
// TODO add condition codes
void sendPacket(uint16_t destAddress, uint8_t* payload, uint8_t payloadSize) {

  // Setup the packet to send to the master node, typically the coordinator
  Tx16Request tx = Tx16Request(destAddress, payload, payloadSize);
  // Packet to hold the reponse from XBee after transmission complete
  TxStatusResponse txStatus = TxStatusResponse();

  xbee.send(tx);

  // flash TX indicator
  flashLed(txLed, 1, 100);
  
  // after sending a tx request, we expect a status response
  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000)) {
    // got a response!

    // should be a znet tx status              
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      xbee.getResponse().getTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getStatus() == SUCCESS) {
        // success.  time to celebrate
        flashLed(txLed, 1, 100);
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        flashLed(errorLed, 3, 250);
      }
    }      
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
    // or flash error led
  } else {
    // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
    flashLed(errorLed, 2, 250);
  }
}

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
void setup() {
  pinMode(rxLed, OUTPUT);
  pinMode(txLed, OUTPUT);
  pinMode(rearLeftRxLed, OUTPUT);
  pinMode(rearRightRxLed, OUTPUT);
  pinMode(rearLeftCornerLed, OUTPUT);
  pinMode(rearRightCornerLed, OUTPUT);
  pinMode(errorLed, OUTPUT);

  // Mark Corners
  corners[0].addr = REAR_LEFT_ADDRESS;
  corners[1].addr = REAR_RIGHT_ADDRESS;
  
  // start serial
  
  Serial.begin(9600);
  xbee.begin(Serial);
  
  flashLed(txLed, 2, 250);
  flashLed(rxLed, 2, 250);
  flashLed(rearRightCornerLed, 2, 250);
  flashLed(rearRightRxLed, 2, 250);
  flashLed(rearLeftCornerLed, 2, 250);
  flashLed(rearLeftRxLed, 2, 250);
  delay(500);
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    
  uint8_t retv;
  retv = retrievePacket();
  if(retv  == XBEE_ERROR) {
    flashLed(errorLed, 2, 100);
  } else if(retv == XBEE_SUCCESS){
    

    // Received payload
    uint8_t *rxPayload = rx16.getData();
    uint8_t *rxFrame = rx16.getFrameData();
    uint16_t cornerAddr;
    uint8_t cornerRssi;

    // Fill in source address
    // TODO convert to 64bit addressing so all bikes are unique
    cornerAddr = (((uint16_t) rxFrame[0]) << 8) | ((uint16_t) rxFrame[1]);

    cornerRssi = rxPayload[2];

    // Invert RSSI as it's reported as negative decibels
    if(storeCornerData(cornerAddr, cornerRssi)) {
      if(cornerAddr == REAR_LEFT_ADDRESS) {
        flashLed(rearLeftRxLed, 2, 250);
      }
      if(cornerAddr == REAR_RIGHT_ADDRESS) {
        flashLed(rearRightRxLed, 2, 250);
      }
      
    }
    flashLed(rxLed, 2, 100);
  }
  //flashLed(rearLeftCornerLed, getCornerData(REAR_LEFT_ADDRESS), 50);
  //analogWrite(rearLeftCornerLed, getCornerData(REAR_LEFT_ADDRESS));
  //analogWrite(rearRightCornerLed, getCornerData(REAR_RIGHT_ADDRESS));
  
  if(getCornerData(REAR_LEFT_ADDRESS) < getCornerData(REAR_RIGHT_ADDRESS)) {
    digitalWrite(rearLeftCornerLed, HIGH);
    digitalWrite(rearRightCornerLed, LOW);
  } else {
    digitalWrite(rearLeftCornerLed, LOW);
    digitalWrite(rearRightCornerLed, HIGH);
  }
  
  delay(1);
}




