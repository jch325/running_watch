/*
 * gps_test.c
 *
 * Created: 2014/02/24
 * Author: Joel Heck
 *
 * Test cases for the GPS and UART interfaces
 *
 */ 
#define F_CPU 16000000
#define t1 1000

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include "oled.h"
#include "i2c.h"
#include "gps.h"

volatile uint16_t time1;

//timer 0 compare ISR
ISR (TIMER0_COMPA_vect)
{
	//Update the task time
	if (time1>0)	--time1;
}

int main(void) {
	//init the task timer
	time1 = t1;
	//set up timer 0 for 1 mSec timebase
	OCR0A = 249;  		//set the compare re to 250 time ticks
	TIMSK0 = (1<<OCIE0A);	//turn on timer 0 cmp match ISR
	//set prescalar to divide by 64
	TCCR0B = 3; //0b00000011;
	// turn on clear-on-match
	TCCR0A = (1<<WGM01) ;
	
	
	// check reset state
	DDRA = 0x08;
	PORTA = 0x00;
	_delay_ms(500);
	PORTA = 0x08;
	_delay_ms(500);
	
	i2c_init();
	oled_init();
	init_gps();

	oled_send_display();

	_delay_ms(1000);
	
	oled_clear_display_buffer();
	oled_send_display();
	
	sei();
	
	uint8_t x_coord = 0;
	uint8_t y_coord = 0;
	while(1) {
		
		if (time1 == 0) {
			time1 = t1;
			//oled_send_display();
			//oled_clear_display_buffer();
			//uint8_t status = update_gps();
			if (update_gps()) {
				oled_clear_display_buffer();
				
				x_coord = 0;
				y_coord = 0;
				uint8_t buffer_size = 8;
				char buffer[buffer_size];
				//uint8_t hour = get_hour();
				//uint8_t minute = get_minute();
				int8_t speed = get_speed();
				int16_t heading = get_altitude();
				
				sprintf(buffer, "%d:%d", speed, heading);
				for (uint8_t index = 0; index < buffer_size; index++) {
					oled_draw_char(buffer[index], x_coord, y_coord, SMALL);
					x_coord += 6;
				}
				x_coord = 0;
				y_coord = 16;
				
				double lat = get_latitude();
				
				sprintf(buffer, "%.3f", lat);
				for (uint8_t index = 0; index < buffer_size; index++) {
					oled_draw_char(buffer[index], x_coord, y_coord, MEDIUM);
					x_coord += 11;
				}
				
				x_coord = 0;
				y_coord = 32;
				
				double lon = get_longitude();
				sprintf(buffer, "%.3f", lon);
				for (uint8_t index = 0; index < buffer_size; index++) {
					oled_draw_char(buffer[index], x_coord, y_coord, MEDIUM);
					x_coord += 11;
				}
				oled_send_display();
			}
		}
	}
}