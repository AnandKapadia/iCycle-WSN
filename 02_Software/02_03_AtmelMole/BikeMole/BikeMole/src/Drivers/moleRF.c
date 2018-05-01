/*
 * moleRF.c
 *
 * Created: 4/29/2018 10:20:55 PM
 *  Author: user
 */ 

#include "moleRF.h"

// Payload struct to hold the message to be sent to the master
static cornerMessage_t cornerMessage;
static bikeMessage_t bikeMessage;

// Transmit frame
static NWK_DataReq_t txMsg;


static void blinkGPIO(port_pin_t pin, uint16_t delay, uint8_t repeats){
    
    for(uint8_t i = 0; i < repeats;  i++) {
        gpio_set_pin_high(pin);
        delay_ms(delay);
        gpio_set_pin_low(pin);
        delay_ms(delay);
    }
}

// Application in initialization function stolen from the example project
// Most content not relevant to our application, only nwk and phy setup.
void appInit(void) {
    NWK_SetAddr(APP_ADDR);
    NWK_SetPanId(APP_PANID);
    PHY_SetChannel(APP_CHANNEL);
    PHY_SetRxState(true);
}

// Reception callback function
bool receivePacket(NWK_DataInd_t *ind)
{
    // process the frame
    // Blink RX LED
    blinkGPIO(RX_LED, 100, 1);
    
    // TODO set LEDs and buzzer based on car proximity
    
    // Do not send ACK frame
    return false;
}
// Call back function for sending a packet
static void txConfirm(NWK_DataReq_t *req){
    
    if(req->status == NWK_SUCCESS_STATUS) {
        // Blink LED to confirm transmission
        blinkGPIO(TX_LED, 50, 1);
    }
}
// Transmit a packet
void sendPacket(uint16_t destAddr, uint8_t *payload, uint8_t size) {

    txMsg.dstAddr = destAddr;
    txMsg.dstEndpoint = APP_ENDPOINT;
    txMsg.srcEndpoint = APP_ENDPOINT;
    txMsg.options = NWK_OPT_BROADCAST_PAN_ID;
    txMsg.data = payload;
    txMsg.size = size;
    txMsg.confirm = txConfirm;
    NWK_DataReq(&txMsg);
}
