/* This module contains code for the support of PNG images. */

#if !SUPPORT_PNG

static void dummy(int x) { dummy(x-1); }

#else
#include "sdop.h"
#include <setjmp.h>

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>



static png_structp png_ptr;
static png_infop info_ptr;
static png_bytep *row_pointers;



/*************************************************
*                Set up PNG file                 *
*************************************************/

/* This function reads a PNG file into main memory, passing back some
information about the image. It must *not* close the file.

Arguments:
  f        the open file
  msg      where to return an error message

Returns:   TRUE on success, FALSE on failure
*/

int
read_PNG_file(FILE *f, uschar **msg)
{
int i;
uschar hdr[8];

/* Check that we really do have a PNG file. */

(void)fread(hdr, 1, 8, f);
if (png_sig_cmp(hdr, 0, 8) != 0)
  {
  *msg = US"not a PNG file";
  fclose(f);
  return FALSE;
  }

png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
if (png_ptr == NULL)
  {
  *msg = US"failure of png_create_read_struct()";
  return FALSE;
  }

info_ptr = png_create_info_struct(png_ptr);
if (info_ptr == NULL)
  {
  *msg = US"failure of png_create_info_struct()";
  return FALSE;
  }

if (setjmp(png_jmpbuf(png_ptr)))
  {
  *msg = US"failure to read PNG image information";
  return FALSE;
  }

png_init_io(png_ptr, f);
png_set_sig_bytes(png_ptr, 8);
png_read_info(png_ptr, info_ptr);
give_png_image_data(info_ptr->width, info_ptr->height, info_ptr->rowbytes);

if (setjmp(png_jmpbuf(png_ptr)))
  {
  *msg = US"failure to read PNG image data";
  return FALSE;
  }

row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * info_ptr->height);
for (i = 0; i < info_ptr->height; i++)
  row_pointers[i] = (png_byte *) malloc(info_ptr->rowbytes);
png_read_image(png_ptr, row_pointers);

return TRUE;
}



/*************************************************
*           Output PNG data                      *
*************************************************/

/* This function output the PNG rows in hex, and then frees the pointers and
the data.

Argument:  the output file
Returns:   nothing
*/

void
write_png_data(FILE *outfile)
{
int i, j;
for (i = 0; i < info_ptr->height; i++)
  {
  for (j = 0; j < info_ptr->rowbytes; j++)
    {
    if ((j & 31) == 0) (void)fprintf(outfile, "\n");
    (void)fprintf(outfile, "%02x", (unsigned int)row_pointers[i][j]);
    }
  (void)fprintf(outfile, "\n");
  free(row_pointers[i]);
  }

free(row_pointers);
}

#endif

/* End of png.c */
