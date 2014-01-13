/*
 * uart.c
 *
 * Created 2013/12/22
 * Author: Joel Heck
 * 
 * Adapted from code "gps.c" created by Brian on 30 Mar 2012 for a 4760
 * project
 *
 * Defines functions used for GPS data collection over UART
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include "uart.h"
#include "gps.h"

// Current index in data buffer being sent
static uint8_t send_index;
// Length of data buffer
static uint8_t buffer_size;
// Buffer for sending data
static char* send_buffer;

/*
 *
 */
ISR(USART_RX_vect) {
	char c;
	c = UDR;

	uart_data_rx(c);
}

/*
 *
 */
ISR(USART_UDRE_vect) {
	if (buffer_size > send_index) {
		UDR = send_buffer[send_index];
		send_index++;
	} else {
		buffer_size = 0;
		// Check variable names**************
		UCSR1B &= ~(1 << UDRIE);
	}
}

/*
 *
 */
void uart_init(){
	send_index = 0;
	buffer_size = 0;

	// Turn on the UART RX and TX
	UCSR1B |= (1 << RXEN) | (1 << TXEN);
	// Use 8-bit char size, no parity, one stop bit (8N1)
	UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);

	// Load upper 8-bits of the baud rate value into high byte of UBRR register
	UBRR1H = (BAUD_PRESCALE >> 8);
	// Load lower 8-bits of the baud rate value into low byte of UBRR register
	UBRR1L = BAUD_PRESCALE;

	// Enable the USART Recieve Complete interrupt (USART_RXC)
	UCSR1B |= (1 << RXCIE);
	// Disable sending
	UCSR1B &= ~(1 << UDRIE);
}

/*
 *
 */
uint8_t uart_send(char* buffer, uint8_t size){
	// Don't send if already sending data
	if (buffer_size)
		return 1;
	else {
		buffer_size = size;
		send_index = 0;
		send_buffer = buffer;
		// Enable UART interrupt (Data Register Empty Interrupt will execute)
		UCSR1B |= (1 << UDRIE);

		return 0;
	}
}