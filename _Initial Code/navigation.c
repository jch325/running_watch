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

// Macros
#define ARRAY_LENGTH( x )	(sizeof(x)/sizeof(x[0]))

// Imported global variables and functions

// Exported global variables and functions

// Private variables and functions to file

typedef struct waypoint {
	double latitude;
	double longitude;
} waypoint;

// Classes, constant definitions, enums, type definitions
enum direction {
	STRAIGHT,
	LEFT,
	RIGHT,
	NONE
};

static waypoint **route;	// array of waypoint structures defining a running route
static uint8_t num_waypts_in_route;	// number of waypoints in the route
static uint8_t current_waypt_num;	// index in route of the next waypoint
static uint32_t elapsed_time;		// seconds since beginning of run
static waypoint *current_location;	// the current user location
static waypoint *prev_location;		// the last known user location

static uint16_t total_distance_run;
/*
 * This routine initializes private class variables and stores the input array in a class
 * variable. It performs basic error checking on the input and returns a value indicating 
 * success or failure.
 */
boolean init_nav(waypoint *new_route) {
	// Check that input is valid
	if (array_valid(new_route)) {
		// Save input into private array
		memcpy(route, new_route, sizeof(new_route));
		
		// TODO: Initialize private class variables
		num_waypts_in_route = ARRAY_LENGTH(route);
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
 * This routine checks that the input array has more than two values, and is not NULL.
 * Returns a value indicating valid or not.
 */
boolean array_valid(waypoint *test_route) {
	// Check if array not NULL
	if (test_route != NULL) {
		
		// Check if array has more than two values
		if (ARRAY_LENGTH(test_route) > 2) {
			return TRUE;
		}
	}
	
	return FALSE;
}

/*
 * This routine calculates the equirectangular approximation of the distance between two 
 * (latitude, longitude) waypoints in meters. This approximation should work for 
 * distances of at least 100 km.
 */
uint16_t dist_between_waypts(waypoint *first_waypt, waypoint *second_waypt) {
	// Convert input latitudes/longitudes to radians
	double longitude_difference = (second_waypt.longitude - first_waypt.longitude) 
		* (double) DEG_TO_RAD;

	double latitude_difference = (second_waypt.latitude - first_waypt.latitude)
		* (double) DEG_TO_RAD;

	double latitude_average = ( (second_waypt.latitude + first_waypt.latitude) 
		* (double) DEG_TO_RAD ) / 2.0;

	double x_coord = longitude_difference * cos(latitude_average);
	double y_coord = latitude_difference;
	
	// Convert to integer as double precision not necessary
	uint16_t distance = (uint16_t) ( sqrt((x * x) + (y * y)) * EARTH_RADIUS);

	return distance;
}

/*
 * This routine updates the total distance completed and distance remaining to the next
 * waypoint based on the distance traveled since the last routine call. It also updates
 * the previous location of the user with the current location.
 */
uint16_t update_distance() {
	uint16_t distance_covered = dist_between_waypts(prev_location, current_location);
	
	// If the distance traveled in the last second is reasonable
	if (distance_covered <= REASONABLE_DISTANCE) {
		
		// Add the distance to the total distance for the run
		total_distance_run += distance_covered;
		
		// Find the distance to the next waypoint
		return dist_between_waypts(current_location, route[current_waypt_num]);
	}

	// Do nothing otherwise
}

/*
 * This routine indicates whether or not the user is within a certain distance of the next
 * waypoint, but not at the waypoint.
 */
boolean near_waypoint(uint16_t distance) {
	if (distance <= (uint16_t) NOTIFY_DISTANCE) {
		if (distance > (uint16_t) CHANGE_DISTANCE) {
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * This routine indicates whether or not the user is at the next waypoint. This means
 * that the user is within a smaller distance than being near the waypoint.
 */
boolean at_waypoint(uint16_t distance) {
	if (distance <= (uint16_t) CHANGE_DISTANCE) {
		return TRUE;
	}

	return FALSE;
}

/*
 * This is the main routine of the class and runs the user route navigation between 
 * waypoints. It is called roughly every second to update the user about the next
 * waypoint and keep track of the route state.
 */
uint8_t navigate_route() {
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
			
			uint16_t distance_to_waypt = update_distance();
				
			
			// Vibrate motors if near next waypt, at a turn, or off course
			// Update waypoints if at the next waypoint
			// If near next waypt
			if (near_waypoint(distance_to_waypt)) {
				// Vibrate motor in direction of following waypoint
				if (current_waypt_num < num_waypts_in_route) {
					// Find bearing to the next waypoint
					int16_t waypt_bearing = bearing_to_waypt(current_location, 
						route[current_waypt_num]);

					int16_t user_heading = get_heading();

					// Find the direction for user to turn
					direction turn_direction = direction_to_turn(user_heading, waypt_bearing);

					indicate_turn_direction(turn_direction);
				}
				// If next waypoint does not exist (end of route)
					// Do nothing
				// Vibrate motors to indicate direction of waypoint
			}
			// If at the next waypt
			else if (at_waypoint(distance_to_waypt)) {
				// If route not finished
					// Vibrate motor in direction of following waypoint
					// Find direction of next waypoint (left, right, straight)
					// Vibrate motors to indicate direction of waypoint
					// Update waypoint index
				// Else run complete
			}
			// If off course
			else if (off_course()) {
				// Vibrate motor in direction of waypoint
			}


			// Update the display
			
			// Stop vibrating motors
			
			// Save the current latitude and longitude
			memcpy(prev_location, current_location, sizeof(waypoint));
		}
		// Else 
			// Vibrate motors
			// Store statistics on first iteration
			// Update the display with finishing message
	}
	// Else
		// Update the time of the run
	
	// Update display or just update the time
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