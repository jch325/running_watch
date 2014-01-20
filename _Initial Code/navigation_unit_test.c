/*
 * navigation_unit_test.c
 *
 * Created: 2014/01/18
 * Author: Joel Heck
 *
 * Navigation unit test functions
 * 
 * This file includes all the unit test functions for the functions found in navigation.c.
 * It includes a main function at the bottom to run these test functions.
 */

#include "navigation.h"

/*
 * Testing for the init_nav() and array_valid() functions
 */
uint16_t test_init_nav() {
	uint16_t result = 65535;	// 0xffff all TRUE

	// Check for detection of invalid arrays as inputs
	// Empty arrays
	if (empty_check()) {
		result ^= (1 << 0);
	}
	
	// Arrays with 1 or 2 values 
	if (one_check()) {
		result ^= (1 << 1);
	}
	if (two_check()) {
		result ^= (1 << 2);
	}
	
	// Check border case of array of length 3
	if (!three_check()) {
		result ^= (1 << 3);
	}

	// Check arrays with length 10, 20, 50, and 100
	if (!ten_check()) {
		result ^= (1 << 4);
	}
	if (!twenty_check()) {
		result ^= (1 << 5);
	}
	if (!fifty_check()) {
		result ^= (1 << 6);
	}
	if (!hundred_check()) {
		result ^= (1 << 7);
	}

	const uint8_t LENGTH = 25;
	waypoint **new_route = create_route(LENGTH);
	boolean output = init_nav(new_route, LENGTH);
	if (output) {
		
		// Print out first and last lat/long values
		printf("%f, %f; %f, %f\n", route[0]->latitude, route[0]->longitude, 
			route[LENGTH-1]->latitude, route[LENGTH-1]->longitude);

		// Check values of num_waypts, current_waypt, elapsed, total_distance
		if (get_num_waypts() != LENGTH) {
			result ^= (1 << 9);
		}
		if (get_current_waypt() != 0) {
			result ^= (1 << 10);
		}
		if (get_elapsed_time() != 0) {
			result ^= (1 << 11);
		}
		if (get_total_distance() != 0) {
			result ^= (1 << 12);
		}

		// Check size of current_location, prev_location
		if (sizeof(get_current_location()) != sizeof(waypoint *)) {
			result ^= (1 << 13);
		}
		if (sizeof(get_previous_location()) != sizeof(waypoint *)) {
			result ^= (1 << 14);
		}
	}

	printf("%d\n", result);
	return result;
}

waypoint (**create_route(uint8_t length)) {
	waypoint **route = (waypoint **) malloc(sizeof(waypoint *) * length);
	uint8_t index;
	for (index = 0; index < length; index++) {
		waypoint *new_waypt = (waypoint *) malloc(sizeof(waypoint));
		new_waypt->latitude = index;
		new_waypt->longitude = index * -1;
		route[index] = new_waypt;
	}

	return route;
}

boolean empty_check() {
	waypoint **new_route = create_route(0);
	boolean output = init_nav(new_route, 0);
	free(new_route);
	return output;
}

boolean one_check() {
	waypoint **new_route = create_route(1);
	boolean output = init_nav(new_route, 1);
	free(new_route);
	return output;
}

boolean two_check() {
	waypoint **new_route = create_route(2);
	boolean output = init_nav(new_route, 2);
	free(new_route);
	return output;
}

boolean three_check() {
	waypoint **new_route = create_route(3);
	boolean output = init_nav(new_route, 3);
	free(new_route);
	return output;
}

boolean ten_check() {
	waypoint **new_route = create_route(10);
	boolean output = init_nav(new_route, 10);
	free(new_route);
	return output;
}

boolean twenty_check() {
	waypoint **new_route = create_route(20);
	boolean output = init_nav(new_route, 20);
	free(new_route);
	return output;
}

boolean fifty_check() {
	waypoint **new_route = create_route(50);
	boolean output = init_nav(new_route, 50);
	free(new_route);
	return output;
}

boolean hundred_check() {
	waypoint **new_route = create_route(100);
	boolean output = init_nav(new_route, 100);
	free(new_route);
	return output;
}

//*************************************************************

/*
 * Testing for the dist_between_waypts() function
 */
boolean test_dist_between_waypts() {
	
}