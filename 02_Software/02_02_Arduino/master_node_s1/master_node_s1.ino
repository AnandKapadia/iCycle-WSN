/**
 *
 *
 */
 
#include <XBee.h>

#define XBEE_SUCCESS  0
#define XBEE_ERROR    0xff
#define XBEE_RX_EMPTY 1


static const uint64_t bikeStaleTimeout_ms = 500;

static const uint8_t MAX_BIKES = 8;
static const uint8_t BIKE_PAYLOAD_SIZE = 1;
static const uint8_t NUM_CORNERS = 2;
static const uint8_t MASTER_ADDR = 0;

typedef enum {CORNER_RR_ADDR = 1, CORNER_RL_ADDR = 2, CORNER_FR_ADDR = 3, CORNER_FL_ADDR = MASTER_ADDR} node_addr_t;
//#define REAR_LEFT_ADDRESS 1
//#define REAR_RIGHT_ADDRESS 2

typedef struct cornerMessage_t {
  uint16_t bikeAddr;
  uint8_t bikeRssi;
  uint8_t bikePayload[BIKE_PAYLOAD_SIZE];
} cornerMessage_t;

// TODO Expand to master sotring a set of corner data for each in a list of bikes

// TODO add timestamp for checking data freshness
typedef struct cornerData_t {
  bool isFresh;
  uint16_t addr; // Address of this corner
  uint8_t rssi; // RSSI of the last packet received
  uint64_t timestamp; // Time in milliseconds when last packet was received
} cornerData_t;

typedef struct bikeData_t {
  bool isFresh;
  uint16_t addr;
  cornerData_t corners[NUM_CORNERS];
} bikeData_t;

//cornerNode_t corners[NUM_CORNERS];

static bikeData_t bikes[MAX_BIKES] = { 0 };

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


int txLed = 9;
int rxLed = 8;
int rearLeftCornerLed = 4;
int rearRightCornerLed = 2;
int rearLeftRxLed = 8;
int rearRightRxLed = 7;
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
      xbee.getResponse().getRx64Response(rx16);
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
  //flashLed(txLed, 1, 100);
  
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


// Initializes the corners of a bike
// Kinda jank, should use enum
void initBikeCorners(bikeData_t *bike) {
  for(uint8_t i = 0; i < NUM_CORNERS; i++) {
    bikes->corners[i].isFresh = false;
    bikes->corners[i].addr = i;
    bikes->corners[i].rssi = 0;
    bikes->corners[i].timestamp = 0;
  }
}


// Returns true if any of the corners for a bike are fresh, false otherwise
bool isBikeFresh(bikeData_t *bike) {
  for(uint8_t i = 0; i < NUM_CORNERS; i++) {
    if(bikes->corners[i].isFresh == true){
      return true;
    }
  }
  return false;
}

// Stores the data for a bike message in the bikeData array. Finds the relevant bike
// and the relevant corner within that bike and updates the RSSI and freshness timer.
// If the relevant bike is not found in the array, it is added to an empty location.
// Returns true if data was stored, false if bike array was full.
bool storeCornerData(uint16_t bikeAddr, uint16_t cornerAddr, uint8_t rssi) {
  uint8_t i;
  uint8_t emptyIndex = 0xff;
  uint8_t bikeIndex = 0xff; // Index of the bike in bikeData;
  for(i = 0; i < MAX_BIKES; i++) {
    if( isBikeFresh(&bikes[i]) && bikes[i].addr == bikeAddr){
      bikeIndex = i;
      break;
    }
    if( !isBikeFresh(&bikes[i]) && emptyIndex == 0xFF) {
      emptyIndex = i;
    }
  }
  if(bikeIndex == 0xff) {
    // Did not find a fresh bike
    if(emptyIndex == 0xff) {
      // All other bike slots were already filled, return without storing
      return false; 
    }
    // Create a new bike entry
    bikeIndex = emptyIndex;
    initBikeCorners(&bikes[bikeIndex]);
  }
  
  for(i = 0; i < NUM_CORNERS; i++) {
    if(bikes[bikeIndex].corners[i].addr == cornerAddr){
      bikes[bikeIndex].corners[i].rssi = rssi;
      bikes[bikeIndex].corners[i].isFresh = true;
      bikes[bikeIndex].corners[i].timestamp = millis();
      return true;
    }
  }

  return false;
}


