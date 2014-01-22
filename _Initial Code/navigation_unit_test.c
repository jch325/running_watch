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
	// Test with waypoints with both positive and negative lat/long, 100 meters apart
	// Positive latitudes and longitudes (48.1426, 11.5293; 48.1428, 11.5279)
	if (!close_enough_check(48.1426, 11.5293, 48.1428, 11.5279, 100)) {
		printf("Failed pos/pos.\n");
		return FALSE;
	}

	// Positive latitudes, negative longitudes (35.7797,-78.6423; 35.7798, -78.6434)
	if (!close_enough_check(35.7797, -78.6423, 35.7798, -78.6434, 100)) {
		printf("Failed pos/neg.\n");
		return FALSE;
	}

	// Negative latitudes and longitudes (-16.1293, -130.2804; -16.1294, -130.2794)
	if (!close_enough_check(-16.1293, -130.2804, -16.1294, -130.2794, 100)) {
		printf("Failed neg/neg.\n");
		return FALSE;
	}

	// Negative latitude, positive longitudes (-26.7649, 140.6969; -26.7643, 140.6977)
	if (!close_enough_check(-26.7649, 140.6969, -26.7643, 140.6977, 100)) {
		printf("Failed neg/pos.\n");
		return FALSE;
	}

	// Test with waypoints roughly 10, 100, 1000, 10000, and 100000 meters apart
	// 10 meters (34.0742,17.9331; 34.0742, 17.9332)
	if (!close_enough_check(34.0742, 17.9331, 34.0742, 17.9332, 10)) {
		printf("Failed 10 meters.\n");
		return FALSE;
	}

	// 100 meters (34.0742,17.9331;34.0744,17.9342)
	if (!close_enough_check(34.0742, 17.9331, 34.0744, 17.9342, 100)) {
		printf("Failed 100 meters.\n");
		return FALSE;
	}

	// 1000 meters (34.0741,17.9331;34.0760,17.9438)
	if (!close_enough_check(34.0741, 17.9331, 34.0760, 17.9438, 1000)) {
		printf("Failed 1000 meters.\n");
		return FALSE;
	}

	// 10000 meters (34.0728,17.9240;34.0908,18.0308)
	if (!close_enough_check(34.0728, 17.9240, 34.0908, 18.0308, 10000)) {
		printf("Failed 10000 meters.\n");
		return FALSE;
	}

	// 100000 meters (34.0663,17.8900;34.2441,18.9649)
	if (!close_enough_check(34.0663, 17.8900, 34.2441, 18.9649, 100000)) {
		printf("Failed 100000 meters.\n");
		return FALSE;
	}

	// Test with same waypoint (0 meters) (34.0663,17.8900)
	if (!close_enough_check(34.0663, 17.8900, 34.0663, 17.8900, 0)) {
		printf("Failed 0 meters.\n");
		return FALSE;
	}

	// Test cases along equator and prime meridian
	// Equator case (0.0056,-30.3756;-0.0037,-30.3749 - 1028 meters)
	if (!close_enough_check(0.0056, -30.3756, -0.0037, -30.3749, 1028)) {
		printf("Failed equator.\n");
		return FALSE;
	}

	// Prime meridian case (-0.6461,-0.0023;-.6468,0.0067 - 1008 meters)
	if (!close_enough_check(-0.6461, -0.0023, -.6468, 0.0067, 1008)) {
		printf("Failed meridian.\n");
		return FALSE;
	}

	// Test at 15, 30, 45, and 60 degrees N of equator (200 meters)
	// 15 degrees (15.1130,-92.8127; 15.1117,-92.8114)
	if (!close_enough_check(15.1130, -92.8127, 15.1117, -92.8114, 200)) {
		printf("Failed 15 deg.\n");
		return FALSE;
	}

	// 30 degrees (30.0451,-94.6209; 30.0451,-94.6188)
	if (!close_enough_check(30.0451, -94.6209, 30.0451, -94.6188, 200)) {
		printf("Failed 30 deg.\n");
		return FALSE;
	}

	// 45 degrees (45.0072,-94.8822;45.0071,-94.8797)
	if (!close_enough_check(45.0072, -94.8822, 45.0071, -94.8797, 200)) {
		printf("Failed 45 deg.\n");
		return FALSE;
	}

	// 60 degrees (60.0005,-95.0040;60.0006,-95.0004)
	if (!close_enough_check(60.0005, -95.0040, 60.0006, -95.0004, 200)) {
		printf("Failed 60 deg.\n");
		return FALSE;
	}

	return TRUE;
}

