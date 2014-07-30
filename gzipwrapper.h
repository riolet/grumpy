/*
 * gzipwrapper.h
 *
 *  Created on: Jul 29, 2014
 *      Author: rohana
 */

#ifndef GZIPWRAPPER_H_
#define GZIPWRAPPER_H_

#include <zlib.h>
typedef struct {
	bool gzenabled;
	Request request;
	gzFile gzfile;
	FILE * file;
} gzwRequest;

gzwRequest gzw_init(Request request);
void gzw_sendHeadersFromString(gzwRequest gzwr,const char *headerString);
void gzwSendHeadersType(Request request,const char *type);
long gzw_printf (gzwRequest gzwr, const char *format, ...);
int gzw_puts (gzwRequest gzwr, const char *buf);
int gzw_close (gzwRequest gzwr);
#define GZW_SEND_TEXT_HTML_HEADER(request) gzw_sendHeadersType(request,MIME_TEXT_HTML)

#endif /* GZIPWRAPPER_H_ */
