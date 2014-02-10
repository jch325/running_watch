/* 
 * iic.h
 *
 * Created: 2014/02/03
 * Author: Joel Heck
 *
 * I2C/TWI communication library
 * 
 * Only for use as a Master Transmitter.
 */

#define START 0x08		// START sent flag
#define SLAW_ACK 0x18	// SLA+W ACK received flag
#define SLAW_NACK 0x20		// SLA+W NACK received flag
#define DATA_ACK 0x28	// Data ACK received flag
#define DATA_NACK 0x30		// Data NACK received flag

// TODO CPU CLK = 11.0592 MHz
#define F_CPU 16000000	// CPU frequency
#define SCLK 4000000	// Serial clock frequency (100kHz-400kHz)

typedef enum boolean {
	FALSE,
	TRUE = !FALSE
} boolean;

void i2c_init(void);
boolean i2c_start(uint8_t);
boolean i2c_write(uint8_t);
void i2c_stop(void);
