/*
 * gps.h
 *
 * Created: 2013/12/05
 * Author: Joel Heck
 *
 * Header for GPS functions
 *
 * Adapted from code "gps.c" created by Brian on 25 Mar 2012 for a 4760
 * project
 *
 * Defines data structures and declares functions used for GPS data processing
 */

#ifndef GPS_H_
#define GPS_H_

// Command to turn on RMC and GGA output from GPS module
#define GGA_RMC_OUTPUT "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
// 1Hz update rate
#define UPDATE_1HZ "$PMTK220,1000*1F\r\n"

// Defines the max number of data character in a GPS sentence that can be 
// stored in the receive buffer
#define MAX_STRING_SIZE 200
// Conversion knots to miles per hour
#define KTS_TO_MPH 1.151
// Conversion knots to kilometers per hour
#define KTS_TO_KPH 1.852
// Conversion char to single-digit positive integer
#define CHAR_TO_INT 48

// Time conversion constants
#define SEC_IN_HR 3600
#define SEC_IN_MIN 60
// Degree conversion constant
#define MIN_IN_DEG 60


// Data collection states
enum data_rx_state {
	RECEIVED,
	NOT_RECEIVED,
};

// GPS data string types
enum gps_string_type {
	GGA,
	RMC,
	INIT	
};

// Data validity
enum valid {
	INVALID,
	VALID
};

// Coordinate type
enum lat_long {
	LATITUDE,
	LONGITUDE
};

// Move parsed data into a "non-volatile" data structure (add lock)
uint8_t update_gps(void);
// Initialization sequence
void init_gps(void);
// Receive data from UART
void uart_data_rx(char);

//************* Retrieval functions ***********************
uint32_t get_gps_time(void);
uint8_t get_hour(void);
uint8_t get_minute(void);
uint8_t get_day(void);
uint8_t get_month(void);
uint16_t get_year(void);

int8_t get_speed(void);
int16_t get_heading(void);

double get_latitude(void);
double get_longitude(void);
int16_t get_altitude(void);

uint8_t is_fix_valid(void);

#endif // GPS_H_