/*! \file stringlib.c
	\brief String handling library source file
*/

#include "stringlib.h"

//! \addtogroup stringlib_functions
//! @{

//! Parse a string into words
/*! Divide a string into words separated by white space and return an array of the
 * results.
	\param string Zero terminated character string.
	\param words Empty array for storage of substrings.
	\param wmax maximum number of words that can be stored in words array.
	\return Number of words
*/
uint16_t string_parse(char *string, char *words[], uint16_t wmax)
{
	uint16_t wcount, ccount, i;

	if (string == NULL)
		return 0;

	wcount = ccount = i = 0;
	while (string[i] == ' ' || string[i] == '\t')
		i++;

	if (string[i] == 0)
		return 0;

	while (string[i] != 0)
	{
		if (string[i] == ' ' || string[i] == '\t')
		{
			words[wcount][ccount] = 0;
			ccount = 0;
			wcount++;
			if (wcount == wmax-1)
				break;
			do
			{
				i++;
			} while (string[i] == ' ' || string[i] == '\t');
		}
		else
		{
			if (!ccount)
				words[wcount] = &string[i];
			ccount++;
			i++;
		}
	}
	if (ccount)
		wcount++;
	words[wcount] = (char *)NULL;
	return (wcount);
}

int string_cmp(const char *wild, const char *string)
{
	// Written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
	const char *cp = NULL, *mp = NULL;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
			return 0;
		}
		wild++;
		string++;
	}

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
				return 1;
			}
			mp = wild;
			cp = string+1;
        } else if ((*wild == *string) || (*wild == '?'))
        {
			wild++;
			string++;
		} else {
			wild = mp;
			string = cp++;
		}
	}

    while (*wild == '*')
    {
		wild++;
	}
	return !*wild;
}

//! @}
