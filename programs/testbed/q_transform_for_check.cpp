#include "mathlib.h"

int main(int argc, char *argv[])
{
	rvector sourcex = {{1., 0., 0.}};
	rvector sourcey = {{0., 1., 0.}};
	rvector sourcez = {{0., 0., 1.}};
	rvector targetx;
	rvector targety;
	rvector targetz;

	targetx = rotate_q(q_change_around_rv({{1.,1.,-1.}}, RADOF(45.)), sourcex);
	targety = rotate_q(q_change_around_rv({{1.,1.,-1.}}, RADOF(45.)), sourcey);
	targetz = rotate_q(q_change_around_rv({{1.,1.,-1.}}, RADOF(45.)), sourcez);

	quaternion tq = q_transform_for(sourcex, sourcey, targetx, targety);

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
