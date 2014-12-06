/*! \file stringlib.h
	\brief stringlib include file
	A library providing functions for handling various types of string.
*/

//! \ingroup support
//!	\defgroup stringlib String handling library

#ifndef _STRINGLIB_H
#define _STRINGLIB_H 1

#include "configCosmos.h"

#include "mathlib.h"

//#include <sys/types.h>
#include <math.h>
#include <errno.h>

//! \ingroup stringlib
//! \defgroup stringlib_functions String handling functions
//! @{

uint16_t string_parse(char *string, char *word[], uint16_t size);
int string_cmp(const char *wild, const char *string);
//! @}

#endif
