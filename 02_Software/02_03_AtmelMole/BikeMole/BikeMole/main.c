/*
 * CornerMole.c
 *
 * Created: 4/29/2018 3:54:53 PM
 * Author : user
 */ 

#include <asf.h>
#include <avr/io.h>
#include "sysTimer.h"
#include "delay.h"
#include "gpio.h"
//#include "msg_structs.h"
#include "moleRF.h"

static SYS_Timer_t sendTimer;
static bikeMessage_t bikeMessage;

static void sendTimerHandler(SYS_Timer_t *timer) {
    static uint8_t count = 0;
    bikeMessage.other = count++;
    sendPacket(BROADCAST_ADDR, &bikeMessage, sizeof(bikeMessage));
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

    // TODO Setup IMU
    
    // Setup reception
    NWK_OpenEndpoint(APP_ENDPOINT, receivePacket);

    // Initialize transmission packet
    bikeMessage.imuX = 'x';
    bikeMessage.imuY = 'y';
    bikeMessage.imuZ = 'z';
    bikeMessage.other = 0;
    
    
    // Initialize other drivers
        
    gpio_configure_pin(RX_LED, IOPORT_DIR_OUTPUT);
    gpio_configure_pin(TX_LED, IOPORT_DIR_OUTPUT);
    gpio_configure_pin(LEFT_LED, IOPORT_DIR_OUTPUT);
    gpio_configure_pin(RIGHT_LED, IOPORT_DIR_OUTPUT);

    gpio_set_pin_high(RX_LED);
    gpio_set_pin_high(TX_LED);
    gpio_set_pin_high(LEFT_LED);
    gpio_set_pin_high(RIGHT_LED);
    delay_ms(250);
    gpio_set_pin_low(RX_LED);
    gpio_set_pin_low(TX_LED);
    gpio_set_pin_low(LEFT_LED);
    gpio_set_pin_low(RIGHT_LED);
    
    delay_ms(1000);

    cpu_irq_enable();

    sendTimer.interval = 250;
    sendTimer.mode = SYS_TIMER_PERIODIC_MODE;
    sendTimer.handler = sendTimerHandler;
    SYS_TimerStart(&sendTimer);

    /* Replace with your application code */
    while (1) 
    {
        SYS_TaskHandler();
    }
}

