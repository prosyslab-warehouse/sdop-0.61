/*************************************************
*          sdop - Simple DocBook Processor       *
*************************************************/

/* Copyright (c) Philip Hazel, 2008 */

/* This module contains code for scanning the input document and
handling "revisionflag" attributes. We know that everything is nested
correctly. */

#include "sdop.h"


/*************************************************
*        Scan items and check revisionflag       *
*************************************************/

/* This function scans the input document and handles the revisionflag
attribute, which is a common attribute that is allowed on all elements. We only
support "changed" at present - the check for other values happens with the
general check for supported elements/attributes.

Argument:      the first item in the list to be processed
Returns:       TRUE to continue processing; FALSE to stop
               (currently always TRUE)
*/

BOOL
revision_check(item *item_list)
{
item *i, *ii;
paramstr *p;

DEBUG(D_any) debug_printf("Processing revisionflag attributes\n");

for (i = item_list; i != NULL; i = i->next)
  {
  if (i->name[0] == '/' || i->name[0] == '#') continue;
  if ((p = misc_param_find(i, US"revisionflag")) == NULL) continue;

  if (Ustrcmp(p->value, "changed") == 0)
    {
    for (ii = i->next; ii != i->partner; ii = ii->next)
      {
      if (Ustrcmp(ii->name, "#PCDATA") == 0)
        {
        ii->p.txtblk->pin_flags |= PIN_REVCH;
        }
      }
    }
  }

return TRUE;
}

/* End of revision.c */
