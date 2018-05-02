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

// Master message to send to a bicycle
masterMessage_t masterMessage;

// UART pi packet struct
static uint8_t uartRxBuf[sizeof(uart_txPacket_t)];
static uart_txPacket_t uart_txPacket;
//uint8_t uartIsFresh = false;

static void sendTimerHandler(SYS_Timer_t *timer) {

	//sendPacket(0xffff, &payload, 1);
	//gpio_toggle_pin(STATUS_LED);
	
}

int main(void)
{
    uint8_t uartRx;

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

/*
    sendTimer.interval = 500;
    sendTimer.mode = SYS_TIMER_PERIODIC_MODE;
    sendTimer.handler = sendTimerHandler;
    SYS_TimerStart(&sendTimer);*/

    /* Replace with your application code */
    while (1) 
    {
        SYS_TaskHandler();

        // Receive a message from the pi

        // Check for a packet header
        read_bytes_to_buffer(&(uartRxBuf[0]), 1);
        if (uartRxBuf[0] == EXPECTED_PACKET_HEADER) {
            //Received a packet header, pull the rest of the packet into the buffer
            read_bytes_to_buffer(sizeof(uart_txPacket) - 1, &(uartRxBuf[1]));
            memcpy(&uart_txPacket, &uartRxBuf, sizeof(uart_txPacket_t));
            if (true) { // TODO check for trailer
                // Process packet from pi, send to bicycle
                masterMessage.masterID = MASTER_ADDR;
                masterMessage.location = uart_txPacket.bicycleRelativeToVehicleOrientation;
                sendPacket(uart_txPacket.destinationAddress.vehicleAddress, &masterMessage, sizeof(masterMessage_t));
            }
        }

		//serial_write_packet("HELLO FROM THE BOARD", strlen("HELLO FROM THE BOARD"));
    }
}

