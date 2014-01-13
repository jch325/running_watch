/*
 * motor.c
 *
 * Created 2013/12/22
 * Author: Joel Heck
 *
 * Defines functions used for vibration motor control
 */

/*
 *
 */
void init_motors() {
	DDR = ???;
	PORT = ???;
}

/*
 *
 */
void vibrate_left() {
	PORT |= (1 << LEFT_M);
	PORT &= ~(1 << RIGHT_M);
}

/*
 *
 */
void vibrate_right() {
	PORT |= (1 << RIGHT_M);
	PORT &= ~(1 << LEFT_M);
}

/*
 *
 */
void vibrate_both() {
	PORT |= (1 << LEFT_M);
	PORT |= (1 << RIGHT_M);
}

/*
 *
 */
void vibrate_off() {
	PORT &= ~(1 << LEFT_M);
	PORT &= ~(1 << RIGHT_M);
}