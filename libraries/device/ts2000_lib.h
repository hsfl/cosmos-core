/**
* Contains definitions and functions for running a TS2000
* controller.
* @file ts2000_lib.h
* @brief TS2000 controller includes
*/

#ifndef _TS2000LIB_H
#define _TS2000LIB_H 1

#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "mathlib.h"
#include "cssl_lib.h"
#include "cosmos-errno.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TS2000_ID 1
#define TS2000_BAUD 9600
#define TS2000_BITS 8
#define TS2000_PARITY 0
#define TS2000_STOPBITS 1


/**
* Internal structure containing the state of the TS2000.
* @brief TS2000 state
*/
typedef struct
	{
	double frequency[2];
	} ts2000_state;

int ts2000_connect(char *dev);
int ts2000_disconnect();
double ts2000_get_frequency(int channel);
double ts2000_set_frequency(int channel, double frequency);

#ifdef __cplusplus
}
#endif

#endif
