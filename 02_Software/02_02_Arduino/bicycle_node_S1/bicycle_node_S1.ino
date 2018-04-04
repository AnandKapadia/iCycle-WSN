/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <XBee.h>

#define BROADCAST_ADDRESS 0xFFFF

// create the XBee object
XBee xbee = XBee();

/*
// allocate two bytes for to hold a 10-bit analog reading
uint8_t payload[] = { 0, 0 };

// with Series 1 you can use either 16-bit or 64-bit addressing

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0xFFFF, payload, sizeof(payload));

// 64-bit addressing: This is the SH + SL address of remote XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x4008b490);
// unless you have MY on the receiving radio set to FFFF, this will be received as a RX16 packet
//Tx64Request tx = Tx64Request(addr64, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();
*/
// IO Pinmap
int txLed = 8;
int rxLed = 9;
int statusLed = 11;
int errorLed = 13;
int buttonPin = 12;

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

void setup() {
  pinMode(txLed, OUTPUT);
  pinMode(rxLed, OUTPUT);
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(buttonPin, INPUT);

  flashLed(txLed, 2, 250);
  flashLed(rxLed, 2, 250);
  flashLed(statusLed, 2, 250);
  flashLed(errorLed, 2, 250);
  
  Serial.begin(9600);
  xbee.setSerial(Serial);
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
        flashLed(txLed, 2, 250);
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

void loop() {   
  uint8_t count = 0;
  uint8_t buttonVal = digitalRead(buttonPin);
  uint8_t txPayload[1] = {0};
  
  if(buttonVal) {
    while(buttonVal) {
      count++;
      buttonVal = digitalRead(buttonPin);
      delay(100);
    }
    txPayload[0] = count;
    sendPacket(BROADCAST_ADDRESS, txPayload, 1);
    flashLed(statusLed, count, 50);
    delay(1000);
  }
 
  delay(1);
}

