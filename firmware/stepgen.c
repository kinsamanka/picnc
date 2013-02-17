/*    Copyright (C) 2013 GP Orcullo
 *
 *    This file is part of PiCnc.
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <p32xxxx.h>
#include <plib.h>

#include <string.h>

#include "hardware.h"
#include "stepgen.h"

#define STEPBIT		17
#define STEP_MASK	(1L<<(STEPBIT-1))
#define DIR_MASK	(1L<<31)

/*
  Timing diagram:
	     _______	     _______
  STEP	   _/       \_______/       \__
  	   _________		     __
  DIR	            \_______________/

  Step pulse has a 50% duty cyle.
  Direction signal changes on the falling edge of the step pulse.

*/

static void step_hi(int);
static void step_lo(int);
static void dir_hi(int);
static void dir_lo(int);

static volatile int32_t position[MAXGEN] = { 0 };

static int32_t oldpos[MAXGEN] = { 0 },
        oldvel[MAXGEN] = { 0 };

static volatile stepgen_input_struct stepgen_input = { {0} };

static int do_step_hi[MAXGEN] = {1, 1, 1, 1};

void stepgen_get_position(void *buf) {
	disable_int();
	memcpy(buf, (const void *)position, sizeof(position));
	enable_int();
}

void stepgen_update_input(const void *buf) {
	disable_int();
	memcpy((void *)&stepgen_input, buf, sizeof(stepgen_input));
	enable_int();
}

void stepgen_reset(void) {
	int i;

	disable_int();

	for (i = 0; i < MAXGEN; i++) {
		position[i] = 0;
		oldpos[i] = 0;
		oldvel[i] = 0;

		stepgen_input.velocity[i] = 0;

		do_step_hi[i] = 1;
	}

	enable_int();

	for (i = 0; i < MAXGEN; i++) {
		step_lo(i);
		dir_lo(i);
	}
}

void stepgen(void) {
	uint32_t stepready;
	int i;

	for (i = 0; i < MAXGEN; i++) {
		/* check if a step pulse can be generated */
		stepready = (position[i] ^ oldpos[i]) & STEP_MASK;

		/* generate a step pulse */
		if (stepready) {

			oldpos[i] = position[i];

			if (do_step_hi[i]) {
				do_step_hi[i] = 0;
				step_hi(i);
			} else {
				do_step_hi[i] = 1;
				step_lo(i);
			}
		}

		/* update position counter */
		position[i] += stepgen_input.velocity[i];

		/* generate direction pulse */
		if (stepready && do_step_hi[i]) {
			/* check for direction change */
			if ((stepgen_input.velocity[i] ^ oldvel[i]) & DIR_MASK) {

				oldvel[i] = stepgen_input.velocity[i];

				if (stepgen_input.velocity[i] > 0)
					dir_lo(i);
				if (stepgen_input.velocity[i] < 0)
					dir_hi(i);
			}
		}
	}
}

__inline__ void step_hi(int i) {

	if (i == 0)
		STEPHI_X;
	if (i == 1)
		STEPHI_Y;
	if (i == 2)
		STEPHI_Z;
	if (i == 3)
		STEPHI_A;
}

__inline__ void step_lo(int i) {

	if (i == 0)
		STEPLO_X;
	if (i == 1)
		STEPLO_Y;
	if (i == 2)
		STEPLO_Z;
	if (i == 3)
		STEPLO_A;
}

__inline__ void dir_hi(int i) {

	if (i == 0)
		DIR_HI_X;
	if (i == 1)
		DIR_HI_Y;
	if (i == 2)
		DIR_HI_Z;
	if (i == 3)
		DIR_HI_A;
}

__inline__ void dir_lo(int i) {

	if (i == 0)
		DIR_LO_X;
	if (i == 1)
		DIR_LO_Y;
	if (i == 2)
		DIR_LO_Z;
	if (i == 3)
		DIR_LO_A;
}
