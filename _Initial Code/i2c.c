/* 
 * i2c.c
 *
 * Created: 2014/01/26
 * Author: Joel Heck
 *
 * I2C/TWI communication library
 * 
 * Only for use as a Master Transmitter.
 */

#include <stdlib.h>
#include <avr/io.h>
#include "i2c.h"

/*
 * Sends byte in the data register onto the I2C/TWI bus and waits for an acknowledgement
 */
static void i2c_send() {
	TWCR = (1 << TWINT) | (1 << TWEN);

	// Wait for TWINT flag to acknowledge transmission
	while ( !(TWCR & (1 << TWINT)) ) {
		// Do nothing
	}
}

/*
 * Checks that a particular acknowledge flag is set (transmission successful)
 */
static boolean ack_received(uint16_t flag) {
	if ((TWSR & 0xF8) == flag) {
		return TRUE;
	}

	return FALSE;
}

/*
 * Initialize I2C/TWI registers
 */
void i2c_init() {
	// Set bit rate register
	TWBR = ( (F_CPU / SCLK) - 16) / 2;
	
	// Prescalar set to /1 by default
}

/*
 * Sends a START condition onto the I2C/TWI bus and sends the slave address
 */
boolean i2c_start(uint8_t address) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	
	// Wait for TWINT flag to acknowledge transmission
	while ( !(TWCR & (1 << TWINT)) ) {
		// Do nothing
	}

	// Send address if START transmitted
	if (ack_received(START)) {
		
		// Add WRITE bit to address (make LSB zero)
		address &= ~(1 << 1); 

		// Load address into data register
		TWDR = address;

		// Send address
		i2c_send();

		// Error check
		return ack_received(SLAW_ACK);
	}

	// Failed START transmission
	return FALSE;
}

/*
 * Write data to the slave device
 */
boolean i2c_write(uint8_t data) {
	
	// Load data into data register
	TWDR = data;

	// Send data
	i2c_send();

	// Error check
	return ack_received(DATA_ACK);
}

/*
 * Sends a STOP condition onto the I2C/TWI bus to end communication
 */
void i2c_stop() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1<< TWSTO);

	while ( TWCR & (1 << TWSTO) ) {
		//Do nothing
	}
}