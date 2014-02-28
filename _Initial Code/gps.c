/*
 * gps.c
 *
 * Created 2013/12/05
 * Author: Joel Heck
 * 
 * Adapted from code "gps.c" created by Brian on 30 Mar 2012 for a 4760
 * project
 *
 * Defines functions used for GPS data processing
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gps.h"
#include "uart.h"

// Format time
static void set_time(char*);
// Format latitude/longitude
static void set_lat_long(char*, uint8_t);
// Format date
static void set_date(char*);
// Parse collected data
static void parse_data(char*);

// Holds characters as they arrive from UART
static char rx_data[MAX_STRING_SIZE];
// Copy of rx_data once all characters received
static char rx_data_copy[MAX_STRING_SIZE];
// Index of array rx_data
static uint8_t rx_data_pos;
// Indicates whether or not data has been received
static volatile uint8_t data_received;
// Indicates which GPS string is being received
static volatile uint8_t gps_string_state;

// GPSData structure to store parsed GPS data
typedef struct {
	
    uint32_t gps_time;      // Current GPS time in seconds
    uint8_t hour;           // Local hour time
    uint8_t minute;         // Local minute time

    int8_t speed;           // Speed in MPH (from GPS)
    int16_t heading;        // True course in degrees (from GPS)
                            // Only good if speed >4 MPH    

    double latitude;        // Current latitude in degrees
    double longitude;       // Current longitude in degrees
    int16_t altitude;       // Current altitude in meters

    uint8_t day;			// Current day of the month
    uint8_t month;			// Current month
    uint16_t year;			// Current year

    // return?**************************
    double prev_lat;        // Current latitude in degrees
    double prev_lon;        // Current longitude in degrees

    uint8_t fix;
} gps_data;

// GPSData structure
static gps_data my_gps;

/*
 * Returns the current UTC time in seconds
 */
uint32_t get_gps_time() {
	return my_gps.gps_time;
}

/*
 * Returns the current UTC hour
 */
uint8_t get_hour() {
	return my_gps.hour;
}

/*
 * Returns the current UTC minute
 */
uint8_t get_minute() {
	return my_gps.minute;	
}

/*
 * Returns the current UTC day of month
 */
uint8_t get_day() {
	return my_gps.day;
}

/*
 * Returns the current UTC month
 */
uint8_t get_month() {
	return my_gps.month;
}

/*
 * Returns the current UTC year
 */
uint16_t get_year() {
	return my_gps.year;
}

/*
 * Returns the current speed
 */
int8_t get_speed() {
	return my_gps.speed;
}

/*
 * Returns the current heading
 */
int16_t get_heading() {
	return my_gps.heading;
}

/*
 * Returns the current latitude
 */
double get_latitude() {
	return my_gps.latitude;
}

/*
 * Returns the current longitude
 */
double get_longitude() {
	return my_gps.longitude;
}

/*
 * Returns the current altitude
 */
int16_t get_altitude(){
	return my_gps.altitude;
}

/*
 * Initialize the GPS and UART control software
 */
void init_gps() {
	
	// Initialize variables
	rx_data_pos = 0;
	data_received = NOT_RECEIVED;
	gps_string_state = INIT;
	
	// Initialize UART
	uart_init();
	
	// Set GPS module to output RMC and GGA data strings at 1 Hz
	uart_send(GGA_RMC_OUTPUT, strlen(GGA_RMC_OUTPUT));
	uart_send(UPDATE_1HZ, strlen(UPDATE_1HZ));
}

/*
 * Store the received character from the GPS module into the data array
 */
void uart_data_rx(char c) {
	
	// Only start storing data if first character is '$'
	if (c != '$' && rx_data_pos == 0) {
		return;
	}

	// Determine which sentence is being sent
	if (c == '$') {
		if (gps_string_state == GGA) {
			gps_string_state = RMC;
		}
		else {
			gps_string_state = GGA;
		}
	}

	// Copy data to another array and reset array index when all data received
	if (c == '\n' && gps_string_state == RMC) {
		rx_data[rx_data_pos] = 0;
		rx_data_pos = 0;
		if (data_received == NOT_RECEIVED) {
			memcpy(rx_data_copy, rx_data, MAX_STRING_SIZE);
			data_received = RECEIVED;
		}

		return;
	}

	// Store received character
	rx_data[rx_data_pos] = c;
	rx_data_pos++;		
}

/*
 * Tells whether or not the latest GPS data is valid
 *
 * return: 0 for invalid, 1 for valid
 */
uint8_t is_fix_valid(){
	if (my_gps.fix >= 1) {
		return VALID;
	}
	else {
		return INVALID;
	}
}

/*
 * Parse the last-received GPS data
 *
 * return: 0 for invalid data or no available data, 1 for valid data
 */
uint8_t update_gps() {
	if (data_received == RECEIVED) {
		parse_data(rx_data_copy);
		data_received = NOT_RECEIVED;

		return is_fix_valid();
	}

	return INVALID;
}

/*
 * Stores the received GPS time as integer seconds. Also stores the hour and
 * minute in separate variables. The GPS time is given in hhmmss.ss format 
 * and is in UTC time standard. Discards fractional seconds
 * 
 * data: array of integers as char types giving current UTC time
 */
