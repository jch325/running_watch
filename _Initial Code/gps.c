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
#include "gps.h"

// Format received time
static void set_time(char*);
// Parse collected data
static void parse_data(char*);

// Holds characters as they arrive from UART
static char rx_data[MAX_STRING_SIZE];
// Copy of rx_data once all characters received
static char rx_data_copy[MAX_STRING_SIZE];
// Index of array rx_data
static uint8_t rx_data_pos;
// Indicates whether or not data is being received
static volatile uint8_t data_received;

// GPSData object to store GPS data
static typedef struct {
	
    uint32_t gps_time;      // Current GPS time in seconds
    uint8_t hour;           // Local hour time
    uint8_t minute;         // Local minute time

    int8_t speed;           // Speed in MPH (from GPS)
    int16_t heading;        // True course in degrees (from GPS)
                            // Only good if speed >4 MPH    

    double latitude;         // Current latitude in degrees
    double longitude;        // Current longitude in degrees
    int16_t altitude;       // Current altitude in meters

    // return?**************************
    double prev_lat;         // Current latitude in degrees
    double prev_lon;        // Current longitude in degrees

    uint8_t fix;
} gps_data;

//
static gps_data my_gps;


/*
 *
 */
void init_gps() {
	// Initialize variables
	// Initialize UART
	// Send data output commands to GPS module
	rx_data_pos = 0;
	data_received = NOT_RECEIVED;
}

/*
 *
 */
void uart_data_rx(char c) {
	// Store c in the array
	// When done with sentence, update some variables
	// Update array index
	
	// Only start storing data if first character is '$'
	if (c != '$' && rx_data_pos == 0)
		return;

	if (c == '$') {
		if (data_received == GGA)
			data_received = RMC;
		else
			data_received = GGA;
	}

	if (c == '\n' && data_received == RMC) {
		rx_data[rx_data_pos] = 0;
		rx_data_pos = 0;
		memcpy(rx_data_copy, rx_data, MAX_STRING_SIZE);
		data_received = RECEIVED;
		return;
	}

	rx_data[rx_data_pos++] = c;		
}

/*
 *
 */
uint8_t update_gps() {
	if (data_received == RECEIVED) {
		char* data = (char*) strtok((char*)rx_data_copy, ",\n");
		parse_data(data);
		data_received = NOT_RECEIVED;
		return is_fix_valid();
	}

	return 0;
}

/*
 *
 */
static void parse_data(char* data) {
	char* p = strstr(data, "$GPGGA");
	if(p != NULL) {
		p = strchr(p, ',') + 1; // UTC time
		set_time(p);

		p = strchr(p, ',') + 1; // Latitude
		set_lat_long(p, 0);
		p = strchr(p, ',') + 1; // Latitude direction
		if (p[0] == 'S')
			my_gps.latitude *= -1;

		p = strchr(p, ',') + 1; // Longitude
		set_lat_long(p, 1);
		p = strchr(p, ',') + 1; // Longitude direction
		if (p[0] == 'W')
			my_gps.longitude *= -1;

		p = strchr(p, ',') + 1; // Fix quality
		my_gps.fix = p[0] - 48;

		p = strchr(p, ',') + 1; // Number of satellites
		p = strchr(p, ',') + 1; // HDOP

		p = strchr(p, ',') + 1; // Altitude
		my_gps.altitude = atoi(p);

		p = strchr(p, ',') + 1; // Altitude units
		p = strchr(p, ',') + 1; // Geoidal separation
		p = strchr(p, ',') + 1; // Geoidal separation units
		p = strchr(p, ',') + 1; // Age of differental GPS data
		p = strchr(p, '\n') + 1; // Checksum

		p = strchr(p, ',') + 1; // RMC header
		p = strchr(p, ',') + 1; // UTC time
		
		p = strchr(p, ',') + 1; // Status
		if (p[0] == 'V')	// Invalid data
			my_gps.fix = 0;

		p = strchr(p, ',') + 1; // Latitude
		p = strchr(p, ',') + 1; // Latitude direction
		p = strchr(p, ',') + 1; // Longitude
		p = strchr(p, ',') + 1; // Longitude direction

		p = strchr(p, ',') + 1; // Speed
		my_gps.speed = (int8_t)(atof(p)*KTS_TO_KPH);

		p = strchr(p, ',') + 1; // Heading
		my_gps.heading = atoi(p);

		p = strchr(p, ',') + 1; // Date

		// The magnetic variation and checksum are not used
	}
}

/*
 * Change received GPS time to integer seconds. The GPS time is given in 
 * hhmmss.ss format and is in UTC time standard. Discards fractional seconds
 * 
 * data: array of integers giving current UTC time
 */
static void set_time(char* data) {
	uint32_t time = (uint32_t)((data[0]-48)*10+(data[1]-48))*3600;	// Hours
	time += (uint32_t)((data[2]-48)*10+(data[3]-48))*60;	// Minutes
	time += (uint32_t)((data[4]-48)*10+(data[5]-48));		// Seconds
	my_gps.gps_time = time;
	my_gps.hour = (uint8_t)((data[0]-48)*10+(data[1]-48));
	my_gps.minute = (uint8_t)((data[2]-48)*10+(data[3]-48));
}

/*
 * Converts a receiver latitude or longitude position stored in a uint8_t
 * array to a double. Received latitude or longitude is given in ddmm.mmm
 * format. The returned format is dd.ddddd (decimal degrees).
 * 
 * data: array of integers giving each digit of latitude or longitude
 * dim: indicates whether input is a latitude or longitude 
 * 		0 = latitude, 1 = longitude
 */
static void set_lat_long(char* data, uint8_t dim) {
	double pos = 0.0;

	if (dim == 0) {
		pos = (double)((data[0]-48)*10 + (data[1]-48));
		pos += (double)((data[2]-48)*10 + (data[3]-48) +
			(data[5]-48)*0.1 + (data[6]-48)*0.01 +
			(data[7]-48)*0.001)/60;
		my_gps.latitude = pos;
	} else {
		pos = (double)((data[0]-48)*100 + (data[1]-48)*10 + 
			(data[2]-48));
		pos += (double)((data[3]-48)*10 + (data[4]-48) + 
			(data[6]-48)*0.1 + (data[7]-48)*0.01 + 
			(data[8]-48)*0.001)/60;
		my_gps.longitude = pos;
	}
}

/*
 *
 */
uint32_t get_elapsed_time() {
	return my_gps.elapsed_time;
}

/*
 *
 */
uint8_t get_hour() {
	return my_gps.hour;
}

/*
 *
 */
uint8_t get_minute() {
	return my_gps.minute;	
}

/*
 *
 */
char* get_date() {
	// return local date
	return NULL;
}

/*
 *
 */
int8_t get_speed() {
	return my_gps.speed;
}

/*
 *
 */
int16_t get_heading() {
	return my_gps.heading;
}

/*
 *
 */
double get_latitude() {
	return my_gps.latitude;
}

/*
 *
 */
double get_longitude() {
	return my_gps.longitude;
}

/*
 *
 */
int16_t get_altitude(){
	return my_gps.altitude;
}

/*
 * Tells whether or not the latest GPS is valid
 *
 * return: 0 for invalid, 1 for valid
 */
uint8_t is_fix_valid(){
	if (my_gps.fix == 1)
		return 1;
	else
		return 0;
}