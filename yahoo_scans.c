#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "./stockapi/stockapi.h"
#include "./boardwidget/boardwidget.h"
#include "./treeapi/treeapi.h"

#define WHAT_ABOUT_TABLE	0x000F
#define TABLE_IS_MAIN	0x0001
#define TABLE_IS_SEARCH_RESULT	0x0002

static void print_main_board_header (WINDOW* wnd, int rows);
static void print_main_board_data (WINDOW* wnd, gpointer data, int rows);

static void print_main_board_header (WINDOW* wnd, int colindex) {
	wprintw (wnd, "%s", "Select a stock you want to scan.");
	wrefresh (wnd);
}

static void print_main_board_data (WINDOW* wnd, gpointer data, int colindex){
	STOCKINFO* temp = (STOCKINFO*) data;
	int pipe = (temp -> format_info & BASIS_PIPE) >> 12;
	int count_tab = (temp -> format_info & BASIS_TAB) >> 8;
	int arm = (temp -> format_info & BASIS_ARM) >> 4;
	int sign = (temp -> format_info & BASIS_SIGN);
	char format [30];
	memset (format, 0x0, sizeof (format));
	char* temp_ch = format;
	int i = 0;
	int t = 0;
	for (i = 0, t = 1; i < count_tab; i++) {
		int length = strlen (format);
		char tab_ch = '\t';
		strncpy (temp_ch + length, &tab_ch, 1);
		if (pipe == 1 && t == 1) {
			length = strlen (format);
			char pipe_ch = '|';
			strncpy (temp_ch + length, &pipe_ch, 1);
			t--;
		}
	}
	int length;
	char* arm_ch;
	if (arm == 1) {
		arm_ch = "|-- ";
		length = strlen (format);
		strncpy (temp_ch + length, arm_ch, 4);
	}
	else if (arm == 2) {
		arm_ch = "*-- ";
		length = strlen (format);
		strncpy (temp_ch + length, arm_ch, 4);
	}


	char* sign_ch;
	if (sign == 1) {
		sign_ch = "+ ";
		length = strlen (format);
		strncpy (temp_ch + length, sign_ch, 2);
	}
	else if (sign == 2) {
		sign_ch = "- ";
		length = strlen (format);
		strncpy ( temp_ch + length, sign_ch, 2);
	}

	length = strlen (format);
	strcpy (temp_ch + length, temp -> symbol);
	length = strlen (format);
	temp_ch [length] = '\0';

	wprintw (wnd, "%s", format);
}

STOCKINFO WORLD = {"World", NULL, true, 1, 0x0000};
STOCKINFO NYSE = {"NYSE", NULL, true, 2, 0x0000};
STOCKINFO KOSPI = {"KOSPI", NULL, true, 2, 0x0000};

STOCKINFO NYSE_LIST [] = 	{
	{"APPLE", "14255.NS", false, 3, 0x0000},
	{"MicroSoft", "14547.NS", false, 3, 0x0000},
	{"Boeing", "17554.NS", false, 3, 0x0000},
};

STOCKINFO KOSPI_LIST [] = 	{
	{"SAMSUNG", "15478.KS", false, 3},
	{"LG", "15467.KS", false, 3},
	{"Hyundai", "78231.KS", false, 3},
	{"KIA", "25236.KS", false, 3}
};


