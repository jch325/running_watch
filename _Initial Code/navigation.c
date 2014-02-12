/*
 * navigation.c
 *
 * Created: 2013/12/24
 * Author: Joel Heck
 *
 * Navigation function code
 * 
 * The navigation function guides the user between two latitude/longitude positions.
 * It retrieves formatted GPS data, makes some distance and bearing calculations, and
 * vibrates the motor(s) if necessary to indicate the correct heading to the next 
 * position.
 */

// #include files
#include "navigation.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Macros
#define ARRAY_LENGTH( x )	(sizeof(x)/sizeof(x[0]))

// Imported global variables and functions

// Exported global variables and functions

// Private variables and functions to file

static waypoint **route;	// array of waypoint structures defining a running route
static unsigned char num_waypts_in_route;	// number of waypoints in the route
static unsigned char current_waypt_num;	// index in route of the next waypoint
static unsigned long elapsed_time;		// seconds since beginning of run
static waypoint *current_location;	// the current user location
static waypoint *prev_location;		// the last known user location

static unsigned int total_distance_run;

/********************************************************/
//REMOVE THIS CODE AFTER TEST
/*waypoint** get_route() {
	return route;
}

unsigned char get_num_waypts() {
	return num_waypts_in_route;
}

unsigned char get_current_waypt() {
	return current_waypt_num;
}

unsigned long get_elapsed_time() {
	return elapsed_time;
}

waypoint* get_current_location() {
	return current_location;
}

waypoint* get_previous_location() {
	return prev_location;
}

unsigned int get_total_distance() {
	return total_distance_run;
}*/
/********************************************************/

/*
 * This routine checks that the input array has more than two values, and is not NULL.
 * Returns a value indicating valid or not.
 */
boolean array_valid(waypoint **test_route, unsigned char route_length) {
	// Check if array not NULL
	if (test_route != NULL) {

		// Check that array has more than two values
		if (route_length > 2) {
			return TRUE;
		}

	}

	return FALSE;
}

/*
 * This routine initializes private class variables and stores the input array in a class
 * variable. It performs basic error checking on the input and returns a value indicating 
 * success or failure.
 */
boolean init_nav(waypoint **new_route, unsigned char route_length) {
	// Check that input is valid
	// TODO: put maximum size on array? (put check elsewhere?)
	if (array_valid(new_route, route_length)) {
		// Save input into private array
		memcpy(&route, &new_route, sizeof(new_route));
		
		// Initialize private class variables
		num_waypts_in_route = route_length;
		current_waypt_num = 0;
		elapsed_time = 0;
		total_distance_run = 0;

		current_location = (waypoint *) malloc(sizeof(waypoint));
		prev_location = (waypoint *) malloc(sizeof(waypoint));

		// Return success
		return TRUE;
	}

	// If not valid, return failure
	return FALSE;
}



/*
 * This routine calculates the equirectangular approximation of the distance between two 
 * (latitude, longitude) waypoints in meters. This approximation should work for 
 * distances of at least 100 km.
 */
unsigned int dist_between_waypts(waypoint *first_waypt, waypoint *second_waypt) {
	// Convert input latitudes/longitudes to radians
	double delta_longitude = (second_waypt->longitude - first_waypt->longitude)
		* (double) DEG_TO_RAD;

	double delta_latitude = (second_waypt->latitude - first_waypt->latitude)
		* (double) DEG_TO_RAD;

	double latitude_average = ( (second_waypt->latitude + first_waypt->latitude)
		* (double) DEG_TO_RAD ) / 2.0;

	double x_coord = delta_longitude * cos(latitude_average);
	double y_coord = delta_latitude;
	
	// Convert to integer as double precision not necessary
	unsigned int distance = (unsigned int) ( sqrt((x_coord * x_coord) + (y_coord * y_coord)) * EARTH_RADIUS);

	return distance;
}

/*
 * This routine updates the total distance completed and distance remaining to the next
 * waypoint based on the distance traveled since the last routine call. It also updates
 * the previous location of the user with the current location.
 */
