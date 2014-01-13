/*
 * motor.h
 *
 * Created: 2013/12/22
 * Author: Joel Heck
 *
 * Header for motor control functions
 *
 * Defines data structures and declares functions used for GPS data processing
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define LEFT_M 
#define RIGHT_M

// Add timer for pulsing if possible
// Vibrate end of run?

void init_motors(void);
void vibrate_left(void);
void vibrate_right(void);
void vibrate_both(void);
void vibrate_off(void);

#endif	// MOTOR_H_