int main(int argc, char* argv[])
{
	/* initialize the stock' informations */
	GNode* world = g_node_new (&WORLD);
	GNode* nyse = g_node_new (&NYSE);
	g_node_insert (world, -1, nyse);
	dump_to_parent (nyse, NYSE_LIST, sizeof (NYSE_LIST) / sizeof (STOCKINFO));

	GNode* kospi = g_node_new (&KOSPI);
	g_node_insert (world, -1, kospi);
	dump_to_parent (kospi, KOSPI_LIST, sizeof (KOSPI_LIST) / sizeof (STOCKINFO));

	GPtrArray* main_data_table;
	GPtrArray* result_data_table = g_ptr_array_new ();
	GNode* result_root;

	main_data_table = node_to_array (world, main_data_table);

	/* initialize the display context */
	init_scr ();
	cbreak ();
	POINT_INFO main_board_point_info;
	main_board_point_info.origin_x = 3;
	main_board_point_info.origin_y = 3;
	main_board_point_info.base_color = COLOR_PAIR (0);
	main_board_point_info.selected_color = COLOR_PAIR (1);
	main_board_point_info.x_from_origin = 0;
	main_board_point_info.y_from_origin = 0;

	int main_data_table_flag = TABLE_IS_MAIN;
	/* initialize the main_board widget */
	BOARD_WIDGET* main_board = new_board (main_board, 20, 1, 1, 33,
			&main_board_point_info, main_data_table,
			print_main_board_header, 
			print_main_board_data, (void*) &main_data_table_flag);

	set_rowIndex (main_board, 0);
	update_board (main_board);
	inactivate_board (main_board);

	/* search_win */
	WINDOW* box_search_wnd = newwin (3, 35, 26, 3);
	box (box_search_wnd, ACS_VLINE, ACS_HLINE);
	wrefresh (box_search_wnd);
	WINDOW* search_wnd = subwin (box_search_wnd, 1, 20, 27, 13);
	wrefresh (search_wnd);
	mvwprintw (box_search_wnd, 1, 1, "Search : ");
	wrefresh (box_search_wnd);

	/* main loop */
	int ch;
	int x;
	int y;
	char buffer [30];
	char regex_exp [30];
	memset (buffer, 0x0, sizeof (buffer));
	int length;

	char regex_head = '/';
	char regex_tail = '/';
	/* when search_by_regex called, it's needed to remember */
	int remember_firstrow;
	int remember_lastrow;
	int remember_selected_index;
	GPtrArray* remember_dataTable;

	int result_data_flag = TABLE_IS_SEARCH_RESULT;

	GNode* main_node = world;
	while ((ch = getch ()) != KEY_F(1)) {
		switch (ch) {
			/* form window loop */
			case 'b' :
				board_eventhandler (main_board, main_node);
				break;
			case 's' :
				wprintw (search_wnd, "");
				wrefresh (search_wnd);
				curs_set (1);
				nodelay (stdscr, false);

				if ((*(int*) (main_board -> userdata) & WHAT_ABOUT_TABLE) ==  TABLE_IS_MAIN) {
					remember_dataTable = main_board -> dataTable;
					remember_firstrow = main_board -> firstrow_index;
					remember_lastrow = main_board -> lastrow_index;
					remember_selected_index = main_board -> selected_index;
				}
			
				while ((ch = getch ()) != '\n') {
					switch (ch) {
						case KEY_BACKSPACE :
							length = strlen (buffer);
							*(buffer + length - 1) = '\0';
							werase (search_wnd);
							wrefresh (search_wnd);
							wprintw (search_wnd, buffer);
							wrefresh (search_wnd);							
							break;

						default :

							g_ptr_array_unref (result_data_table);

							length = strlen (buffer);
							if (length < 20) {
								strncpy (buffer + length, (char*) &ch, 1);
								werase (search_wnd);
								wrefresh (search_wnd);
								wprintw (search_wnd, buffer);
								wrefresh (search_wnd);
							}
							memset (regex_exp, 0x0, sizeof (regex_exp));
				/*			strncpy (regex_exp, &regex_head, 1); */
							strncpy (regex_exp, buffer, 30); 
				/*			length = strlen (regex_exp);
							strncpy (regex_exp + length, &regex_tail, 1);*/
							
							result_root = search_by_regex (world, regex_exp, result_root);
							main_node = result_root;
							result_data_table = node_to_array (result_root, result_data_table);
							clear_board (main_board);
							main_board -> dataTable = result_data_table;
							main_board -> userdata = (void*) &result_data_flag;
							set_rowIndex (main_board, 0);
							update_board (main_board);
							inactivate_board (main_board);

							break;
					}
				}
				curs_set (0);
				nodelay (stdscr, true);
			default :
				break;
		}
		usleep (1000);
	}

	/* free the memory */
	g_ptr_array_unref (main_data_table);
	g_ptr_array_unref (result_data_table);
	g_node_destroy (world);
	del_board (main_board);
	endwin ();
	return 0;
}

