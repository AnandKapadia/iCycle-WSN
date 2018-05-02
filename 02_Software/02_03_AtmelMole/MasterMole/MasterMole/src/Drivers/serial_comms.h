/*
 * serial_comms.h
 *
 * Created: 4/29/2018 10:21:15 PM
 *  Author: user
 */ 


#ifndef SERIAL_COMMS_H_
#define SERIAL_COMMS_H_

#include <stdbool.h>
#include <string.h>
#include "gpio.h"
#include "delay.h"
#include "serial.h"

//UART Defines
#define USART_SERIAL                     &USARTA1
#define USART_SERIAL_BAUDRATE            4800
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

#define BUF_SIZE 20

//initialize the serial interface
void serial_init(void);

//clear the internal serial buffer
void serial_clear_rx_buffer(void);

// Reads n bytes off the serial line into buf
uint8_t read_bytes_to_buffer(uint8_t* buf, uint8_t n);

//returns 1 if there has been a byte read
//returns 0 of no update to the buffer
//reads uart for any bytes to be read and adds them to the buffer
int32_t serial_update_rx_buffer(void);

//returns the next (unread) byte from the rx buffer
char serial_read_byte_from_rx_buffer(void);


//write c to uart
void serial_write_byte(char c);

//writes string to uart
void serial_write_packet(const uint8_t *data, size_t len);



#endif /* SERIAL_COMMS_H_ */