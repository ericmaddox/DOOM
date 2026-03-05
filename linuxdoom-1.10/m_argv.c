// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: m_argv.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";


#include <string.h>

int		myargc;
char**		myargv;




//
// M_CheckParm
// Checks for the given parameter
// in the program's command line arguments.
// Returns the argument number (1 to argc-1)
// or 0 if not present
int M_CheckParm (char *check)
{
    int		i;

    for (i = 1;i<myargc;i++)
    {
	if ( !myargv[i] )
	    continue;
	if ( !strcasecmp(check, myargv[i]) )
	    return i;
    }

    // didn't find it, so check for a - parameter
    if (check[0] == '-')
    {
	char        newstring[256];

	strncpy(newstring, check+1, sizeof(newstring) - 1);
	newstring[sizeof(newstring) - 1] = 0;

	for (i=1; i<myargc; i++)
	{
	    if ( !myargv[i] )
		continue;
	    if ( !strcasecmp (newstring, myargv[i]) )
		return i;
	}
    }

    return 0;
}