// TODO Convert this to bike address, not index
uint8_t getCornerData(uint8_t bikeIndex, uint16_t cornerAddr) {
  for(uint8_t i = 0; i < NUM_CORNERS; i++) {
    if(bikes[bikeIndex].corners[i].addr == cornerAddr) {
      return bikes[bikeIndex].corners[i].rssi;
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
  pinMode(10, OUTPUT);

  
  // start serial
  
  Serial.begin(9600);
  xbee.begin(Serial);
  
  flashLed(txLed, 2, 50);
  flashLed(rxLed, 2, 50);
  flashLed(rearRightCornerLed, 2, 50);
  flashLed(rearRightRxLed, 2, 50);
  flashLed(rearLeftCornerLed, 2, 50);
  flashLed(rearLeftRxLed, 2, 50);
  flashLed(errorLed, 2, 50);
  //delay(500);
}

// Checks all bikes for stale corners. Removes corner data that is stale.
// If all corners are stale for a bike, it is removed from the list.
// Number of bikes remaining aftyer cull is returned.
uint8_t cullBikes(){
  return 0;
}

bool isCornerAddr(uint16_t addr) {
  return addr == CORNER_RL_ADDR || addr == CORNER_RR_ADDR;
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    
  uint8_t retv;
  retv = retrievePacket();
  if(retv  == XBEE_ERROR) {
    flashLed(errorLed, 2, 100);
  } else if(retv == XBEE_SUCCESS){
    

    // Received message frame data and payload
    uint8_t *rxPayload = rx16.getData();
    uint8_t *rxFrame = rx16.getFrameData();
    uint16_t sourceAddr;

    // Information about the transmitting bike, regardless of
    // whether or not it came through a corner
    uint8_t *bikePayload;
    uint16_t bikeAddr;
    uint8_t bikeRssi;

    // Address of the corner where the bike message was received
    uint16_t cornerAddr;

    // Fill in source address
    // TODO convert to 64bit addressing so all bikes are unique
    sourceAddr = (((uint16_t) rxFrame[0]) << 8) | ((uint16_t) rxFrame[1]);//*(uint16_t*)rxFrame;
    
    if(sourceAddr != 42) {
      flashLed(10, 2, 100);
    }
    if(isCornerAddr(sourceAddr)) {
      //flashLed(rxLed, 2, 100);
      // Message came from a corner node on our vehicle
      // Check payload for bike address and bike message RSSI
      /*
      cornerMessage_t *cornerMessage = (cornerMessage_t*) rxPayload;
      bikeAddr = cornerMessage->bikeAddr;
      bikeRssi = cornerMessage->bikeRssi;
      bikePayload = cornerMessage->bikePayload;
      */
      bikeRssi = rxPayload[2];
      bikeAddr = (((uint16_t) rxPayload[0]) << 8) | ((uint16_t) rxPayload[1]);
      cornerAddr = sourceAddr;
      flashLed(10, 2, 100);

      storeCornerData(bikeAddr, cornerAddr, bikeRssi);
      flashLed(10, 2, 100);

      if(cornerAddr == 1)
        flashLed(rearLeftRxLed, 2, 250);
      if(cornerAddr == 2)
        flashLed(rearRightRxLed, 2, 250);

    } else {
      flashLed(10, 1, 100);

      // Message came directly from bike, fill in RSSI from this packet
      bikeAddr = sourceAddr;
      bikeRssi = rx16.getRssi();
      bikePayload = rxPayload;
      // Set receiving corner to be the master 
      cornerAddr = CORNER_FL_ADDR;
    }

    //bikeAddr = *(uint64_t*)rxPayload;
    // Invert RSSI as it's reported as negative decibels
    //cornerRssi = 255-rxPayload[2];

    // Store the RSSI data from this packet with the appropriate bike and corner
    //storeCornerData(bikeAddr, cornerAddr, bikeRssi);



    //flashLed(rxLed, 2, 250);
  }



  
  // TODO Process this data properly for multiple bikes

  for (uint8_t i = 0; i < MAX_BIKES; i++) {
    if( isBikeFresh(&bikes[i])) {
      
      if(getCornerData(i, CORNER_RL_ADDR) < getCornerData(i, CORNER_RR_ADDR)) {
        digitalWrite(rearLeftCornerLed, HIGH);
        digitalWrite(rearRightCornerLed, LOW);
      } else {
       digitalWrite(rearLeftCornerLed, LOW);
       digitalWrite(rearRightCornerLed, HIGH);
      }
      // display one bike for now
      break;
    }
  }

  // TODO cull corners, expunging those who are unfresh

  //flashLed(10, 1, 50);

  delay(1);
}




