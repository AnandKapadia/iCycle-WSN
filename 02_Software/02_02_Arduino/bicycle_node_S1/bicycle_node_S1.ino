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
#include <SparkFun_ADXL345.h>


#define BROADCAST_ADDRESS (0xFFFF)

// create the XBee object
XBee xbee = XBee();

// create the accelerometer object (i2c)
ADXL345 accelerometer = ADXL345();           

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

const int leftLed = 2;
const int rightLed = 4;
const int txLed = 7;
const int statusLed = 8;
const int rxLed = 9;
const int buzzer = 10;
const int buttonPin = 12;
const int errorLed = 13;

typedef struct{
  int16_t x;
  int16_t y;
  int16_t z;
} accel_data_t;

void setup() {
  //start the serial port
  Serial.begin(9600);
  
  //setup the io
  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(txLed, OUTPUT);
  pinMode(rxLed, OUTPUT);
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin, INPUT);

  //ensure all leds work
  flashLed(leftLed, 2, 250);
  flashLed(rightLed, 2, 250);
  flashLed(txLed, 2, 250);
  flashLed(rxLed, 2, 250);
  flashLed(statusLed, 2, 250);
  flashLed(errorLed, 2, 250);
  flashLed(buzzer, 2, 250);

  //accel setup
  setup_accelerometer();

  //xbee setup
  xbee.setSerial(Serial);
}


void loop() {  
  //inits
  uint8_t count = 0;
  uint8_t buttonVal = digitalRead(buttonPin);
  uint8_t txPayload[1] = {0};
  accel_data_t accel_data;
  accel_data.x = 0;
  accel_data.y = 0;
  accel_data.z = 0;

  //send if button pushed
  if(buttonVal) {
    while(buttonVal) {
      count++;
      buttonVal = digitalRead(buttonPin);
      delay(100);
    }

    //get the accelerometer data
    accelerometer.readAccel(&(accel_data.x), &(accel_data.y), &(accel_data.z)); 
    Serial.print(accel_data.x);
    Serial.print(", ");
    Serial.print(accel_data.y);
    Serial.print(", ");
    Serial.println(accel_data.z); 
    sendPacket(BROADCAST_ADDRESS, (uint8_t *)&accel_data, 1);
    flashLed(statusLed, count, 50);
    delay(1000);
  }
 
  delay(1);
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

static void flashLed(int pin, int times, int wait) {

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);

    if (i + 1 < times) {
      delay(wait);
    }
  }
}

static void setup_accelerometer(){
  accelerometer.powerOn();                     // Power on the accelerometer345

  accelerometer.setRangeSetting(16);           // Give the range settings
                                               // Accepted values are 2g, 4g, 8g or 16g
                                               // Higher Values = Wider Measurement Range
                                               // Lower Values = Greater Sensitivity

  accelerometer.setSpiBit(0);                  // Configure the device to be in 4 wire SPI mode when set to '0' or 3 wire SPI mode when set to 1
                                               // Default: Set to 1
                                               // SPI pins on the ATMega328: 11, 12 and 13 as reference in SPI Library 
   
  accelerometer.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "accelerometer.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  accelerometer.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  accelerometer.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "accelerometer.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  accelerometer.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  accelerometer.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  accelerometer.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "accelerometer.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  accelerometer.setTapThreshold(50);           // 62.5 mg per increment
  accelerometer.setTapDuration(15);            // 625 μs per increment
  accelerometer.setDoubleTapLatency(80);       // 1.25 ms per increment
  accelerometer.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // Set values for what is considered FREE FALL (0-255)
  accelerometer.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  accelerometer.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment
 
  // Setting all interupts to take place on INT1 pin
  //accelerometer.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "accelerometer.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                                   // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the accelerometer345 to use for Interrupts.
                                                                   // This library may have a problem using INT2 pin. Default to INT1 pin.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  accelerometer.InactivityINT(0);
  accelerometer.ActivityINT(0);
  accelerometer.FreeFallINT(0);
  accelerometer.doubleTapINT(0);
  accelerometer.singleTapINT(0);
}

