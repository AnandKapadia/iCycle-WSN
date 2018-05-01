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
#include "serial_comms.h"

static SYS_Timer_t sendTimer;
static uint8_t payload = 0x4;

static void sendTimerHandler(SYS_Timer_t *timer) {

	//sendPacket(0xffff, &payload, 1);
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
    NWK_OpenEndpoint(APP_ENDPOINT, receivePacket);

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

	serial_init();

    cpu_irq_enable();

    sendTimer.interval = 500;
    sendTimer.mode = SYS_TIMER_PERIODIC_MODE;
    sendTimer.handler = sendTimerHandler;
    SYS_TimerStart(&sendTimer);

    /* Replace with your application code */
    while (1) 
    {
        SYS_TaskHandler();
		serial_update_rx_buffer();
/*
		char c;
		while((c = serial_read_byte_from_rx_buffer()) != '\0'){
			serial_write_byte(c);
		}
        */

		//serial_write_packet("HELLO FROM THE BOARD", strlen("HELLO FROM THE BOARD"));
    }
}

