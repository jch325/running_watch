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

// Defines the max number of data character in a GPS sentence that can be 
// stored in the receive buffer
#define MAX_STRING_SIZE 200
// Command to turn on RMC and GGA output from GPS module
#define GGA_RMC_OUTPUT "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";
// 1Hz update rate
#define UPDATE_1HZ "$PMTK220,1000*1F\r\n";
// Conversion knots to miles per hour
#define KTS_TO_MPH 1.151
// Conversion knots to kilometers per hour
#define KTS_TO_KPH 1.852

// The different states of data collection
enum data_rx_state {
	RECEIVED,
	NOT_RECEIVED,
	GGA,
	RMC
};

// Move parsed data into a "non-volatile" data structure (add lock)
uint8_t update_gps(void);
// Initialization sequence
void init_gps(void);
// Receive data from UART
void uart_data_rx(char);

//************* Retrieval functions ***********************
uint32_t get_elapsed_time(void);
uint8_t get_hour(void);
uint8_t get_minute(void);
char* get_date(void);

int8_t get_speed(void);
int16_t get_heading(void);

double get_latitude(void);
double get_longitude(void);
int16_t get_altitude(void);

uint8_t is_fix_valid(void);

#endif // GPS_H_