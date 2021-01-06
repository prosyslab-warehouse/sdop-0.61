/*************************************************
*                  Son-of-GCAL                   *
*                Cloned for sdop                 *
*************************************************/

/* Copyright (c) Philip Hazel 2008 */
/* Created by Philip Hazel, November 1989 */
/* Last modified: February 1990 */
/* Cloned for sdop: April 2005 */

/* Header file for interfacing to the hyphenation routines. */

extern FILE *main_hyphenfile;

void Hyphen_Init(void);
int  Hyphen_Prepare(uschar *);
int  Hyphen_DePlural(uschar *, uschar *);
int  Hyphen_Next(uschar *, int);

/* End of hyphen.h */
