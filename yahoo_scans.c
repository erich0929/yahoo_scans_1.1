#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "./stockapi/stockapi.h"
#include "./boardwidget/boardwidget.h"
#include "./treeapi/treeapi.h"
#include "./commoninfo.h"
#include "./kospiInfo.h"

#define WHAT_ABOUT_TABLE	0x000F
#define TABLE_IS_MAIN	0x0001
#define TABLE_IS_SEARCH_RESULT	0x0002

static void print_main_board_header (WINDOW* wnd, int rows);
static void print_main_board_data (WINDOW* wnd, gpointer data, int rows);

static void print_main_board_header (WINDOW* wnd, int colindex) {
	wprintw (wnd, "%s", "Select a stock you want to scan.");

}

static void print_main_board_data (WINDOW* wnd, gpointer data, int colindex){
	TreeElement* temp = (TreeElement*) data;
	STOCKINFO* temp_stock = (STOCKINFO*) temp -> userdata;
/*
	char temp_ch [60];
	memset (temp_ch, 0x0, sizeof (temp_ch));
	int length = strlen (temp -> format);
	strncpy (temp_ch, temp -> format, length);
	int position = strlen (temp_ch);
	length = strlen (temp_stock -> symbol);
	strncpy (temp_ch + position, temp_stock -> symbol, length);	
	wprintw (wnd, "%s", temp_ch);
*/
	wprintw (wnd, "%s%s", temp -> format, temp_stock -> symbol);
}




int main(int argc, char* argv[])
{
	/* initialize the stock' informations */
	GNode* world = new_tree_node ((void*) &WORLD, IS_OPENED | IS_ACTIVATED,
			NULL);

	GNode* nyse = new_tree_node ((void*) &NYSE, IS_CLOSED | IS_ACTIVATED, world);

	g_node_insert (world, -1, nyse);
	/* dump_to_parent (nyse, NYSE_LIST, sizeof (NYSE_LIST) / sizeof (STOCKINFO)); */
	int i;
	for (i = 0; i < sizeof (NYSE_LIST) / sizeof (STOCKINFO); i++) {
		GNode* temp_node = new_tree_node ((void*) &NYSE_LIST [i], 
				IS_CLOSED | IS_NOT_ACTIVATED, nyse);
		g_node_insert (nyse, -1, temp_node);
	}

	GNode* kospi = new_tree_node ((void*) &KOSPI, IS_CLOSED | IS_ACTIVATED, world);
	g_node_insert (world, -1, kospi);

	/* dump_to_parent (kospi, KOSPI_LIST, sizeof (KOSPI_LIST) / sizeof (STOCKINFO)); */

	for (i = 0; i < sizeof (KOSPI_LIST) / sizeof (STOCKINFO); i++) {
		GNode* temp_node = new_tree_node ((void*) &KOSPI_LIST [i], 
				IS_CLOSED | IS_NOT_ACTIVATED, kospi);
		g_node_insert (kospi, -1, temp_node);
	}

	GPtrArray* main_data_table;
	GPtrArray* result_data_table = g_ptr_array_new ();
	GNode* result_root = NULL;

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
	BOARD_WIDGET* main_board = new_board (main_board, 20, 1, 1, 63,
			&main_board_point_info, main_data_table,
			print_main_board_header, 
			print_main_board_data, (void*) &main_data_table_flag);

	set_rowIndex (main_board, 0);
	update_board (main_board);
	inactivate_board (main_board);

	/* search_win */
	WINDOW* box_search_wnd = newwin (3, 65, 26, 3);
	box (box_search_wnd, ACS_VLINE, ACS_HLINE);
	wrefresh (box_search_wnd);
	WINDOW* search_wnd = subwin (box_search_wnd, 1, 50, 27, 13);
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
	while ((ch = getch ()) != KEY_F(2)) {
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

							/* 검색 */
							if (length = strlen (buffer)) {
								memset (regex_exp, 0x0, sizeof (regex_exp));
								/*			strncpy (regex_exp, &regex_head, 1); */
								strncpy (regex_exp, buffer, 30); 
								/*			length = strlen (regex_exp);
											strncpy (regex_exp + length, &regex_tail, 1);*/

								result_root = search_by_regex (world, regex_exp, result_root);

								main_node = result_root; /* is copied with world */

								result_data_table = node_to_array (result_root, result_data_table);
								clear_board (main_board);
								main_board -> dataTable = result_data_table;
								main_board -> firstrow_index = 0;
								main_board -> selected_index = 0;
								main_board -> userdata = (void*) &result_data_flag;
								set_rowIndex (main_board, 0);
								update_board (main_board);
								inactivate_board (main_board);
							}
							break;

						default :
							if (result_root != NULL) g_node_destroy (result_root);
							g_ptr_array_unref (result_data_table);

							length = strlen (buffer);
							if (length < 20) {
								strncpy (buffer + length, (char*) &ch, 1);
								werase (search_wnd);
								wrefresh (search_wnd);
								wprintw (search_wnd, buffer);
								wrefresh (search_wnd);
							}

							/* 검색 */
							memset (regex_exp, 0x0, sizeof (regex_exp));
							/*			strncpy (regex_exp, &regex_head, 1); */
							strncpy (regex_exp, buffer, 30); 
							/*			length = strlen (regex_exp);
										strncpy (regex_exp + length, &regex_tail, 1);*/

							result_root = search_by_regex (world, regex_exp, result_root);
					
							main_node = result_root; /* is copied with world */
					
							result_data_table = node_to_array (result_root, result_data_table);
							clear_board (main_board);
							main_board -> dataTable = result_data_table;
							main_board -> firstrow_index = 0;
							main_board -> selected_index = 0;
							main_board -> userdata = (void*) &result_data_flag;
							set_rowIndex (main_board, 0);
							update_board (main_board);
							inactivate_board (main_board);

							break;
					}

				}
				curs_set (0);
				nodelay (stdscr, true);
				break;
			
			case 't' :
				if ((*(int*) (main_board -> userdata) & WHAT_ABOUT_TABLE) == TABLE_IS_SEARCH_RESULT) {
					main_board -> dataTable = remember_dataTable;
					main_board -> firstrow_index = remember_firstrow;
					main_board -> lastrow_index = remember_lastrow;
					main_board -> selected_index = remember_selected_index;
					main_board -> userdata = (void*) &main_data_table_flag;
					main_node = world;

					clear_board (main_board);
					set_rowIndex (main_board, 0);
					set_rowIndex (main_board, remember_selected_index);
					main_board -> wndFlag = true;
					main_board -> dataFlag = true;
					update_board (main_board);
					inactivate_board (main_board);
				}

				break;
		
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
