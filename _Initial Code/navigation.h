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

#include "motor.h"

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



void init_nav(void);
void navigate_route(void);
void run_complete(void);
void run_motors(void);
void wait_for_gps(void);

uint16_t dist_between_waypts(double, double, double, double);
int16_t bearing_to_waypt(double, double, double, double);
int8_t dir_to_waypt(int16_t, int16_t, uint8_t);

//*********************************************************
static uint32_t elapsed_time;
static uint8_t stats_saved;
static uint16_t dist_rem;      // Distance to next waypt (meters)
static double dist_done;        // Current completed distance (kilometers)
static uint16_t dist_covered;	// Distance covered in last second

/*
 * Initialize certain navigation parameters with the selected route
 */
void init_nav(/*INSERT ROUTE SELECTION*/) {
 	elapsed_time = 0;
 	stats_saved = 0;
 	dist_done = 0;
 	dist_rem = 0;	/*INSERT DISTANCE B/T FIRST TWO WAYPOINTS*/
}

/*
 * Run through the navigation sequence (called once a second)
 */
void navigate_route() {
	
	if(update_gps()) {	// Parse latest data, valid data
		if (/*INSERT END OF ROUTE CHECK*/)
			run_complete();
		else {
			elapsed_time++;

			update_distances();
			//bear = bearing_to_waypt(/*INSERT WAYPTS*/);

			update_waypts();
		}
	} else {	// Invalid data
		elapsed_time++;
	}

	/*PRINT NEW INFORMATION*/
}

/*
 *
 */
static void update_waypts() {
	if (dist_rem < (uint16_t)NEAR_DIST) {
		/*UPDATE THE WAYPT INDEX*/
		if (/*WAYPT IS NOT LAST*/) {
			indicate_turn_dir();
		}
	}
}

/*
 *
 */
static void indicate_turn_dir(/*INSERT ARGS*/) {
 	/*CALCULATE BEARING
 	 *CALL dir_to_waypt()
 	 *CALL run_motors()*/
}


/*
 * Private function for handling completed runs
 */
void run_complete() {
	if(!stats_saved) {	// Save run statistics
		vibrate_both();		// Vibrate motors (INSERT end of run function?)
		stats_saved++;
		/*INSERT SAVE STATS TO EEPROM*/
		/*PRINT MSGS*/
	}

	vibrate_off();
}

/*
 *
 */
void run_motors(/*INSERT ARGS*/) {
	
}

/*
 *
 */
void wait_for_gps(/*INSERT ARGS*/) {
 	/*INSERT PRINT STATEMENTS*/
}


/*
 * Finds the bearing between two (latitude, longitude) positions in degrees
 *
 * lat: latitude in decimal degrees of first position
 * lon: longitude in decimal degrees of first position
 * next_lat: latitude in decimal degrees of second position
 * next_lon: longitude in decimal degrees of second position
 *
 * return: integer bearing from the first position to the second position
 * 		   in degrees
 */
int16_t bearing_to_waypt(double lat, double lon, double next_lat, double next_lon) {
	double dLong = next_lon-lon;
	double dy = sin(dLong*DEGRAD)*cos(next_lat*DEGRAD);
	double dx = cos(lat*DEGRAD)*sin(next_lat*DEGRAD)-
		sin(lat*DEGRAD)*cos(next_lat*DEGRAD)*cos(dLong*DEGRAD);
	int16_t result = (int16_t)(atan2(dy,dx)*57.3);	// 57.3 = 180/pi
	
	if (result > 0) 
		return result;
	else
		return 360+result;
}

/*
 * Finds the direction to turn to reach the active waypoint if off-track or 
 * transitioning to the next waypoint. Only gives an indication to turn if 
 * the user's bearing is more than 60 degrees from the expected bearing
 *
 * actual: the actual bearing of the user in integer degrees (0-360)
 * expected: the bearing from the user's current position to the active 
 * 		waypoint in integer degrees (0-360)
 * next: an integer (0 or 1) to indicate if transitioning to a new waypoint
 *
 * return: an integer giving the direction to turn (if any) or an error
 * 		-2 = input out of bounds, -1 = turn left, 0 = straight, 1 = turn right
 * 		2 = do nothing (on track)
 */
int8_t dir_to_waypt(int16_t actual, int16_t expected, uint8_t next) {
	if (actual > 360 || expected > 360 || actual < 0 || expected < 0)
		return -2;	// Error (input must be in range [0,360])

	int16_t diff = expected - actual;
	int16_t absD = (diff < 0) ? -diff : diff;

	if (absD > TURN_IND || next == 1) {
		if (absD == 0)
			return 0;
		else if (absD < 180)
			return (diff < 0) ? -1 : 1;
		else if (absD == 180)
			return -1;	// Arbitrarily chose turn left
		else if (expected > actual) {
			if (((actual+360)-expected) > TURN_IND)	
				return -1;
		}
		else {
			if (((expected+360)-actual) > TURN_IND)	
				return 1;
		}
	}

	return 2;
}