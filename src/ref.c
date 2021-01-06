/*************************************************
*          sdop - Simple DocBook Processor       *
*************************************************/

/* Copyright (c) Philip Hazel, 2008 */

/* This module contains code for resolving cross references. */


#include "sdop.h"




/*************************************************
*               Resolve references               *
*************************************************/

/* This function goes through the document and resolves references.

Argument:     the start of the items to be processed
Returns:      TRUE/FALSE
*/

BOOL
ref_resolve(item *item_list)
{
item *i, *new, *refitem;

DEBUG(D_any) debug_printf("Resolving references\n");

for (i = item_list; i != NULL; i = i->next)
  {
  paramstr *p;
  textblock *tb;
  tree_node *tn;
  uschar *ref;

  if (Ustrcmp(i->name, "#FILENAME") == 0)
    {
    read_filename = i->p.string;
    continue;
    }
  read_linenumber = i->linenumber;

  if (Ustrcmp(i->name, "xref") != 0 &&
      Ustrcmp(i->name, "footnoteref") != 0) continue;

  /* We have an <xref> or <footnoteref> element; search for its "linkend"
  parameter. */

  for (p = i->p.param; p != NULL; p = p->next)
    { if (Ustrcmp(p->name, "linkend") == 0) break; }
  if (p == NULL) continue;

  /* Now see if the reference was set. */

  tn = tree_search(id_tree, p->value);
  if (tn == NULL)
    {
    (void)error(12, p->value);
    continue;
    }
  refitem = (item *)tn->data.ptr;

  /* Handle <footnoteref>. Like <footnote>, we want to remove any preceding
  newline, to avoid an unwanted space. Then insert a dummy reference key (which
  will be replaced later). */

  if (Ustrcmp(i->name, "footnoteref") == 0)
    {
    footnote_remove_newline(i);
    footnote_insert_reference(i, PIN_FNREFREF);
    }

  /* Handle <xref>. The referenced item should have a number parameter, called
  "#number". */

  else
    {
    p = misc_param_find(refitem, US"#number");
    if (p == NULL)
     {
     (void)error(13, refitem->name);
     ref = US"???";
     }
    else ref = p->value;

    /* Unless the reference is empty, which it can be for pathological figure and
    table references, create a new item to be inserted into the chain after the
    xref element. This contains the text of the reference. */

    if (ref[0] != 0)
      {
      new = misc_malloc(sizeof(item));
      new->partner = new;
      new->linenumber = i->linenumber;
      new->flags = 0;
      Ustrcpy(new->name, "#PCDATA");
      misc_insert_item(new, i->next);

      tb = misc_malloc(sizeof(textblock) + Ustrlen(ref));
      tb->next = NULL;
      tb->vfont = NULL;
      tb->pin_flags = 0;
      tb->colour = 0;
      Ustrcpy(tb->string, ref);
      tb->length = Ustrlen(ref);

      new->p.txtblk = tb;
      }
    }
  }

read_linenumber = 0;

DEBUG(D_ref) debug_print_item_list(item_list, "after resolving references");

return TRUE;
}


/* End of ref.c */
