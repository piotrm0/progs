#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <libmng.h>

typedef struct user_struct {
  FILE *hFile;                 /* file handle */
  int  iIndent;                /* for nice indented formatting */
} userdata;
typedef userdata * userdatap;

/* ************************************************************************** */

mng_ptr myalloc (mng_size_t iSize)
{
  return (mng_ptr)calloc (1, iSize);   /* duh! */
}

/* ************************************************************************** */

void myfree (mng_ptr pPtr, mng_size_t iSize)
{
  free (pPtr);                         /* duh! */
  return;
}

/* ************************************************************************** */

mng_bool myopenstream (mng_handle hMNG)
{
  return MNG_TRUE;                     /* already opened in main function */
}

/* ************************************************************************** */

mng_bool myclosestream (mng_handle hMNG)
{
  return MNG_TRUE;                     /* gets closed in main function */
}

/* ************************************************************************** */

mng_bool myreaddata (mng_handle hMNG,
                     mng_ptr    pBuf,
                     mng_uint32 iSize,
                     mng_uint32 *iRead)
{                                      /* get to my file handle */
  userdatap pMydata = (userdatap)mng_get_userdata (hMNG);
  /* read it */
  *iRead = fread (pBuf, 1, iSize, pMydata->hFile);
  /* iRead will indicate EOF */
  return MNG_TRUE;
}

/* ************************************************************************** */

mng_bool myiterchunk (mng_handle  hMNG,
                      mng_handle  hChunk,
                      mng_chunkid iChunktype,
                      mng_uint32  iChunkseq)
{                                      /* get to my file handle */
  userdatap pMydata = (userdatap)mng_get_userdata (hMNG);
  char aCh[4];
  char zIndent[80];
  int iX;
  /* decode the chunkname */
  aCh[0] = (char)((iChunktype >> 24) & 0xFF);
  aCh[1] = (char)((iChunktype >> 16) & 0xFF);
  aCh[2] = (char)((iChunktype >>  8) & 0xFF);
  aCh[3] = (char)((iChunktype      ) & 0xFF);
  /* indent less ? */
  if ( (iChunktype == MNG_UINT_MEND) || (iChunktype == MNG_UINT_IEND) ||
       (iChunktype == MNG_UINT_ENDL) )
    pMydata->iIndent -= 2;
  /* this looks ugly; but I haven't
     figured out how to do it prettier */
  for (iX = 0; iX < pMydata->iIndent; iX++)
    zIndent[iX] = ' ';
  zIndent[pMydata->iIndent] = '\0';
  /* print a nicely indented line */
  printf ("%s%c%c%c%c\n", zIndent, aCh[0], aCh[1], aCh[2], aCh[3]);
  /* indent more ? */
  if ( (iChunktype == MNG_UINT_MHDR) || (iChunktype == MNG_UINT_IHDR) ||
       (iChunktype == MNG_UINT_JHDR) || (iChunktype == MNG_UINT_DHDR) ||
       (iChunktype == MNG_UINT_BASI) || (iChunktype == MNG_UINT_LOOP)    )
    pMydata->iIndent += 2;

  return MNG_TRUE;                     /* keep'm coming... */
}

mng_bool mywritedata (mng_handle hMNG,
                      mng_ptr    pBuf,
                      mng_uint32 iSize,
                      mng_uint32 *iWritten)
{                                      /* get to my file handle */
  userdatap pMydata = (userdatap)mng_get_userdata (hMNG);
  /* write it */
  *iWritten = fwrite (pBuf, 1, iSize, pMydata->hFile);
  /* iWritten will indicate errors */
  return MNG_TRUE;
}

int makeimage (char * zFilename)
{
  userdatap pMydata;
  mng_handle hMNG;
  mng_retcode iRC;
  /* get a data buffer */
  pMydata = (userdatap)calloc (1, sizeof (userdata));

  if (pMydata == NULL)                 /* oke ? */
    {
      fprintf (stderr, "Cannot allocate a data buffer.\n");
      return 1;
    }
  /* can we open the file ? */
  if ((pMydata->hFile = fopen (zFilename, "wb")) == NULL)
    {                                    /* error out if we can't */
      fprintf (stderr, "Cannot open output file %s.\n", zFilename);
      return 1;
    }
  /* let's initialize the library */
  hMNG = mng_initialize ((mng_ptr)pMydata, myalloc, myfree, MNG_NULL);

  if (!hMNG)                           /* did that work out ? */
    {
      fprintf (stderr, "Cannot initialize libmng.\n");
      iRC = 1;
    }
  else
    {                                    /* setup callbacks */
      if ( ((iRC = mng_setcb_openstream  (hMNG, myopenstream )) != 0) ||
	   ((iRC = mng_setcb_closestream (hMNG, myclosestream)) != 0) ||
	   ((iRC = mng_setcb_writedata   (hMNG, mywritedata  )) != 0)    )
	fprintf (stderr, "Cannot set callbacks for libmng.\n");
      else
	{                                  /* create the file in memory */
	  if ( ((iRC = mng_create        (hMNG)                                                    ) != 0) ||
	       ((iRC = mng_putchunk_mhdr (hMNG, 640, 480, 1000, 3, 1, 3, 0x0007)                   ) != 0) ||
	       ((iRC = mng_putchunk_basi (hMNG, 640, 160, 8, 2, 0, 0, 0, 0xFF, 0x00, 0x00, 0xFF, 1)) != 0) ||
	       ((iRC = mng_putchunk_iend (hMNG)                                                    ) != 0) ||
	       ((iRC = mng_putchunk_defi (hMNG, 0, 0, 0, MNG_TRUE, 0, 160, MNG_FALSE, 0, 0, 0, 0  )) != 0) ||
	       ((iRC = mng_putchunk_basi (hMNG, 640, 160, 8, 2, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 1)) != 0) ||
	       ((iRC = mng_putchunk_iend (hMNG)                                                    ) != 0) ||
	       ((iRC = mng_putchunk_defi (hMNG, 0, 0, 0, MNG_TRUE, 0, 320, MNG_FALSE, 0, 0, 0, 0  )) != 0) ||
	       ((iRC = mng_putchunk_basi (hMNG, 640, 160, 8, 2, 0, 0, 0, 0x00, 0x00, 0xFF, 0xFF, 1)) != 0) ||
	       ((iRC = mng_putchunk_iend (hMNG)                                                    ) != 0) ||
	       ((iRC = mng_putchunk_mend (hMNG)                                                    ) != 0)    )
	    fprintf (stderr, "Cannot create the chunks for the image.\n");
	  else
	    {
	      if ((iRC = mng_write (hMNG)) != 0)
		fprintf (stderr, "Cannot write the image.\n");

	    }
	}

      mng_cleanup (&hMNG);               /* cleanup the library */
    }

  fclose (pMydata->hFile);             /* cleanup */
  free (pMydata);

  return iRC;
}

int main (int argc, char *argv[])
{
  return makeimage ("dutch.mng");
}
