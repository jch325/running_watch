/* 
 * oled.c
 *
 * Created: 2014/01/27
 * Author: Joel Heck
 *
 * SSD1306 OLED display driver communication library
 * 
 * See Driver Guide for buffer byte mapping to display pixels
 */

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "i2c.h"
#include "oled.h"

// Buffer to hold color (black or white) of each pixel in display
//static uint8_t display_buffer[DISPLAY_BUFFER_SIZE] = {0x00};

// Buffer to hold color (black or white) of each pixel in display
static uint8_t display_buffer[DISPLAY_BUFFER_SIZE] = {
	/* Page 1 */
	0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00, 0x00, 0x14, 0x00,
	0x00, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 2 */
	0xF8, 0xF8, 0x06, 0x06, 0x86, 0x86, 0x66, 0x66, 0xF8, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00,
	0x00, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 3 */
	0x1F, 0x1F, 0x66, 0x66, 0x61, 0x61, 0x60, 0x60, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00,
	0x00, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 4 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00,
	0x00, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 5 */
	0xC0, 0xC0, 0xC0, 0xC0, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
	0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 6 */
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0x3C, 0x3C, 0x3C, 0x3C,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 7 */
	0xFF, 0xFF, 0xFF, 0xFF, 0x3C, 0x3C, 0x3C, 0x3C, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	/* Page 8 */
	0x03, 0x03, 0x03, 0x03, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C, 0x3C,
	0x03, 0x03, 0x03, 0x03, 0x00, 0x45, 0x3E, 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
	0xFF, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

/* Returns the byte in the buffer containing the desired pixel */
static uint16_t get_buffer_index(coordinate x_coord, coordinate y_coord) {
	uint16_t index = x_coord;
	index += (y_coord / 8) * OLED_WIDTH;

	return index;
}

/* Calculates a modulo of two numbers more efficiently than the mod operator */
static uint8_t modulus(coordinate argument, uint8_t divisor) {
	uint8_t multiple = (uint8_t) argument / divisor;
	uint8_t remainder = (uint8_t) argument - (multiple * divisor);

	return remainder;
}

/* Send a command byte */
static void oled_send_command(uint8_t command) {
	i2c_start(OLED_I2C_ADDRESS);
	
	// Indicate that a command follows
	i2c_write(COMMAND_TYPE);

	// Send the command
	i2c_write(command);
	i2c_stop();
}

/* Set a pixel white or black */
void oled_set_pixel(coordinate x_coord, coordinate y_coord, color pixel_color) {
	// Don't set pixels outside display area
	if ( (OLED_WIDTH <= x_coord) || (OLED_HEIGHT <= y_coord) ) {
		return;
	}

	uint16_t buffer_index = get_buffer_index(x_coord, y_coord);

	// Set pixel color in display buffer
	if (pixel_color == WHITE) {
		display_buffer[buffer_index] |= (1 << (modulus(y_coord, 8)) );
	}
	else {
		display_buffer[buffer_index] &= ~(1 << (modulus(y_coord, 8)) );
	}
}

/* Write the buffer to the display */
void oled_send_display() {
	
	// Start cursor at top, left pixel before sending data
	oled_send_command(OLED_SET_PAGE_START);
	oled_send_command(OLED_LOW_COLUMN);
	oled_send_command(OLED_HIGH_COLUMN);
	
	i2c_start(OLED_I2C_ADDRESS);

	// Indicate that data follows
	i2c_write(DATA_TYPE);

	// Send each byte in the display buffer
	uint16_t buffer_index = 0;
	while (buffer_index < DISPLAY_BUFFER_SIZE) {
		i2c_write(display_buffer[buffer_index]);
		buffer_index++;	
	}

	i2c_stop();
}

/* Clear display buffer */
void oled_clear_display_buffer() {
	memset(display_buffer, 0, DISPLAY_BUFFER_SIZE);
}

/* Initialize display with a reset and startup commands in given order */
void oled_init() {
	// Reset the display
	RESET_PORT &= ~(1 << RESET_PIN);
	//PORTA &= ~(1 << 3);
	_delay_ms(10.0);
	RESET_PORT |= (1 << RESET_PIN);
	//PORTA |= (1 << 3);
	
	oled_send_command(OLED_DISPLAY_OFF);
	
	/* Set by default during reset
	oled_send_command(OLED_SET_DISPLAY_CLK_DIV);
	oled_send_command(OLED_DISPLAY_CLK_DIV);
	oled_send_command(OLED_SET_MULTIPLEX);
	oled_send_command(OLED_MULTIPLEX_RATIO);
	oled_send_command(OLED_SET_DISPLAY_OFFSET);
	oled_send_command(OLED_DISPLAY_OFFSET);
	oled_send_command(OLED_START_LINE);
	*/
	
	oled_send_command(OLED_SET_CHARGE_PUMP);
	oled_send_command(OLED_CHARGE_PUMP);	
	
	// Set the memory addressing mode to horizontal with full display
	oled_send_command(OLED_SET_MEMORY_MODE);
	oled_send_command(OLED_HORIZ_MODE);
	oled_send_command(OLED_SET_PAGE);
	oled_send_command(OLED_PAGE_START);
	oled_send_command(OLED_PAGE_END);
	oled_send_command(OLED_SET_COLUMN);
	oled_send_command(OLED_COLUMN_START);
	oled_send_command(OLED_COLUMN_END);
	
	// Reverse column and row mapping 
	oled_send_command(OLED_SEG_REMAP_INV);
	oled_send_command(OLED_COM_SCAN_DIR_INV);
	
	/* Set by default during reset
	oled_send_command(OLED_SET_COM_PINS);
	oled_send_command(OLED_COM_PINS);
	*/
	
	// Set operating characteristics
	oled_send_command(OLED_SET_CONTRAST);
	oled_send_command(OLED_CONTRAST_SETTING);	
	oled_send_command(OLED_SET_PRECHARGE_PER);
	oled_send_command(OLED_PRECHARGE_PER);	
	oled_send_command(OLED_SET_VCOM_DESELECT);
	oled_send_command(OLED_VCOM_DESELECT);
	
	// Turn display back on
	oled_send_command(OLED_DISPLAY_ALLOW_RESUME);
	oled_send_command(OLED_NORMAL_DISPLAY);
	oled_send_command(OLED_DISPLAY_ON);
}