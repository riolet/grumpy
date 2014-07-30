/*
 * gzipwrapper.c
 *
 *  Created on: Jul 29, 2014
 *      Author: Rohana Rezel
 *
 *      Requires: zlib
 */

#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include "nope.h"
#include "nopeutils.h"
#include "gzipwrapper.h"

gzwRequest gzw_init(Request request) {
	char * ae=getHeader(request.headers,"Accept-Encoding");
	gzwRequest gzwr;
	gzwr.request=request;
	if (strstr(ae,ENCODING_GZIP)!=NULL) {
		gzwr.gzenabled=true;
		gzwr.gzfile= gzdopen(request.client,"w");
	} else {
		gzwr.gzenabled=false;
		gzwr.file= fdopen(request.client,"w");
	}
	return gzwr;
}

void gzw_sendHeadersFromString(gzwRequest gzwr,const char *headerString) {
	int client=gzwr.request.client;
	if (gzwr.gzenabled) {
		FDPRINTF(client, "Content-Encoding: %s\r\n", ENCODING_GZIP,0);
	}
	FDPRINTF(client, "%s", headerString,0);
	STATIC_SEND(client, "\r\n", 0);
}

void gzw_sendHeadersType(gzwRequest gzwr,const char *type)
{
	int client=gzwr.request.client;
	STATIC_SEND(client, "HTTP/1.0 200 OK\r\n", 0);
	STATIC_SEND(client, SERVER_STRING, 0);
	FDPRINTF(client, "Content-Type: %s\r\n", type,0);
	if (gzwr.gzenabled) {
		FDPRINTF(client, "Content-Encoding: %s\r\n", ENCODING_GZIP,0);
	}
	STATIC_SEND(client, "Vary: Accept-Encoding\r\n", 0);
	STATIC_SEND(client, "\r\n", 0);
}

long gzw_printf (gzwRequest gzwr, const char *format, ...)
{
	/* initial buffer large enough for most cases, will resize if required */
	char *buf = malloc(MAX_BUFFER_SIZE);
	int len;

	va_list arg;
	long done;
	va_start (arg, format);
	len = vsnprintf (buf, MAX_BUFFER_SIZE, format, arg);
	va_end (arg);

	if (len > MAX_BUFFER_SIZE) {
		/* buffer size was not enough */
		free(buf);
		buf = malloc(len+1);
		if (buf == NULL) {
			printf("Could not allocate memory.");
			exit(EXIT_FAILURE);
		}
		va_start (arg, format);
		vsnprintf (buf, len+1, format, arg);
		va_end (arg);
	}

	if (gzwr.gzenabled) {
		gzwrite(gzwr.gzfile,buf,len);
	} else {
		fwrite(buf,1,len,gzwr.file);
	}

	free(buf);
	return done;
}

int gzw_puts (gzwRequest gzwr, const char *buf) {
	int len;
	if (gzwr.gzenabled) {
		len=gzputs(gzwr.gzfile,buf);
	} else {
		len=fputs(gzwr.file,buf);
	}
	return len;
}

int gzw_close (gzwRequest gzwr) {
	int len;
	if (gzwr.gzenabled) {
		gzclose(gzwr.gzfile);
	} else {
		fclose(gzwr.file);
	}
	return len;
}


