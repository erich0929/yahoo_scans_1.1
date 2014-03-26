#ifndef _STOCKAPI_H_
#define _STOCKAPI_H_

#include <stdio.h>
#include <glib.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BASIS_PIPE	0xf000
#define ERICH_PIPE	0x1000
#define BASIS_TAB	0x0f00
#define ERICH_TAB1	0x0100
#define ERICH_TAB2	0x0200
#define BASIS_ARM	0x00f0
#define ERICH_ARM1	0x0010
#define ERICH_ARM2	0x0020
#define BASIS_SIGN	0x000f
#define ERICH_POSITIVE	0x0001
#define ERICH_NAGATIVE 	0x0002

typedef struct _STOCK {
	char date[50];
	float open;
	float high;
	float low;
	float close;
	float volume;
	float adj_close;
}STOCK;

typedef struct _STOCKINFO {
	char* symbol;
	char* code;
	bool IsActivated;
	int depth;
	int format_info;
} STOCKINFO;

typedef struct _BufferStruct {
	char* buffer;
	size_t size;
}BufferStruct;

static size_t WriteMemoryCallback
(void *ptr, size_t size, size_t nmemb, void *data);

int str_to_array (char* line, GPtrArray* data);

long get_stock_from_yahoo (char* code, 
		char* from_mm, char* from_dd, char* from_yy, 
		char* to_mm, char* to_dd, char* to_yy, GPtrArray* data);

#endif
