#include "treeapi.h"

/* --------- < Used by open_close_branch > ------------- */
static void activate_node (GNode* node, gpointer data) {
	STOCKINFO* temp = (STOCKINFO*) node -> data;
	bool flag = *((bool*) data);
	if (flag == false && g_node_depth (node) == 2) {
		open_close_branch (node, flag); /* Recursive call */
	}
	if (flag == true) {
		temp -> IsActivated = true;
	}
	else temp -> IsActivated = false;
}

/* --------- < Used by node_to_array > ----------- */
static gboolean store_into_g_ptr_array (GNode* node, gpointer g_ptr_array) {
	STOCKINFO* temp = (STOCKINFO*) node -> data;

	if (temp -> IsActivated == true) {

		temp -> format_info = 0x000;
		/* pipe info */
		if (g_node_depth (node) == 3) {
			GNode* root = g_node_get_root (node);
			GNode* last_market = g_node_last_child (root);
			GNode* first_sibling = g_node_first_sibling (node);
			if (first_sibling != g_node_first_child (last_market)) {
				temp -> format_info = 0x1000;
			}
		}

		/* tabchar info */
		int depth;
		if ((depth = g_node_depth (node)) > 1) {
			temp -> format_info += (depth - 1 << 8);
		}
		/* arm info */
		if (g_node_depth (node) > 1) {
			if (node == g_node_last_sibling (node)) {
				temp -> format_info += ERICH_ARM2;
			}
			else {
				temp -> format_info += ERICH_ARM1;
			}
		}
		/* sign info */
		if (!(G_NODE_IS_LEAF (node))) {
			GNode* first = g_node_first_child (node);
			STOCKINFO* first_temp = (STOCKINFO*) first -> data;
			if (first_temp -> IsActivated) {
				temp -> format_info += ERICH_NAGATIVE;
			}
			else temp -> format_info += ERICH_POSITIVE;
		}

		GPtrArray* array = (GPtrArray*) g_ptr_array;
		g_ptr_array_add (g_ptr_array, temp);
	}
	return false;
}

static gboolean check_and_store (GNode* node, gpointer data) {
	STOCKINFO* temp = (STOCKINFO*) node -> data;
	/* regex_t_and_array* temp1 = (regex_t_and_array*) data; */
	regex_t_and_array* temp_data = (regex_t_and_array*) data;

	int status = regexec (&(temp_data -> state), temp -> symbol, 0, NULL, 0);
	if (status == 0 || g_node_depth (node) <= 2) {
		g_ptr_array_add (temp_data -> array, temp);
	}	
	return false;
}
void open_close_branch (GNode* parent, bool flag) {
	g_node_children_foreach (parent, G_TRAVERSE_ALL, activate_node, &flag);
}

GPtrArray* node_to_array (GNode* node, GPtrArray* empty_GPtrArray) {
	empty_GPtrArray = g_ptr_array_new ();
	GPtrArray* fulled_GPtrArray = empty_GPtrArray;
	g_node_traverse (node, G_PRE_ORDER, G_TRAVERSE_ALL, -1, store_into_g_ptr_array, (gpointer) fulled_GPtrArray);
	return fulled_GPtrArray;
}
/*
int next_branch_point (int id) {
	char stringID [20];
	sprintf (stringID, "%d", id);

	int length = strlen (stringID);
	int zero_position = length;
	char* temp = stringID;
	for (zero_position; *temp; temp ++, zero_position --) {
		if (*temp == '0') break;
	}
	
	char* plus = (char*) malloc (zero_position + 2);
	plus [0] = '1';
	temp = plus;
	temp ++;
	for (; *temp; temp ++) {
		*temp = '0';
	}
	plus [zero_position + 1] = '\0';
	id += atoi (plus);

	return id;
}
*/

void dump_to_parent (GNode* parent, STOCKINFO table [], int length) {
	int i = 0;
	for (i = 0; i < length ; i++) {
		GNode* temp = g_node_new (&table [i]);
		g_node_insert (parent, -1, temp);
	}
}

GPtrArray*  search_by_regex (GNode* node, char* pattern, GPtrArray* empty_GPtrArray) {
	empty_GPtrArray = g_ptr_array_new ();
	GPtrArray* fulled_GPtrArray = empty_GPtrArray;
	regex_t_and_array data;
	regex_t* state = &(data.state);
	int res = regcomp (state, pattern, REG_EXTENDED);
	char str [100];
	regerror (res, state, str, sizeof (str));

	data.array = fulled_GPtrArray;
	g_node_traverse (node, G_PRE_ORDER, G_TRAVERSE_ALL, -1, check_and_store, (gpointer) &data);
	regfree (&data.state);
	return fulled_GPtrArray;
}
