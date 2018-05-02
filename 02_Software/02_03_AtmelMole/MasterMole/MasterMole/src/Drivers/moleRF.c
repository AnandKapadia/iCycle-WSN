/*
 * moleRF.c
 *
 * Created: 4/29/2018 10:20:55 PM
 *  Author: user
 */ 

#include "moleRF.h"

// Payload structs to hold received messages
static cornerMessage_t cornerMessage;
static bikeMessage_t bikeMessage;

// Message to send to master pi
static uart_rxPacket_t piMessage;

// Transmit frame
static NWK_DataReq_t txMsg;

static void transferReceivedPacket(uart_rxPacket_t *pkt){
    serial_write_packet(pkt, sizeof(uart_rxPacket_t));
}


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
    blinkGPIO(RX_LED, 50, 1);
    
    
    if(ind->dstAddr == BROADCAST_ADDR && ind->size == sizeof(bikeMessage)) {
        // We received a packet from a bike node! send it to the pi
        piMessage.packetHeader = 0x7A; // TODO convert to const
        piMessage.sourceAddress.vehicleAddress = (uint8_t) ind->srcAddr;
        piMessage.sourceAddress.cornerAddress = FRONT_LEFT;
        piMessage.sourceAddress.vehicleType = BICYCLE;
        piMessage.rssi = ind->rssi;
        piMessage.packetTrailer = '\n';
        memcpy(&piMessage.bikeMessage, ind->data, sizeof(bikeMessage_t));

    } else if (ind->dstAddr == MASTER_ADDR && ind->size == sizeof(cornerMessage_t)) {
        // We received a message from another corner, parse it to send to the pi
        cornerMessage_t *cornerPtr = ind->data;
        
        piMessage.packetHeader = 0x7A;
        piMessage.sourceAddress.vehicleAddress = cornerPtr->bikeAddress;
        piMessage.sourceAddress.cornerAddress = ind->srcAddr;
        piMessage.sourceAddress.vehicleType = BICYCLE;
        piMessage.rssi = cornerPtr->bikeRssi;
        piMessage.packetTrailer = '\n';
        memcpy(&piMessage.bikeMessage, &(cornerPtr->bikePayload), sizeof(bikeMessage));

    }
        
    // Send to the master pi
    transferReceivedPacket(&piMessage);
    //sendPacket(MASTER_ADDR, &cornerMessage, sizeof(cornerMessage));
    
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
