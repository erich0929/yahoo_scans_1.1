#ifndef _TREEAPI_H_
#define _TREEAPI_H_

#include <stdio.h>
#include <glib.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <sys/types.h>
#include "../stockapi/stockapi.h"

#define BASIS_ACTIVATED		0x0000000f
#define IS_ACTIVATED		0x00000000
#define IS_NOT_ACTIVATED	0x00000001

#define BASIS_OPENED		0x000000f0
#define IS_OPENED			0x00000000
#define IS_CLOSED			0x00000010

#define BASIS_LEAF			0x00000f00
#define IS_LEAF				0x00000100
#define IS_NOT_LEAF			0x00000000


typedef struct _TreeElement {
	char base_format [10];
	char format [40];
	int state_info;
	GNode* parent;
	GNode* lastchild;
	void* userdata;
} TreeElement;

typedef struct _regex_t_and_node {
	regex_t state;
	GNode* array;
}regex_t_and_node;


/*
GPtrArray* array_to_GPtrArray (TreeElement table [], int length,
	   							GPtrArray* empty_GPtrArray) {
	empty_GPtrArray = g_ptr_array_new ();
	GPtrArray* fulled_GPtrArray = empty_GPtrArray;
	
	int i = 0;
	for (i=0; i<length - 1; i++) {
		if (table [i].IsActivated == true) {
			g_ptr_array_add (fulled_GPtrArray, &table [i]);
		}
	}

	return fulled_GPtrArray;
}
*/
static void activate_node (GNode* node, gpointer data);
static gboolean store_into_g_ptr_array (GNode* node, gpointer g_ptr_array);
static void check_and_store (GNode* node, gpointer g_ptr_array);
void open_close_branch (GNode* node, int flag);
GPtrArray* node_to_array (GNode* node, GPtrArray* empty_GPtrArray);
void dump_to_parent (GNode* parent, TreeElement table [], int length);
GNode* search_by_regex (GNode* node, char* pattern, GNode* empty_GNode);

#endif
