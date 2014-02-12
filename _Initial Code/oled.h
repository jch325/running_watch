/* 
 * oled.h
 *
 * Created: 2014/01/31
 * Author: Joel Heck
 *
 * SSD1306 OLED display driver communication library header
 * 
 * 
 */

#ifndef OLED_H_
#define OLED_H_

#define OLED_HEIGHT 64	// Display height in pixels
#define OLED_WIDTH 128	// Display width in pixels
#define OLED_I2C_ADDRESS 0x7A	// b01111010

#define COMMAND_TYPE 0x00	// Control byte indicating command transmission
#define DATA_TYPE 0x40		// Control byte indicating data transmission

#define RESET_PIN 0x08 	// Reset pin on A.3
#define RESET_PORT PORTA

#define DISPLAY_BUFFER_SIZE (OLED_HEIGHT * OLED_WIDTH / 8)

// See page 28 in Driver Guide for details
#define OLED_SET_CONTRAST 0x81	// OLED_CONTRAST_SETTING must follow this
#define OLED_CONTRAST_SETTING 0xCF	// The contrast setting (0-255, reset 0x7F)

#define OLED_DISPLAY_ALLOW_RESUME 0XA4	// Display ON, follow RAM content (reset)
#define OLED_DISPLAY_IGNORE_RESUME 0xA5	// Display ON, ignore RAM content
#define OLED_NORMAL_DISPLAY 0XA6	// 1 in RAM = ON (reset)
#define OLED_DISPLAY_OFF 0xAE	// Sleep mode (reset)
#define OLED_DISPLAY_ON 0XAF	// Normal mode 

#define OLED_SET_MEMORY_MODE 0x20	// Command to set addressing mode

#define OLED_PAGE_MODE 0x02		// Page addressing
#define OLED_LOW_COLUMN 0x00	// Set low column
#define OLED_HIGH_COLUMN 0x10	// Set high column
#define OLED_SET_PAGE_START 0xB0	// Set page

#define OLED_HORIZ_MODE	0x00	// Horizontal addressing (follows above command) (reset = 0x02)
#define OLED_SET_COLUMN 0x21	// Set column start and end addresses
#define OLED_COLUMN_START 0x00	// Column start at 0 (reset = 0x00)
#define OLED_COLUMN_END 0xFF	// Column end at 127 (reset = 0xFF)
#define OLED_SET_PAGE 0x22		// Set page start and end
#define OLED_PAGE_START 0x00	// Page start at 0 (reset = 0x00)
#define OLED_PAGE_END 0x07		// Page end at 7 (reset = 0x07)

#define OLED_START_LINE 0x40	// Set RAM display start line register (reset = 0x40)

#define OLED_SEG_REMAP_NORM 0xA0	// Column 0 mapped to SEG0 (reset)
#define OLED_SEG_REMAP_INV 0xA1		// Column 127 mapped to SEG0

#define OLED_SET_MULTIPLEX 0xA8		// Set MUX setting
#define OLED_MULTIPLEX_RATIO 0x3F	// Multiplex setting 64MUX (reset = 0x3F)

#define OLED_COM_SCAN_DIR_NORM 0xC0		// Normal scan COM0 to COM63 (63 from MUX setting) (reset)
#define OLED_COM_SCAN_DIR_INV 0xC8	// Remapped scan COM63 to COM0 (63 from MUX setting)

#define OLED_SET_DISPLAY_OFFSET 0xD3	// Vertical shift from COM
#define OLED_DISPLAY_OFFSET 0x00	// No offset (reset = 0x00)

#define OLED_SET_COM_PINS 0xDA	// Set COM pin hardware configuration
#define OLED_COM_PINS 0x12		// Alternative COM pin configuration (reset = 0x12)

#define OLED_SET_DISPLAY_CLK_DIV 0xD5	// Display CLK divide ratio
#define OLED_DISPLAY_CLK_DIV 0x80		// (reset = 0x80)

#define OLED_SET_PRECHARGE_PER 0XD9		// Precharge period
#define OLED_PRECHARGE_PER 0xF1			// (reset = 0x22)

#define OLED_SET_VCOM_DESELECT 0XDB		// VCOM deselect level
#define OLED_VCOM_DESELECT 0x40		// VCC? (reset = 20)

#define OLED_SET_CHARGE_PUMP 0x8D
#define OLED_CHARGE_PUMP 0x14

// Pixel coordinate in display
typedef uint8_t coordinate;

// Pixel color
typedef enum color {
	BLACK,
	WHITE
} color;

void oled_set_pixel(coordinate, coordinate, color);
void oled_send_display(void);
void oled_init(void);
void oled_clear_display_buffer(void);

#endif // OLED_H_