static void set_time(char* data) {
	
	// Convert and store time as seconds
	// Hours
	uint32_t time = (uint32_t)( 10 * (data[0] - CHAR_TO_INT) + (data[1] - CHAR_TO_INT) ) * SEC_IN_HR;
	
	// Minutes
	time += (uint32_t)( 10 * (data[2] - CHAR_TO_INT) + (data[3] - CHAR_TO_INT) ) * SEC_IN_MIN;
	
	// Seconds
	time += (uint32_t)( 10 * (data[4] - CHAR_TO_INT) + (data[5] - CHAR_TO_INT) );
	my_gps.gps_time = time;

	// Convert and store hour and minute
	my_gps.hour = (uint8_t)( 10 * (data[0] - CHAR_TO_INT) + (data[1] - CHAR_TO_INT) );
	my_gps.minute = (uint8_t)( 10 * (data[2] - CHAR_TO_INT) + (data[3] - CHAR_TO_INT) );
}

/*
 * Converts and stores a latitude or longitude position stored in a char
 * array to a double. Received latitude or longitude is given in ddmm.mmm
 * format. The stored format is dd.ddddd (decimal degrees).
 * 
 * data: array of integers as char types giving latitude or longitude
 * dim: indicates whether input is a latitude or longitude 
 */
static void set_lat_long(char* data, uint8_t dimension) {
	double pos = 0.0;

	if (dimension == LATITUDE) {
		pos = (double)( 10 * (data[0] - CHAR_TO_INT) + (data[1] - CHAR_TO_INT) );

		pos += (double)( 10 * (data[2] - CHAR_TO_INT) + (data[3] - CHAR_TO_INT) +
			0.1 * (data[5] - CHAR_TO_INT) + 0.01 * (data[6] - CHAR_TO_INT) +
			0.001 * (data[7] - CHAR_TO_INT) ) / MIN_IN_DEG;
		
		my_gps.latitude = pos;
	} 
	else {
		pos = (double)( 100 * (data[0] - CHAR_TO_INT) + 10 * (data[1] - CHAR_TO_INT) + 
			(data[2] - CHAR_TO_INT) );

		pos += (double)( 10 * (data[3] - CHAR_TO_INT) + (data[4] - CHAR_TO_INT) + 
			0.1 * (data[6] - CHAR_TO_INT) + 0.01 * (data[7] - CHAR_TO_INT) + 
			0.001 * (data[8] - CHAR_TO_INT) ) / MIN_IN_DEG;

		my_gps.longitude = pos;
	}
}

/*
 * Converts and stores the current GPS date (for UTC). Received format is ddmmyy.
 */
static void set_date(char* data) {
	my_gps.day = (uint8_t)( 10 * (data[0] - CHAR_TO_INT) + (data[1] - CHAR_TO_INT) );
	my_gps.month = (uint8_t)( 10 * (data[2] - CHAR_TO_INT) + (data[3] - CHAR_TO_INT) );
	my_gps.year = 2000 + (uint16_t)( 10 * (data[4] - CHAR_TO_INT) + (data[5] - CHAR_TO_INT) );
}

/*
 * Parses an array of GPS data by comma delimiters and stores important sections
 */
static void parse_data(char* data) {
	
	// Copy section of array that starts with "$GPGGA" (should be whole array)
	char *p = strstr(data, "$GPGGA");

	if(p != NULL) {	

		// UTC time	
		p = strchr(p, ',') + 1;
		set_time(p);
		
		// Latitude
		p = strchr(p, ',') + 1; 
		set_lat_long(p, LATITUDE);
		
		// Latitude direction
		p = strchr(p, ',') + 1; 
		if (p[0] == 'S')
			my_gps.latitude *= -1;

		// Longitude
		p = strchr(p, ',') + 1; 
		set_lat_long(p, LONGITUDE);
		
		// Longitude direction
		p = strchr(p, ',') + 1; 
		if (p[0] == 'W')
			my_gps.longitude *= -1;

		// Fix quality
		p = strchr(p, ',') + 1; 
		my_gps.fix = p[0] - CHAR_TO_INT;

		// Number of satellites
		p = strchr(p, ',') + 1; 

		// HDOP
		p = strchr(p, ',') + 1; 

		// Altitude
		p = strchr(p, ',') + 1; 
		my_gps.altitude = atoi(p);

		// Altitude units
		p = strchr(p, ',') + 1; 

		// Geoidal separation
		p = strchr(p, ',') + 1; 

		// Geoidal separation units
		p = strchr(p, ',') + 1; 

		// Age of differental GPS data
		p = strchr(p, ',') + 1; 

		// Checksum
		p = strchr(p, '\n') + 1; 

		// RMC header
		p = strchr(p, ',') + 1; 

		// UTC time
		p = strchr(p, ',') + 1; 
		
		// Status ('A' = VALID, 'V' = INVALID)
		p = strchr(p, ',') + 1; 
		if (p[0] == 'V')	
			my_gps.fix = INVALID;

		// Latitude
		p = strchr(p, ',') + 1; 

		// Latitude direction
		p = strchr(p, ',') + 1; 

		// Longitude
		p = strchr(p, ',') + 1; 

		// Longitude direction
		p = strchr(p, ',') + 1; 

		// Speed
		p = strchr(p, ',') + 1; 
		my_gps.speed = (int8_t)(atof(p) * KTS_TO_KPH);

		// Heading
		p = strchr(p, ',') + 1;
		my_gps.heading = atoi(p);

		// Date
		p = strchr(p, ',') + 1; 
		set_date(p);

		// The magnetic variation and checksum are not used
	}
}
