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
#include "sysTimer.h"
#include "delay.h"
#include "gpio.h"


static void blinkGPIO(port_pin_t pin, uint16_t delay, uint8_t repeats){
    
    for(uint8_t i = 0; i < repeats;  i++) {
        gpio_set_pin_high(pin);
        delay_ms(delay);
        gpio_set_pin_low(pin);
        delay_ms(delay);
    }
}


// Reception callback function
static bool appDataInd(NWK_DataInd_t *ind)
{
    // process the frame
    // Blink LED
    gpio_set_pin_high(RX_LED);
    delay_ms(100);
    gpio_set_pin_low(RX_LED);
    
    uint8_t rxPayload = ind->data[0];
    
    blinkGPIO(STATUS_LED, 50, (rxPayload>>4));
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

static NWK_DataReq_t txMsg;

static void sendPacket(uint16_t destAddr, uint8_t *payload, uint8_t size) {

    txMsg.dstAddr = destAddr;
    txMsg.dstEndpoint = APP_ENDPOINT;
    txMsg.srcEndpoint = APP_ENDPOINT;
    txMsg.options = NWK_OPT_BROADCAST_PAN_ID;
    txMsg.data = payload;
    txMsg.size = size;
    txMsg.confirm = txConfirm;
    NWK_DataReq(&txMsg);
}

static SYS_Timer_t sendTimer;
static uint8_t payload = 0x4;

static void sendTimerHandler(SYS_Timer_t *timer) {

    sendPacket(0x0001, &payload, 1);
    //gpio_toggle_pin(STATUS_LED);
}

int main(void)
{
    
  	irq_initialize_vectors();

    // TODO switch clock source to external oscillator?
	board_init();
    sysclk_init();

    // Initialize RF system
    SYS_Init();

    // Initialize application parameters
    appInit();

    // Setup reception
    NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);

    // Initialize other drivers
        
    gpio_configure_pin(RX_LED, IOPORT_DIR_OUTPUT);
    gpio_configure_pin(TX_LED, IOPORT_DIR_OUTPUT);
    gpio_configure_pin(STATUS_LED, IOPORT_DIR_OUTPUT);
    gpio_configure_pin(ERROR_LED, IOPORT_DIR_OUTPUT);

    gpio_set_pin_high(RX_LED);
    gpio_set_pin_high(TX_LED);
    gpio_set_pin_high(STATUS_LED);
    gpio_set_pin_high(ERROR_LED);
    delay_ms(250);
    gpio_set_pin_low(RX_LED);
    gpio_set_pin_low(TX_LED);
    gpio_set_pin_low(STATUS_LED);
    gpio_set_pin_low(ERROR_LED);
    
    delay_ms(1000);

    cpu_irq_enable();

    sendTimer.interval = 500;
    sendTimer.mode = SYS_TIMER_PERIODIC_MODE;
    sendTimer.handler = sendTimerHandler;
    SYS_TimerStart(&sendTimer);

    /* Replace with your application code */
    while (1) 
    {
        SYS_TaskHandler();
    }
}

