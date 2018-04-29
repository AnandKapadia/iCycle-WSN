/*
 * CornerMole.c
 *
 * Created: 4/29/2018 3:54:53 PM
 * Author : user
 */ 

#include <asf.h>
#include <avr/io.h>
#include "config.h"
#include "nwk.h"
#include "phy.h"
#include "sys.h"
#include "delay.h"
#include "gpio.h"

// Reception callback function
static bool appDataInd(NWK_DataInd_t *ind)
{
    // process the frame
    // Blink LED
    gpio_set_pin_high(RX_LED);
    delay_ms(250);
    gpio_set_pin_low(RX_LED);
    
    // Send ACK frame
    return true;
}


// Application in initialization function stolen from the example project
// Most content not relevant to our application, only nwk and phy setup.
static void appInit(void)
{
    //Setup Message structs
    /*
    appMsg.commandId            = APP_COMMAND_ID_NETWORK_INFO;
    appMsg.nodeType             = APP_NODE_TYPE;
    appMsg.extAddr              = APP_ADDR;
    appMsg.shortAddr            = APP_ADDR;
    appMsg.softVersion          = 0x01010100;
    appMsg.channelMask          = (1L << APP_CHANNEL);
    appMsg.panId                = APP_PANID;
    appMsg.workingChannel       = APP_CHANNEL;
    appMsg.parentShortAddr      = 0;
    appMsg.lqi                  = 0;
    appMsg.rssi                 = 0;

    appMsg.sensors.type        = 1;
    appMsg.sensors.size        = sizeof(int32_t) * 3;
    appMsg.sensors.battery     = 0;
    appMsg.sensors.temperature = 0;
    appMsg.sensors.light       = 0;

    appMsg.caption.type         = 32;
    appMsg.caption.size         = APP_CAPTION_SIZE;
    memcpy(appMsg.caption.text, APP_CAPTION, APP_CAPTION_SIZE);
    */
    
    //
    NWK_SetAddr(APP_ADDR);
    NWK_SetPanId(APP_PANID);
    PHY_SetChannel(APP_CHANNEL);
    PHY_SetRxState(true);

    // ?????
    NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);
    
    
    // Seems irrelevant
    /*
    appDataSendingTimer.interval = APP_SENDING_INTERVAL;
    appDataSendingTimer.mode = SYS_TIMER_INTERVAL_MODE;
    appDataSendingTimer.handler = appDataSendingTimerHandler;

    #if APP_ROUTER || APP_ENDDEVICE
    appNetworkStatus = false;
    appNetworkStatusTimer.interval = 500;
    appNetworkStatusTimer.mode = SYS_TIMER_PERIODIC_MODE;
    appNetworkStatusTimer.handler = appNetworkStatusTimerHandler;
    SYS_TimerStart(&appNetworkStatusTimer);

    appCommandWaitTimer.interval = NWK_ACK_WAIT_TIME;
    appCommandWaitTimer.mode = SYS_TIMER_INTERVAL_MODE;
    appCommandWaitTimer.handler = appCommandWaitTimerHandler;
    #else
    #if (LED_COUNT > 0)
    LED_On(LED_NETWORK);
    
    #endif
    #endif
    */
    //APP_CommandsInit();

}

void txConfirm(NWK_DataReq_t *req){
    
    if(req->status == NWK_SUCCESS_STATUS) {
        // Blink LED to confirm transmission
        gpio_set_pin_high(TX_LED);
        delay_ms(250);
        gpio_set_pin_low(TX_LED);

    } else {
        // ERROR: transmission failed
    }        
     
}

static void sendPacket(uint16_t destAddr, uint8_t *payload, uint8_t size) {

    NWK_DataReq_t txMsg;

    txMsg.dstAddr = destAddr;
    txMsg.dstEndpoint = APP_ENDPOINT;
    txMsg.srcEndpoint = APP_ENDPOINT;
    txMsg.options = NWK_OPT_BROADCAST_PAN_ID;
    txMsg.data = payload;
    txMsg.size = size;
    txMsg.confirm = txConfirm;
    NWK_DataReq(&txMsg);
}


int main(void)
{
    // TODO switch clock source to external oscillator?
	board_init();

    // Initialize RF system
    SYS_Init();

    // Initialize application parameters
    appInit();

    // Initialize other drivers
    delay_init(unsigned long 16000000);

    uint8_t payload = 0x42;
    
    gpio_set_pin_high(RX_LED);
    gpio_set_pin_high(TX_LED);
    delay_ms(250);
    gpio_set_pin_low(RX_LED);
    gpio_set_pin_low(TX_LED);
    
    delay_ms(1000);

    
    /* Replace with your application code */
    while (1) 
    {
        SYS_TaskHandler();
        sendPacket(0x0002, &payload, 1);
        delay_ms(1000);
        
    }
}

