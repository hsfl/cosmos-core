#include "configCosmos.h"

////! JSON Name Space array.
//void *zalloc(size_t nmemb, size_t size)
//{
//	void *ptr;
//#ifdef TRACKMEM
//	int i, iretn;
//#endif

//	if ((ptr=calloc(nmemb,size)) == NULL)
//		return (NULL);

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
