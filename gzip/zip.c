/* zip.c -- compress files to the gzip or pkzip format
 * Copyright (C) 1992-1993 Jean-loup Gailly
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License, see the file COPYING.
 */

#ifdef RCSID
static char rcsid[] = "$Id: zip.c,v 0.17 1993/06/10 13:29:25 jloup Exp $";
#endif

#include <ctype.h>
#include <sys/types.h>

#include "tailor.h"
#include "gzip.h"
#include "crypt.h"
#include "librarezip.h"

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#ifndef NO_FCNTL_H
#  include <fcntl.h>
#endif

_Thread_local local ulg crc;       /* crc on uncompressed file data */
_Thread_local long header_bytes;   /* number of bytes in gzip header */
_Thread_local uch *in_file_buffer;
_Thread_local size_t in_file_remaining;
_Thread_local uch *out_file_buffer;
_Thread_local size_t out_file_remaining;
size_t bufs_init();

/* ===========================================================================
 * Deflate in to out.
 * IN assertions: the input and output buffers are cleared.
 *   The variables time_stamp and save_orig_name are initialized.
 */
int _zip(in, out)
    int in, out;            /* input and output file descriptors */
{
    uch  flags = 0;         /* general purpose bit flags */
    ush  attr = 0;          /* ascii/binary flag */
    ush  deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */

    ifd = in;
    ofd = out;
    outcnt = 0;

    /* Write the header to the gzip file. See algorithm.doc for the format */

    method = DEFLATED;
    put_byte(GZIP_MAGIC[0]); /* magic header */
    put_byte(GZIP_MAGIC[1]);
    put_byte(DEFLATED);      /* compression method */

    if (save_orig_name) {
	flags |= ORIG_NAME;
    }
    put_byte(flags);         /* general flags */
    put_long(time_stamp);

    /* Write deflated file to zip file */
    crc = updcrc(0, 0);

    bi_init(out);
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);

    put_byte((uch)deflate_flags); /* extra flags */
    put_byte(OS_CODE);            /* OS identifier */

    if (save_orig_name) {
	char *p = basename(ifname); /* Don't save the directory part. */
	do {
	    put_char(*p);
	} while (*p++);
    }
    header_bytes = (long)outcnt;

    (void)_deflate();

#if !defined(NO_SIZE_CHECK) && !defined(RECORD_IO)
  /* Check input size (but not in VMS -- variable record lengths mess it up)
   * and not on MSDOS -- diet in TSR mode reports an incorrect file size)
   */
    if (ifile_size != -1L && isize != (ulg)ifile_size) {
	Trace((stderr, " actual=%ld, read=%ld ", ifile_size, isize));
	fprintf(stderr, "%s: %s: file size changed while zipping\n",
		progname, ifname);
    }
#endif

    /* Write the crc and uncompressed size */
    put_long(crc);
    put_long(isize);
    header_bytes += 2*sizeof(long);

    flush_outbuf();
    return OK;
}

size_t deflate(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap){
    uch  flags = 0;         /* general purpose bit flags */
    ush  attr = 0;          /* ascii/binary flag */
    ush  deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */
    method = DEFLATED;

    bufs_init(in_file, in_len, out_file, out_cap);

    bi_init(NO_FILE);
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);
    
    (void)_deflate();

    flush_outbuf();
    return bytes_out;
}

size_t zip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap)
{
    uch  flags = 0;         /* general purpose bit flags */
    ush  attr = 0;          /* ascii/binary flag */
    ush  deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */
    outcnt = 0;
    save_orig_name = 0;

    bufs_init(in_file, in_len, out_file, out_cap);

    /* Write the header to the gzip file. See algorithm.doc for the format */

    method = DEFLATED;
    put_byte(GZIP_MAGIC[0]); /* magic header */
    put_byte(GZIP_MAGIC[1]);
    put_byte(DEFLATED);      /* compression method */

    if (save_orig_name) {
	flags |= ORIG_NAME;
    }
    put_byte(flags);         /* general flags */
    put_long(time_stamp);

    /* Write deflated file to zip file */
    crc = updcrc(0, 0);

    bi_init(NO_FILE);
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);

    put_byte((uch)deflate_flags); /* extra flags */
    put_byte(OS_CODE);            /* OS identifier */

    if (save_orig_name) {
	char *p = basename(ifname); /* Don't save the directory part. */
	do {
	    put_char(*p);
	} while (*p++);
    }
    header_bytes = (long)outcnt;

    (void)_deflate();

