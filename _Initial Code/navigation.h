/*
 * navigation.h
 *
 * Created: 2013/12/24
 * Author: Joel Heck
 *
 * Header for navigation code 
 * 
 * Navigation function code
 */

#ifndef NAVIGATION_H_
#define NAVIGATION_H_

//#include "motor.h"

#define DEG_TO_RAD 0.01745		// pi/180
#define RAD_TO_DEG 57.3 		// 180/pi
#define EARTH_RADIUS 6371000	// meters
#define REASONABLE_DISTANCE 100		// meters
#define CHANGE_DISTANCE 20 	// meters
#define NOTIFY_DISTANCE 40 	// meters
#define MIN_DIFF 30 		// degrees
#define NULL 0
#define EEPROM_TIME 0 	// Place in memory
#define EEPROM_DIST 8	// Place in memory
#define SEC_TO_HR 3600.0 	// seconds to hrs
#define M_TO_KM 1000.0 		// meters to kilometers


typedef enum boolean {
	TRUE = 1,
	FALSE = (!TRUE)
} boolean;

typedef struct waypoint {
	double latitude;
	double longitude;
} waypoint;

// Classes, constant definitions, enums, type definitions
typedef enum direction {
	LEFT,
	STRAIGHT,
	RIGHT,
	ERROR
} direction;

/********************************************************/
//REMOVE THIS CODE AFTER TEST
waypoint** get_route(void);

uint8_t get_num_waypts(void);

uint8_t get_current_waypt(void);

uint32_t get_elapsed_time(void);

waypoint* get_current_location(void);

waypoint* get_previous_location(void);

uint16_t get_total_distance(void);
/********************************************************/

boolean array_valid(waypoint**, uint8_t);
boolean init_nav(waypoint**, uint8_t);
uint16_t dist_between_waypts(waypoint*, waypoint*);
uint16_t update_distance(void);
boolean near_waypoint(uint16_t);
boolean at_waypoint(uint16_t);
int16_t bearing_to_waypt(waypoint*, waypoint*);
direction direction_to_turn(int16_t, int16_t);
//void vibrate_motors(direction);
//void indicate_turn_direction(void);
//void off_course_correction(void);
//void eeprom_save_stats(void);
//unsigned char navigate_route(void);

#endif // NAVIGATION_H_