boolean close_enough_check(double first_lat, double first_long, double second_lat,
	double second_long, uint16_t expected_dist) {
	
	waypoint first_waypt;
	first_waypt.latitude = first_lat;
	first_waypt.longitude = first_long;
	waypoint second_waypt;
	second_waypt.latitude = second_lat;
	second_waypt.longitude = second_long;

	uint16_t actual_dist = dist_between_waypts(&first_waypt, &second_waypt);

	// Error tolerance of +/- 1%
	double max_error = (double) expected_dist / 100.0;
	double difference = (double) (expected_dist - actual_dist);
	if ((-max_error) < difference && difference < max_error) {
		return TRUE;
	}

	return FALSE;
}

//*************************************************************
/*
 * Testing for the near_waypoint() function
 */
boolean test_near_waypoint() {
	// 0 meters, should return false
	if (near_waypoint(0)) {
		printf("Failed 0 meters\n");
		return FALSE;
	}

	// 10 meters, should return false
	if (near_waypoint(10)) {
		printf("Failed 10 meters\n");
		return FALSE;
	}
	
	// 19 meters, should return false
	if (near_waypoint(19)) {
		printf("Failed 19 meters\n");
		return FALSE;
	}
	
	// 20 meters, should return false
	if (near_waypoint(20)) {
		printf("Failed 20 meters\n");
		return FALSE;
	}
	
	// 21 meters, should return true
	if (!near_waypoint(21)) {
		printf("Failed 21 meters\n");
		return FALSE;
	}
	
	// 30 meters, should return true
	if (!near_waypoint(30)) {
		printf("Failed 30 meters\n");
		return FALSE;
	}
	
	// 39 meters, should return true
	if (!near_waypoint(39)) {
		printf("Failed 39 meters\n");
		return FALSE;
	}
	
	// 40 meters, should return true
	if (!near_waypoint(40)) {
		printf("Failed 40 meters\n");
		return FALSE;
	}
	
	// 41 meters, should return false
	if (near_waypoint(41)) {
		printf("Failed 41 meters\n");
		return FALSE;
	}
	
	// 50 meters, should return false
	if (near_waypoint(50)) {
		printf("Failed 50 meters\n");
		return FALSE;
	}
	
	// 100 meters, should return false
	if (near_waypoint(100)) {
		printf("Failed 100 meters\n");
		return FALSE;
	}

	return TRUE;
}

//*************************************************************
/*
 * Testing for the at_waypoint() function
 */
boolean test_at_waypoint() {
	// 0 meters, should return true
	if (!near_waypoint(0)) {
		printf("Failed 0 meters\n");
		return FALSE;
	}

	// 10 meters, should return true
	if (!near_waypoint(10)) {
		printf("Failed 10 meters\n");
		return FALSE;
	}
	
	// 19 meters, should return true
	if (!near_waypoint(19)) {
		printf("Failed 19 meters\n");
		return FALSE;
	}
	
	// 20 meters, should return true
	if (!near_waypoint(20)) {
		printf("Failed 20 meters\n");
		return FALSE;
	}
	
	// 21 meters, should return false
	if (near_waypoint(21)) {
		printf("Failed 21 meters\n");
		return FALSE;
	}
	
	// 30 meters, should return false
	if (near_waypoint(30)) {
		printf("Failed 30 meters\n");
		return FALSE;
	}
	
	// 100 meters, should return false
	if (near_waypoint(100)) {
		printf("Failed 100 meters\n");
		return FALSE;
	}

	return TRUE;
}

//*************************************************************
/*
 * Testing for the bearing_to_waypt() function
 */
boolean test_bearing_to_waypt() {
	// All tests for waypoints 1000 meters apart
	// Test 0, 60, 120, 180, 240, 300 degree bearings
	// 0 degrees (30.5066, -95.9516; 30.5157, -95.9516)
	if (!bearing_close_enough_check(30.5066, -95.9516, 30.5157, -95.9516, 0)) {
		printf("Failed 0 deg.\n");
		return FALSE;
	}

	// 60 degrees (30.5071, -95.9517; 30.5116, -95.9425)
	if (!bearing_close_enough_check(30.5071, -95.9517, 30.5116, -95.9425, 60)) {
		printf("Failed 60 deg.\n");
		return FALSE;
	}
	
	// 120 degrees (30.5071, -95.9517; 30.5026, -95.9427)
	if (!bearing_close_enough_check(30.5071, -95.9517, 30.5026, -95.9427, 120)) {
		printf("Failed 120 deg.\n");
		return FALSE;
	}
	
	// 180 degrees (30.5157, -95.9516; 30.5066, -95.9516)
	if (!bearing_close_enough_check(30.5157, -95.9516; 30.5066, -95.9516, 180)) {
		printf("Failed 180 deg.\n");
		return FALSE;
	}
	
	// 240 degrees (30.5116, -95.9425; 30.5071, -95.9517)
	if (!bearing_close_enough_check(30.5116, -95.9425, 30.5071, -95.9517, 240)) {
		printf("Failed 240 deg.\n");
		return FALSE;
	}
	
	// 300 degrees (30.5026, -95.9427; 30.5071, -95.9517)
	if (!bearing_close_enough_check(30.5026, -95.9427, 30.5071, -95.9517, 300)) {
		printf("Failed 300 deg.\n");
		return FALSE;
	}
	
	// Test 33 degree bearing for:
	// Positive latitudes and longitudes (6.2229, 156.9511; 6.2305, 156.9562)
	if (!bearing_close_enough_check(6.2229, 156.9511, 6.2305, 156.9562, 33)) {
		printf("Failed pos/pos.\n");
		return FALSE;
	}
	
	// Positive latitudes and negative longitudes (30.5071, -95.9516; 30.5147, -95.9458)
	if (!bearing_close_enough_check(30.5071, -95.9516, 30.5147, -95.9458, 33)) {
		printf("Failed pos/neg.\n");
		return FALSE;
	}
	
	// Negative latitudes and longitudes (-5.1340, -157.0007; -5.1264, -156.9957)
	if (!bearing_close_enough_check(-5.1340, -157.0007, -5.1264, -156.9957, 33)) {
		printf("Failed neg/neg.\n");
		return FALSE;
	}
	
	// Negative latitudes and positive longitudes (-11.5785, 173.1210; -11.5708, 173.1261)
	if (!bearing_close_enough_check(-11.5785, 173.1210, -11.5708, 173.1261, 33)) {
		printf("Failed neg/pos.\n");
		return FALSE;
	}
	
	return TRUE;
}

