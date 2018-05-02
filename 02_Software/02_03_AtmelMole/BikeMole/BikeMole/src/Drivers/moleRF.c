/*
 * moleRF.c
 *
 * Created: 4/29/2018 10:20:55 PM
 *  Author: user
 */ 

#include "moleRF.h"

// Payload structs for received messages
static masterMessage_t masterMessage;
// Transmit frame
static NWK_DataReq_t txMsg;

//RX response timeout to disable output after messages stop
SYS_Timer_t rxResponseTimer;

static void blinkGPIO(port_pin_t pin, uint16_t delay, uint8_t repeats){
    
    for(uint8_t i = 0; i < repeats;  i++) {
        gpio_set_pin_high(pin);
        delay_ms(delay);
        gpio_set_pin_low(pin);
        delay_ms(delay);
    }
}
    
// Reception callback function
bool receivePacket(NWK_DataInd_t *ind)
{
    // process the frame
    // Blink RX LED
    blinkGPIO(RX_LED, TXRX_BLINK_TIME, 1);
    
    memcpy(&masterMessage, ind->data, sizeof(masterMessage));
    // TODO set LEDs and buzzer based on car proximity
    
    switch(masterMessage.location){
        case SOUTH:
            gpio_set_pin_high(LEFT_LED);
            gpio_set_pin_high(RIGHT_LED);
            //gpio_set_pin_low(buzzer);
            break;
        case NORTHEAST:
            gpio_set_pin_low(LEFT_LED);
            gpio_set_pin_high(RIGHT_LED);
            //gpio_set_pin_high(buzzer);
            break;
        case SOUTHEAST:
        case EAST:
            gpio_set_pin_low(LEFT_LED);
            gpio_set_pin_high(RIGHT_LED);
            //gpio_set_pin_low(buzzer);
            break;
        case NORTHWEST:
            gpio_set_pin_high(LEFT_LED);
            gpio_set_pin_low(RIGHT_LED);
            //gpio_set_pin_high(buzzer);
            break;
        case SOUTHWEST:
        case WEST:
            gpio_set_pin_low(LEFT_LED);
            gpio_set_pin_high(RIGHT_LED);
            //gpio_set_pin_low(buzzer);
            break;
        case NORTH:
            gpio_set_pin_high(LEFT_LED);
            gpio_set_pin_high(RIGHT_LED);
            //gpio_set_pin_high(buzzer);
            break;
        default:
            // Bad value, stop notifications
            gpio_set_pin_low(LEFT_LED);
            gpio_set_pin_high(RIGHT_LED);
            //gpio_set_pin_low(buzzer);
            break;
    }
    
    // Set timeout to disable output
    SYS_TimerStart(&rxResponseTimer);
    
    // Do not send ACK frame
    return false;
}void rxResponseTimerHandler(SYS_Timer_t *timer) {    gpio_set_pin_low(LEFT_LED);
    gpio_set_pin_low(RIGHT_LED);
    //gpio_set_pin_low(buzzer);}
// Call back function for sending a packet
static void txConfirm(NWK_DataReq_t *req){
    
    if(req->status == NWK_SUCCESS_STATUS) {
        // Blink LED to confirm transmission
        blinkGPIO(TX_LED, TXRX_BLINK_TIME, 1);
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

// Application in initialization function stolen from the example project
// Most content not relevant to our application, only nwk and phy setup.
void appInit(void) {
    NWK_SetAddr(APP_ADDR);
    NWK_SetPanId(APP_PANID);
    PHY_SetChannel(APP_CHANNEL);
    PHY_SetRxState(true);
	PHY_SetTxPower(0x00);
    
    // Setup rx response timer
    rxResponseTimer.interval = RX_RESPONSE_HOLD_MS;
    rxResponseTimer.mode = SYS_TIMER_INTERVAL_MODE;
    rxResponseTimer.handler = rxResponseTimerHandler;
}