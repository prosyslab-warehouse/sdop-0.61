/*************************************************
*          sdop - Simple DocBook Processor       *
*************************************************/

/* Copyright (c) Philip Hazel, 2008 */

/* This module contains functions that are system-dependent. At the moment,
the only supported system is Unix. */

#include "sdop.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


/*************************************************
*        Check for the existence of a file       *
*************************************************/

BOOL
sys_exists(uschar *name)
{
struct stat statbuf;
return stat(CCS name, &statbuf) == 0;
}

/* End of sys.c */