boolean bearing_close_enough_check(double first_lat, double first_long, 
	double second_lat, double second_long, int16_t expected_bear) {
	
	waypoint first_waypt;
	first_waypt.latitude = first_lat;
	first_waypt.longitude = first_long;
	waypoint second_waypt;
	second_waypt.latitude = second_lat;
	second_waypt.longitude = second_long;

	int16_t actual_bear = bearing_to_waypt(&first_waypt, &second_waypt);

	// Error tolerance of +/- 3 degrees
	int16_t max_error = 3;
	int16_t difference = (expected_dist - actual_dist);
	if ((-max_error) < difference && difference < max_error) {
		return TRUE;
	}

	return FALSE;
}

//*************************************************************
/*
 * Testing for the direction_to_turn() function
 */
boolean test_direction_to_turn() {
	direction output;

	// Check all four error cases
	output = direction_to_turn(-1, 10);
	if (output != ERROR) {
		printf("Failed on neg heading.\n");
		return FALSE;
	}
	output = direction_to_turn(2, -10);
	if (output != ERROR) {
		printf("Failed on neg bearing.\n");
		return FALSE;
	}
	output = direction_to_turn(361, 10);
	if (output != ERROR) {
		printf("Failed on high heading.\n");
		return FALSE;
	}
	output = direction_to_turn(2, 361);
	if (output != ERROR) {
		printf("Failed on high bearing.\n");
		return FALSE;
	}

	// Check normal straight case
	output = direction_to_turn(30, 30 + MIN_DIFF - 1);
	if (output != STRAIGHT) {
		printf("Failed on normal straight.\n");
		return FALSE;
	}

	// Check normal left case
	output = direction_to_turn(60, 60 - MIN_DIFF - 1);
	if (output != LEFT) {
		printf("Failed on normal left.\n");
		return FALSE;
	}

	// Check normal right case
	output = direction_to_turn(30, 30 + MIN_DIFF + 1);
	if (output != RIGHT) {
		printf("Failed on neormal right.\n");
		return FALSE;
	}

	// Check straight case across 0 degree mark
	output = direction_to_turn(350, 10);
	if (output != STRAIGHT) {
		printf("Failed on 0 straight.\n");
		return FALSE;
	}

	// Check left case across 0 degree mark
	output = direction_to_turn(20, 300);
	if (output != LEFT) {
		printf("Failed on 0 left.\n");
		return FALSE;
	}

	// Check right case across 0 degree mark
	output = direction_to_turn(270, 30);
	if (output != RIGHT) {
		printf("Failed on 0 right.\n");
		return FALSE;
	}

	return TRUE;
}

//*************************************************************
/*
 * main() function for the testing functions
 */
int main() {
	int num_tests = 6;
	int num_failures = 0;

	if (!test_init_nav()) {
		num_failures++;
	}
	if (!test_at_waypoint()) {
		num_failures++;
	}
	if (!test_near_waypoint()) {
		num_failures++;
	}
	if (!test_direction_to_turn()) {
		num_failures++;
	}
	if (!test_bearing_to_waypt()) {
		num_failures++;
	}
	if (!test_dist_between_waypts()) {
		num_failures++;
	}

	if (num_failures == 0) {
		printf("Tests successful!\n");
	}
	else {
		printf("%d/%d tests failed.\n", num_failures, num_tests);
	}
	
	return 0;
}