unsigned int update_distance() {
	unsigned int distance_covered = dist_between_waypts(prev_location, current_location);
	
	// If the distance traveled in the last second is reasonable
	if (distance_covered <= REASONABLE_DISTANCE) {
		
		// Add the distance to the total distance for the run
		total_distance_run += distance_covered;
		
		// Find the distance to the next waypoint
		return dist_between_waypts(current_location, route[current_waypt_num]);
	}

	return 0;
	// Do nothing otherwise
}

/*
 * This routine indicates whether or not the user is within a certain distance of the next
 * waypoint, but not at the waypoint.
 */
boolean near_waypoint(unsigned int distance) {
	if (distance <= (unsigned int) NOTIFY_DISTANCE) {
		if ((unsigned int) CHANGE_DISTANCE < distance) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * This routine indicates whether or not the user is at the next waypoint. This means
 * that the user is within a smaller distance than being near the waypoint.
 */
boolean at_waypoint(unsigned int distance) {
	if (distance <= (unsigned int) CHANGE_DISTANCE) {
		return TRUE;
	}

	return FALSE;
}

/*
 * This routine finds the bearing from one latitude/longitude location to another in
 * degrees. The first argument is the starting location and the second is the destination.
 */
int bearing_to_waypt(waypoint *first_waypt, waypoint *second_waypt) {
	double delta_longitude = (second_waypt->longitude - first_waypt->longitude) * DEG_TO_RAD;
	double delta_y = sin(delta_longitude) * cos(second_waypt->latitude * DEG_TO_RAD);
	
	double first_lat = first_waypt->latitude * DEG_TO_RAD;
	double second_lat = second_waypt->latitude * DEG_TO_RAD;
	double delta_x = ( cos(first_lat) * sin(second_lat) ) - ( sin(first_lat) * 
		cos(second_lat) * cos(delta_longitude) );

	int bearing = (int) ( atan2( delta_y, delta_x ) * RAD_TO_DEG);

	// Account for negative results
	if (bearing >= 0) {
		return bearing;
	}
	else {
		return 360 + bearing;
	}
}

/*
 * This routine finds the direction to turn for the user's heading to intersect the 
 * intended bearing. 
 */
direction direction_to_turn(int heading, int bearing) {
	// Check that inputs are in proper range [0-360]
	if ( (heading < 0 || 360 < heading) || (bearing < 0 || 360 < bearing) ) {
		return ERROR;
	}

 	// Calculate the difference between inputs
	int diff = bearing - heading;
	int abs_diff = (diff < 0) ? -diff : diff;

	// Only go left/right if difference large enough
	if (MIN_DIFF < abs_diff) {
		if (abs_diff <= 180) {
			
			// Go left if bearing less than heading and right otherwise
			return (diff < 0) ? LEFT : RIGHT;
		}

		// Difference > 180 degrees, only go left/right if difference large enough
		else if (bearing > heading) {
			
			if (MIN_DIFF < ( (heading + 360) - bearing)) {
				return LEFT;
			}
		}
		else if (bearing < heading) {
			
			if (MIN_DIFF < ( (bearing + 360) - heading)) {
				return RIGHT;
			}
		}
	}

	// Go straight if difference small enough
	return STRAIGHT;
}

/*
 * This routine vibrates the motors for user feedback based on the input argument.
 */
void vibrate_motors(direction turn_direction) {
	switch (turn_direction) {
		case STRAIGHT:
			vibrate_both();
			break;
		case LEFT:
			vibrate_left();
			break;
		case RIGHT:
			vibrate_right();
			break;
		case ERROR:
			vibrate_off();
			break;
		default:
			vibrate_off();
			break;
	}
}

/*
 * This routine finds the user's heading and the next waypoint's bearing, determines which
 * direction to turn to get on the bearing, and vibrates the motors to tell the user which
 * direction to turn.
 */
void indicate_turn_direction() {
	// Vibrate motor in direction of next waypoint if it exists
	if (current_waypt_num < num_waypts_in_route) {
		
		// Find bearing to the next waypoint and user heading
		int waypt_bearing = bearing_to_waypt(current_location,
			route[current_waypt_num + 1]);
		int user_heading = get_heading();

		// Find the direction for user to turn
		direction turn_direction = direction_to_turn(user_heading, waypt_bearing);
		vibrate_motors(turn_direction);
	}
}

/*
 * This routine alerts the user to the direction of the current waypoint if the user
 * wanders off the bearing to the current waypoint.
 */
void off_course_correction() {
	// Find bearing to the next waypoint and user heading
	int waypt_bearing = bearing_to_waypt(current_location,
		route[current_waypt_num]);
	int user_heading = get_heading();

	// Find the direction for user to turn
	direction turn_direction = direction_to_turn(user_heading, waypt_bearing);

	// Don't vibrate if not off course
	if (turn_direction != STRAIGHT) {
		vibrate_motors(turn_direction);
	}
}

/*
 * This routine saves certain run statistics, like total distance and time, to the EEPROM
 * for non-volatile storage.
 */
void eeprom_save_stats() {
	// Read saved accumulated run time
	double accumulated_time = eeprom_read_float( (float *) EEPROM_TIME);
	// Convert current run time from seconds to hours and add to accumulated run time
	accumulated_time += ( ((double) elapsed_time) / SEC_TO_HR);
	// Save updated accumulated run time to EEPROM
	eeprom_write_float( (float *) EEPROM_TIME, accumulated_time);
	// Read saved accumulated run distance 
	double accumulated_dist = eeprom_read_float( (float *) EEPROM_DIST);
	// Convert current run distance from meters to km and add to accumulated run distance
	accumulated_dist += ( ((double) total_distance_run) / M_TO_KM);
	// Save updated accumulated run distance to EEPROM
	eeprom_write_float( (float *) EEPROM_DIST, accumulated_dist);
}

/*
 * This is the main routine of the class and runs the user route navigation between 
 * waypoints. It is called roughly every second to update the user about the next
 * waypoint and keep track of the route state.
 */
unsigned char navigate_route() {
	// Retrieve latest GPS data
	// If data valid
	if (update_gps()) {
		
		// If not end of route
		if (current_waypt_num <= num_waypts_in_route) {
			
			// Update the time of the run
			elapsed_time++;

			// Update the distance to next waypoint
			current_location->latitude = get_latitude();
			current_location->longitude = get_longitude();
			unsigned int distance_to_waypt = update_distance();
				
			// If near next waypt, tell user direction to next waypt
			if (near_waypoint(distance_to_waypt)) {
				indicate_turn_direction();				
			}

			// If at the next waypt, tell user direction to next waypt and increment waypt
			else if (at_waypoint(distance_to_waypt)) {
				indicate_turn_direction();
				current_waypt_num++;
			}
			
			// If user not close to intended bearing, tell user direction to the bearing
			else {
				off_course_correction();
			}

			// TODO: Update the display
			
			// Stop vibrating motors?
			vibrate_off();

			// Save the current latitude and longitude
			memcpy(prev_location, current_location, sizeof(waypoint));
		}	// End if not end of route

		else { 
			// Vibrate motors
			vibrate_both();

			// Store statistics on first iteration to EEPROM
			eeprom_save_stats();

			// TODO: Update the display with finishing message
		}
	}	// End if update_gps

	// Only update the time if GPS data invalid
	else {
		elapsed_time++;
	}

	return 0;
	// TODO: Update display or just update the time
}



/* Routine construction
 * Give: info routine will hide, inputs, outputs, preconditions, postconditions
 * Name routine
 * Decide on test procedures
 *
 * Write header and high-level pseudocode
 * Think about the data types
 * Turn pseudocode into comments
 * Write the routine declaration and first and last statements
 * Fill in code below comments
 * Check code and clean up
 */