#if !defined(NO_SIZE_CHECK) && !defined(RECORD_IO)
  /* Check input size (but not in VMS -- variable record lengths mess it up)
   * and not on MSDOS -- diet in TSR mode reports an incorrect file size)
   */
    if (ifile_size != -1L && isize != (ulg)ifile_size) {
	Trace((stderr, " actual=%ld, read=%ld ", ifile_size, isize));
	fprintf(stderr, "%s: %s: file size changed while zipping\n",
		progname, ifname);
    }
#endif

    /* Write the crc and uncompressed size */
    put_long(crc);
    put_long(isize);
    header_bytes += 2*sizeof(long);

    flush_outbuf();
    return bytes_out;
}

size_t bk_zip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap){
    uch  flags = 0;         /* general purpose bit flags */
    ush  attr = 0;          /* ascii/binary flag */
    ush  deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */
    method = DEFLATED;

    bufs_init(in_file, in_len, out_file, out_cap);

    put_byte(0x11); /* magic header */
    put_byte(0x72);
    
    put_byte((in_len >> 24) & 0xff);
    put_byte((in_len >> 16) & 0xff);
    put_byte((in_len >> 8) & 0xff);
    put_byte((in_len >> 0) & 0xff);
    
    bi_init(NO_FILE);
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);
    
    (void)_deflate();

    flush_outbuf();
    return bytes_out;
}

size_t bt_zip(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap){
    uch  flags = 0;         /* general purpose bit flags */
    ush  attr = 0;          /* ascii/binary flag */
    ush  deflate_flags = 0; /* pkzip -es, -en or -ex equivalent */
    method = DEFLATED;
    level = 9;

    bufs_init(in_file, in_len, out_file, out_cap);
    
    put_byte(((in_len / 16) >> 8) & 0xff);
    put_byte(((in_len / 16) >> 0) & 0xff);
    
    bi_init(NO_FILE);
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);
    
    (void)_deflate();

    flush_outbuf();
    return bytes_out;
}

size_t bufs_init(uint8_t *in_file, size_t in_len, uint8_t *out_file, size_t out_cap){
    in_file_buffer = in_file;
    in_file_remaining = in_len;

    out_file_buffer = out_file;
    out_file_remaining = out_cap;

    bytes_in = bytes_out = header_bytes = 0;
    insize = inptr = 0;

    memset(inbuf, 0, INBUFSIZ + INBUF_EXTRA);
    memset(outbuf, 0, OUTBUFSIZ + OUTBUF_EXTRA);
    memset(window, 0, 2*WSIZE);
    memset(d_buf, 0, DIST_BUFSIZE);
    memset(tab_prefix, 0, 1L << 16);
}

/* ===========================================================================
 * Read a new buffer from the current input file, perform end-of-line
 * translation, and update the crc and input file size.
 * IN assertion: size >= 2 (for end-of-line translation)
 */
int file_read(buf, size)
    char *buf;
    unsigned size;
{
    unsigned len;

    Assert(insize == 0, "inbuf not empty");

    len = read(ifd, buf, size);
    if (len == (unsigned)(-1) || len == 0) return (int)len;

    crc = updcrc((uch*)buf, len);
    isize += (ulg)len;
    return (int)len;
}

int buffer_read(buf, size)
    char *buf;
    unsigned size;
{
    unsigned len;

    // Assert(insize == 0, "inbuf not empty");

    len = (size <= in_file_remaining) ? size : in_file_remaining;
    if(len != 0){
        memcpy(buf, in_file_buffer, len);
        in_file_buffer += len;
        in_file_remaining -= len;
    }
    if (len == (unsigned)(-1) || len == 0) return (int)len;

    crc = updcrc((uch*)buf, len);
    isize += (ulg)len;
    return (int)len;
}
