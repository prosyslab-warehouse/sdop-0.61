/*************************************************
*      Create Indexed Hyphenation Dictionary     *
*************************************************/

/* Written by P. Hazel, February 1990 */
/* Store block for holding file increased from 100K to
500K, June 1994. The hyphen list is getting bigger! */

/* Tidies:   August 2003 */
/* (void)s:  July 2005 */
/* Tidies:   August 2005 */

/* This is a free-standing program which reads a hyphenation
dictionary in the form of a list of words, one per line, in
alphabetical order (not counting the hyphens in the sort), and
and writes an identical output file, with an index on the front.
The index is in the form of four-letter entries with the offset
of the relevant point in the dictionary in digits after them.
The total number of entries in the index is output on the
first line. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/* Subroutine to get the next four letter index key into an integer
variable, starting from an arbitrary position in the dictionary. It
accepts the position in "pos", and returns a new position. The next
key and its position are returned via p and q. */

static int
word(int *p, int *q, unsigned char *storefile, int filesize, int pos)
{
int i;
int k = 0;
while (pos < filesize && storefile[pos] != '\n') pos++;
*q = pos + 1;
if (pos >= filesize) return -1;
for (i = 0; i < 4; i++)
  {
  if (storefile[++pos] == '-') pos++;
  k = (k << 8) | storefile[pos];
  }
*p = k;
return pos;
}


/* Main program. It requires two file names as arguments, and
an optional number giving the maximum index size. The default
is 2048. The actual size may be less because of duplications. */

int main(int argc, char **argv)
{
int indexsize = 2048;
int *indexblk;
int indexptr = 2;
int lastword = 0;
int filesize = 0;
int i, ch, base, step;
unsigned char *storefile;
FILE *infile, *outfile;

if (argc < 3 || strcmp(argv[1], "--help") == 0)
  {
  (void)fprintf(stderr, "Usage: buildhy <infile> <outfile> [<indexsize>]\n");
  exit(1);
  }

infile = fopen(argv[1], "r");

if (infile == NULL)
  {
  (void)fprintf(stderr, "*** Failed to open %s: %s\n", argv[1], strerror(errno));
  exit(1);
  }

outfile = fopen(argv[2], "w");

if (outfile == NULL)
  {
  (void)fprintf(stderr, "*** Failed to open %s: %s\n", argv[2], strerror(errno));
  exit(1);
  }

if (argc > 3) indexsize = atoi(argv[3]);

storefile = malloc(500000);
indexblk = malloc(indexsize * 2 * sizeof(int));

while ((ch = fgetc(infile)) != EOF) storefile[filesize++] = ch;
(void)fclose(infile);

step = filesize/indexsize;
base = step;

indexblk[0] = 0;
indexblk[1] = 0;

for (i = 1; i < indexsize; i++)
  {
  int pos = base;
  int xpos, first, next;
  pos = word(&first, &xpos, storefile, filesize, pos);
  do pos = word(&next, &xpos, storefile, filesize, pos); while (first == next);
  if (next != lastword)
    {
    indexblk[indexptr++] = next;
    indexblk[indexptr++] = xpos;
    lastword = next;
    }
  base += step;
  }

(void)fprintf(outfile, "%d\n", indexptr/2);
for (i = 2; i < indexptr; i += 2)
  {
  int c = indexblk[i];
  (void)fprintf(outfile, "%c%c%c%c%d\n", c>>24, c>>16, c>>8, c, indexblk[i+1]);
  }

for (i = 0; i < filesize; i++) fputc(storefile[i], outfile);

(void)fclose(outfile);
return 0;
}
