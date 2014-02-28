/*
 * uart.h
 *
 * Created: 2013/12/22
 * Author: Joel Heck
 *
 * Header for UART functions
 *
 * Adapted from code "uart1.h" created by Brian on 30 Mar 2012 for a 4760
 * project
 *
 * Defines data structures and declares functions used for GPS data processing
 */

#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000
#define UART_BAUD 9600
#define BAUD_PRESCALE (((F_CPU / (UART_BAUD * 16UL))) - 1)

void uart_init(void);
uint8_t uart_send(char*, uint8_t);

#endif 	// UART_H_