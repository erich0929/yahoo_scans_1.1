/* Rebuilding code */
#include "treeapi.h"

/* --------- < Used by open_close_branch > ------------- */
static void activate_node (GNode* node, gpointer data) {
	TreeElement* temp = (TreeElement*) node -> data;
	STOCKINFO* temp_stock = (STOCKINFO*) temp -> userdata; /* for debugging */
	/* remove activate flag */
	temp -> state_info -= (temp -> state_info & BASIS_ACTIVATED);
	/* and redefine */
	int flag = *(int*) data;
	if ((flag & BASIS_OPENED) == IS_CLOSED && !G_NODE_IS_LEAF (node)
			&& (temp -> state_info & BASIS_OPENED) == IS_OPENED) {
		open_close_branch (node, flag); /* Recursive call */
	}
	if (flag == IS_OPENED) {
		temp -> state_info += IS_ACTIVATED;
	}	
	else temp -> state_info += IS_NOT_ACTIVATED;
}

static gboolean store_into_g_ptr_array (GNode* node, gpointer g_ptr_array) {
	TreeElement* temp = (TreeElement*) node -> data;
	STOCKINFO* temp_stock = (STOCKINFO*) temp -> userdata; /* for debugging */
	if ((temp -> state_info & BASIS_ACTIVATED) == IS_ACTIVATED) {
		/* node is the root */
		if (g_node_depth (node) == 1) {
			/* initialize */
			temp -> parent = NULL;
			temp -> lastchild = g_node_last_child (node); /* lastchild */

			memset (temp -> base_format, 0x0, sizeof (temp -> base_format));
			memset (temp -> format, 0x0, sizeof (temp -> format));

			/* base_format */
			/* nothing to do */
			/* format */
			char sign;
			if (!G_NODE_IS_LEAF (node)) {
				if ((temp -> state_info & BASIS_OPENED) == IS_OPENED) sign = '-';

				else sign = '+';

				strncpy (temp -> format, &sign ,1);
			}

		}
		/* node is not root */
		else {
			/* initialize */
			memset (temp -> base_format, 0x0, sizeof (temp -> base_format));
			memset (temp -> format, 0x0, sizeof (temp -> format));

			temp -> lastchild = g_node_last_child (node); /* lastchild */

			/* extend the parent's base_format */
			GNode* parent = temp -> parent;
			TreeElement* temp_parent = (TreeElement*) parent -> data;
			int length = strlen (temp_parent -> base_format);
			strncpy (temp -> base_format, temp_parent -> base_format, length);

			/* base_format */
			char base_format_link [3];
			char* temp_base_format;
			if ((temp_parent -> lastchild) == node) {
				temp_base_format = "\t\0";
			}
			else {
				temp_base_format = "|\t";
			}
			strncpy (base_format_link, temp_base_format, 3);			
			length = strlen (temp -> base_format);
			strncpy (temp -> base_format + length, base_format_link, 3);

			/* format */
			length = strlen (temp -> base_format);
			strncpy (temp -> format, temp -> base_format, length);

			/* ARM */
			char* arm;
			if (node == temp_parent -> lastchild) {
				arm = "|-";
			}
			else {
				arm = "*-";
			}
			length = strlen (temp -> format);
			strncpy (temp -> format + length, arm, 3);

			/* SIGN */
			char sign;
			if (!G_NODE_IS_LEAF (node)) {
				if ((temp -> state_info & BASIS_OPENED) == IS_OPENED) sign = '-';

				else sign = '+';
				length = strlen (temp -> format);
				strncpy (temp -> format + length, &sign ,1);
			}
			else {
				temp -> state_info += IS_LEAF;
			}
		}	

		g_ptr_array_add (g_ptr_array, temp);
	}
	return false;
}

static void check_and_store (GNode* node, gpointer data) {
	TreeElement* temp = (TreeElement*) node -> data;
	STOCKINFO* stock_data = (STOCKINFO*) temp -> userdata;

	/* 얕은 복사 */
	regex_t_and_node* temp_data = (regex_t_and_node*) malloc (sizeof (regex_t_and_node)); 
	*temp_data = *(regex_t_and_node*) data;

	int status = regexec (&(temp_data -> state), stock_data -> symbol, 0, NULL, 0);
	if (status == 0 || g_node_depth (node) == 2) {
		/* 얕은 복사 */
		TreeElement* copy_data = (TreeElement*) malloc (sizeof (TreeElement));
		*copy_data = *temp;
		GNode* copy_node = g_node_new (copy_data);
		g_node_insert (temp_data -> array, -1, copy_node);
		
		temp_data -> array = copy_node;
		if (!G_NODE_IS_LEAF (node)) {
			g_node_children_foreach (node, G_TRAVERSE_ALL, check_and_store, (gpointer) temp_data);
		} /* Recursive call */
	}
	
	free (temp_data);
	return;
}

GPtrArray* node_to_array (GNode* node, GPtrArray* empty_GPtrArray) {
	empty_GPtrArray = g_ptr_array_new ();
	GPtrArray* fulled_GPtrArray = empty_GPtrArray;
	g_node_traverse (node, G_PRE_ORDER, G_TRAVERSE_ALL, -1, store_into_g_ptr_array, (gpointer) fulled_GPtrArray);
	return fulled_GPtrArray;
}

void open_close_branch (GNode* parent, int flag) {
	if (!G_NODE_IS_LEAF (parent)) {
		/* remove open flag */
		TreeElement* temp = (TreeElement*) parent -> data;
		temp -> state_info -= (temp -> state_info & BASIS_OPENED);

		/* and redefine */
		temp -> state_info += flag;
		g_node_children_foreach (parent, G_TRAVERSE_ALL, activate_node, &flag);
	}
}

void dump_to_parent (GNode* parent, TreeElement table [], int length) {
	int i = 0;
	for (i = 0; i < length ; i++) {
		GNode* temp = g_node_new (&table [i]);
		g_node_insert (parent, -1, temp);
	}
}

GNode* search_by_regex (GNode* node, char* pattern, GNode* empty_GNode) {
	/* 얕은 복사 */
	TreeElement* root_data = (TreeElement*) malloc (sizeof (TreeElement));
	*root_data = *(TreeElement*) node -> data;
	empty_GNode = g_node_new (root_data);
	/* -- ! 얕은 복사 */
	GNode* fulled_GNode = empty_GNode;
	regex_t_and_node data;
	regex_t* state = &(data.state);
	int res = regcomp (state, pattern, REG_EXTENDED);
	char str [100];
	regerror (res, state, str, sizeof (str));

	data.array = fulled_GNode;
	g_node_children_foreach (node, G_TRAVERSE_ALL, check_and_store, (gpointer) &data);

	regfree (&data.state);
	return fulled_GNode;
}

GNode* new_tree_node (void* userdata, int state_info, GNode* parent) {
	TreeElement* temp = (TreeElement*) malloc (sizeof (TreeElement));
/*	temp = {NULL, NULL, state_info, parent, NULL, userdata}; */
	memset (temp -> base_format, 0x0, sizeof (temp -> base_format));
	memset (temp -> format, 0x0, sizeof (temp -> format));
	temp -> state_info = state_info;
	temp -> parent = parent;
	temp -> lastchild = NULL;
	temp -> userdata = userdata;
	return g_node_new (temp);
}
