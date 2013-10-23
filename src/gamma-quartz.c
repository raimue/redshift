/* gamma-quartz.c -- Quartz Display Services gamma adjustment source
   This file is part of Redshift.

   Redshift is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Redshift is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Redshift.  If not, see <http://www.gnu.org/licenses/>.

   Copyright (c) 2013  Geoffrey Thomas <geofft@ldprelaod.com>
*/

#include <stdio.h>
#include <stdlib.h>

#include <CoreGraphics/CGDirectDisplay.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(s) gettext(s)
#else
# define _(s) s
#endif

#include "gamma-quartz.h"
#include "colorramp.h"

#define GAMMA_RAMP_SIZE  256


int
quartz_init(quartz_state_t *state)
{
	return 0;
}

int
quartz_start(quartz_state_t *state)
{
	state->display = kCGDirectMainDisplay;
	state->capacity = CGDisplayGammaTableCapacity(state->display);
	state->savedRedTable = malloc(sizeof(CGGammaValue) * state->capacity);
	state->savedGreenTable = malloc(sizeof(CGGammaValue) * state->capacity);
	state->savedBlueTable = malloc(sizeof(CGGammaValue) * state->capacity);
	uint32_t sampleCount;
	CGGetDisplayTransferByTable(state->display,
	                            state->capacity,
	                            state->savedRedTable,
	                            state->savedGreenTable,
	                            state->savedBlueTable,
	                            &sampleCount);
	return 0;
}

void
quartz_free(quartz_state_t *state)
{
	free(state->savedRedTable);
	free(state->savedGreenTable);
	free(state->savedBlueTable);
}


void
quartz_print_help(FILE *f)
{
	fputs(_("Adjust gamma ramps with Quartz Display Services.\n"), f);
	fputs("\n", f);
}

int
quartz_set_option(quartz_state_t *state, const char *key, const char *value)
{
	return -1;
}

void
quartz_restore(quartz_state_t *state)
{
	CGSetDisplayTransferByTable(state->display,
	                            state->capacity,
	                            state->savedRedTable,
	                            state->savedGreenTable,
	                            state->savedBlueTable);
}

int
quartz_set_temperature(quartz_state_t *state, int temp, float brightness,
		       float gamma[3])
{
	float coeff[3];

	CGGammaValue * redTable = malloc(sizeof(CGGammaValue) * state->capacity);
	CGGammaValue * greenTable = malloc(sizeof(CGGammaValue) * state->capacity);
	CGGammaValue * blueTable = malloc(sizeof(CGGammaValue) * state->capacity);

	get_coefficient(coeff, temp, brightness);

	for (int i = 0; i < state->capacity; i++) {
		redTable[i]   = state->savedRedTable[i] * coeff[0];
		greenTable[i] = state->savedGreenTable[i] * coeff[1];
		blueTable[i]  = state->savedBlueTable[i] * coeff[2];
	}

	CGSetDisplayTransferByTable(state->display,
	                            state->capacity,
	                            redTable,
	                            greenTable,
	                            blueTable);
	/*
	CGSetDisplayTransferByFormula(state->display,
	                              0, coeff[0], 1./gamma[0],
	                              0, coeff[1], 1./gamma[1],
	                              0, coeff[2], 1./gamma[2]);
	 */

	return 0;
}
