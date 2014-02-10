/*
 * oled_test.c
 *
 * Created: 2014/02/03
 * Author: Joel Heck
 *
 * Test cases for the SSD1306 OLED display library
 *
 */ 
#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "oled.h"
#include "iic.h"

int main(void) {
	// check reset state
	DDRA = 0x08;
	PORTA = 0x00;
	DDRD = 0x04;	// Set low to turn on
	PORTD = 0x04;
	_delay_ms(500);
	PORTA = 0x08;
	_delay_ms(500);
	
	twi_init();
	oled_init();

	oled_send_display();

	_delay_ms(1000);
	
	/*oled_clear_display_buffer();
	oled_send_display();
	_delay_ms(1000);
	
	for (int i = 0; i < 64; i++) {
		oled_set_pixel(i,i,WHITE);
	}
	oled_set_pixel(10, 10, BLACK);
	oled_send_display();
	_delay_ms(1000);*/
	
	while(1) {
		// Do nothing
	}
}