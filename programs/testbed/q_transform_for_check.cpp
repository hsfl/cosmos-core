/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "math/mathlib.h"

int main(int argc, char *argv[])
{
	rvector sourcex = {{1., 0., 0.}};
	rvector sourcey = {{0., 1., 0.}};
	rvector sourcez = {{0., 0., 1.}};
	rvector targetx;
	rvector targety;
	rvector targetz;
	quaternion tq;

	targetx = {{0., 1., 0.}};
	targety = {{1., 0., 0.}};

	tq = q_transform_for(sourcex, sourcey, targetx, targety);

	targetx = rotate_q(q_change_around_rv({{1.,1.,-1.}}, RADOF(45.)), sourcex);
	targety = rotate_q(q_change_around_rv({{1.,1.,-1.}}, RADOF(45.)), sourcey);
	targetz = rotate_q(q_change_around_rv({{1.,1.,-1.}}, RADOF(45.)), sourcez);

	tq = q_transform_for(sourcex, sourcey, targetx, targety);

	printf("X: [ %f, %f, %f] to ", targetx.col[0], targetx.col[1], targetx.col[2]);
	targetx = transform_q(tq, sourcex);
	printf("[ %f, %f, %f]\n", targetx.col[0], targetx.col[1], targetx.col[2]);
	printf("Y: [ %f, %f, %f] to ", targety.col[0], targety.col[1], targety.col[2]);
	targety = transform_q(tq, sourcey);
	printf("[ %f, %f, %f]\n", targety.col[0], targety.col[1], targety.col[2]);
	printf("Z: [ %f, %f, %f] to ", targetz.col[0], targetz.col[1], targetz.col[2]);
	targetz = transform_q(tq, sourcez);
	printf("[ %f, %f, %f]\n", targetz.col[0], targetz.col[1], targetz.col[2]);
}
