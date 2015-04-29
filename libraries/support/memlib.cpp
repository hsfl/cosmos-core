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

#include "configCosmos.h"

////! JSON Name Space array.
//void *zalloc(size_t nmemb, size_t size)
//{
//	void *ptr;
//#ifdef TRACKMEM
//	int i, iretn;
//#endif

//	if ((ptr=calloc(nmemb,size)) == NULL)
//		return nullptr;

//#ifdef TRACKMEM

//	iretn = 0;
//	for (i=0; i<callocs.count; i++)
//	{
//		if (ptr >= callocs.ptr[i] && ptr < (char *)callocs.ptr[i]+callocs.size[i])
//		{
//			iretn = 1;
//			return ((void *)NULL);
//		}
//	}
//	callocs.ptr[callocs.count] = ptr;
//	callocs.size[callocs.count] = size*nmemb;
//	callocs.count ++;

//#endif
//	return (ptr);
//}

//void zfree(void *ptr)
//{
//#ifdef TRACKMEM
//	int i, j, iretn;

//	iretn = 0;
//	for (i=0; i<callocs.count; i++)
//	{
//		if (callocs.ptr[i] == ptr)
//		{
//			iretn = 1;
//			break;
//		}
//		if (ptr > callocs.ptr[i] && ptr < (char *)callocs.ptr[i]+callocs.size[i])
//		{
//			iretn = -1;
//			break;
//		}
//	}

//	if (iretn < 0)
//	{
//		callocs.count = i;
//		return;
//	}


//	for (j=i; j<callocs.count-1; j++)
//	{
//		callocs.ptr[j] = callocs.ptr[j+1];
//		callocs.size[j] = callocs.size[j+1];
//	}

//	callocs.count--;
//#endif
//	free(ptr);
//}
