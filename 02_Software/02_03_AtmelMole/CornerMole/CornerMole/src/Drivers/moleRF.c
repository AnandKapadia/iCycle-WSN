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
	PHY_SetTxPower(0x00);
}

// Reception callback function
bool receivePacket(NWK_DataInd_t *ind)
{
    // process the frame
    // Blink RX LED
    gpio_set_pin_high(RX_LED);
    delay_ms(100);
    gpio_set_pin_low(RX_LED);
    
    //uint8_t rxPayload = ind->data[0];
    //uint8_t rxRSSI = ind->rssi;
    //blinkGPIO(STATUS_LED, 1, (rxRSSI));
    
    if(ind->size == sizeof(bikeMessage)) {
        // We received a packet from a bike node!
        // Send a message to the master with its details.
        cornerMessage.bikeAddress = ind->srcAddr;
        cornerMessage.bikeRssi = ind->rssi;
        memcpy(&cornerMessage.bikePayload, ind->data, sizeof(bikeMessage));

#ifdef DEBUG_UART
        uint8_t uartMsgBuf[20];
        sprintf(uartMsgBuf, "RSSI: %d\n", cornerMessage.bikeRssi);
        serial_write_packet(uartMsgBuf, strlen(uartMsgBuf));
#endif

        // Send the message to the master. This could be hoisted out of the callback
        sendPacket(MASTER_ADDR, &cornerMessage, sizeof(cornerMessage));
    }
    
    // Do not send ACK frame
    return false;
}
// Call back function for sending a packet
// Blinks error LEDs to convey error type
static void txConfirm(NWK_DataReq_t *req){
    
    if(req->status == NWK_SUCCESS_STATUS) {
        // Blink LED to confirm transmission
        blinkGPIO(TX_LED, 100, 1);
        } else if(req->status == NWK_ERROR_STATUS) {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 1);
        } else if(req->status == NWK_OUT_OF_MEMORY_STATUS) {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 2);
        } else if(req->status == NWK_NO_ACK_STATUS) {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 3);
        } else if(req->status == NWK_NO_ROUTE_STATUS) {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 4);
        } else if(req->status == NWK_PHY_CHANNEL_ACCESS_FAILURE_STATUS) {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 5);
        } else if(req->status == NWK_PHY_NO_ACK_STATUS) {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 6);
        } else {
        // ERROR: transmission failed
        blinkGPIO(ERROR_LED, 100, 7);
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
