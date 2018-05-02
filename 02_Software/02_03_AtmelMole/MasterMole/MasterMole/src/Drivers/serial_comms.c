/*
 * serial_comms.c
 *
 * Created: 4/29/2018 10:20:55 PM
 *  Author: user
 */ 
#include "serial_comms.h"

static uint8_t rx_buf_write_idx = 0;
static uint8_t rx_buf_read_idx = 0;
static char RX_BUF[BUF_SIZE];

static usart_rs232_options_t USART_SERIAL_OPTIONS = {
	.baudrate = USART_SERIAL_BAUDRATE,
	.charlength = USART_SERIAL_CHAR_LENGTH,
	.paritytype = USART_SERIAL_PARITY,
	.stopbits = USART_SERIAL_STOP_BIT
};

void serial_init(void){
	usart_init_rs232(USART_SERIAL, &USART_SERIAL_OPTIONS);
	serial_clear_rx_buffer();
}

void serial_clear_rx_buffer(void){
	rx_buf_write_idx = 0;
	rx_buf_read_idx = 0;
	for(int i = 0; i < BUF_SIZE; i++){
		RX_BUF[i] = 0;
	}
}

char serial_read_byte_from_rx_buffer(void){
	if(rx_buf_write_idx == rx_buf_read_idx){
		return '\0';
	}
	char ret_val = RX_BUF[rx_buf_read_idx];
	rx_buf_read_idx++;
	if(rx_buf_read_idx > BUF_SIZE){
		rx_buf_read_idx = 0;
	}
	return ret_val;
}

void serial_write_byte(char c){
	usart_serial_putchar(USART_SERIAL, c);
}


//returns number of bytes written to the buffer
int32_t serial_update_rx_buffer(){
	//get a byte
    uint8_t bytes_received = 0;
	uint8_t received_byte;
	usart_serial_getchar(USART_SERIAL, &received_byte);
	while(received_byte != '\0') {
        bytes_received++;
		RX_BUF[rx_buf_write_idx] = received_byte;
		rx_buf_write_idx++;
		if(rx_buf_write_idx > BUF_SIZE){
			rx_buf_write_idx = 0;
		}
		usart_serial_getchar(USART_SERIAL, &received_byte);
	}
	
	return bytes_received;
}

// Read a specified number of bytes to a buffer
uint8_t read_bytes_to_buffer(uint8_t* buf, uint8_t n){
    //get a byte
    uint8_t bytes_received = 0;
    uint8_t received_byte;
    for(bytes_received = 0; bytes_received < n; bytes_received++) {
        usart_serial_getchar(USART_SERIAL, &received_byte);
        buf[bytes_received] = received_byte;
    }

    return bytes_received;
}


void serial_write_packet(const uint8_t *data, size_t len){
	 usart_serial_write_packet(USART_SERIAL, data, len);
}