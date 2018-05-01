/**
 *
 *
 */
 
#include <XBee.h>

#define XBEE_SUCCESS  0
#define XBEE_ERROR    0xff
#define XBEE_RX_EMPTY 1

#define MASTER_ADDRESS 0
#define MY_ID 2

int count = 0;
XBee xbee = XBee();
bool gotData = false;

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
        flashLed(txLed, 2, 50);
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

void displayBinary(int pin, uint8_t val, uint8_t period) {
  flashLed(pin, 1, 250);
  for(uint8_t i = 0; i < 8; i++) {
    if((val>>i) &0x01)
      digitalWrite(pin, HIGH);
    else
      digitalWrite(pin, LOW);
    delay(period);
  }
  flashLed(pin, 1, 250);
}

void setup() {
  pinMode(rxLed, OUTPUT);
  pinMode(txLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  
  // start serial
  
  Serial.begin(9600);
  xbee.begin(Serial);
  
  flashLed(txLed, 3, 50);
  flashLed(rxLed, 3, 50);
  delay(5000);
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
    
  uint8_t retv;
  retv = retrievePacket();
  if(retv  == XBEE_ERROR) {
    flashLed(errorLed, 2, 100);
  } else if(retv == XBEE_SUCCESS){

    // Received payload, we don't care about it for now
    uint8_t *rxPayload = rx16.getData();
    // Received frame containing source addr, rssi, options
    uint8_t *rxFrame = rx16.getFrameData();

    
    flashLed(rxLed, 2, 50);

    // Populate packet to master
    uint8_t txPayload[7] = { 0 };
    
    // Fill in source address
    // TODO convert to 64bit addressing so all bikes are unique
    txPayload[0] = rxFrame[0];
    txPayload[1] = rxFrame[1];
    
    // Fill in the RSSI of the received packet
    txPayload[2] = rx16.getRssi();

    txPayload[3] = rxPayload[0];
    txPayload[4] = rxPayload[1];
    txPayload[5] = rxPayload[2];
    txPayload[6] = rxPayload[3];

    //displayBinary(txPayload[2],

    delay(10*MY_ID);
    sendPacket(MASTER_ADDRESS, txPayload, 7);
    
    
  }
  delay(1);
}




