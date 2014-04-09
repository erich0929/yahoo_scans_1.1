#include "boardwidget.h"
#include "../treeapi/treeapi.h"
/*
typedef struct _POINT_INFO {

	int origin_x;
	int origin_y;
	int x_from_origin;
	int y_from_origin;

} POINT_INFO;

typedef void (*PRINT_HEADER_FUNC) (WINDOW*, int);
typedef void (*PRINT_DATA_FUNC) (WINDOW*, gpointer, int);

typedef struct _BOARD_WIDGET {
	WINDOW* mainWnd;
	WINDOW* headerWnd;

	GPtrArray* wndTable;
	GPtrArray* dataTable;
	PRINT_HEADER_FUNC printHeader;
	PRINT_DATA_FUNC printData;
	int firstrow_index;
	int lastrow_index;

	int row;
	int col;
	int row_width;
	int col_width;

	POINT_INFO* point_info;
	POINT_INFO* origin_point_info;

	chtype base_color;
	chtype selected_color;
	guint selected_index;

	bool wndFlag;
	bool dataFlag;	
} BOARD_WIDGET;

*/
void init_scr()
{
	initscr();
	start_color(); 
	curs_set(0);
	noecho();
	nodelay (stdscr, TRUE); 
	keypad(stdscr, TRUE);
	use_default_colors (); 
	short r, g, b;
	r=81, g=243, b=22;
	color_content (COLOR_GREEN, &r, &g, &b);
	r=8, g=35, b=77;
	color_content (COLOR_BLUE, &r, &g, &b);
/*	r=249, g=249, b=41;
	color_content (COLOR_YELLOW, &r, &g, &b); */
	init_pair (1, COLOR_YELLOW, COLOR_GREEN);
	init_pair (2, COLOR_YELLOW, COLOR_BLUE);
	init_pair (3, COLOR_YELLOW, COLOR_RED);
	init_pair (4, COLOR_WHITE, COLOR_GREEN);
	init_pair (5, COLOR_WHITE, COLOR_RED);
	init_pair (6, COLOR_WHITE, COLOR_BLUE);
	init_pair (7, COLOR_WHITE, COLOR_MAGENTA);

	refresh ();
}

BOARD_WIDGET* new_board (BOARD_WIDGET* board, int row, int col,
		int row_width, int col_width, POINT_INFO* point_info,
	   	GPtrArray* dataTable, PRINT_HEADER_FUNC printHeader,
	   	PRINT_DATA_FUNC printData, void* userdata) {

	int i, j;
	int maxX, maxY;
	getmaxyx (stdscr, maxY, maxX); /* It's used for x_from_origin and y_from_origin
									* in the point_info Structure */

	/* Allocate memory for new board */
	board = (BOARD_WIDGET*) malloc (sizeof (BOARD_WIDGET));

	/* Initialize fields value */
	board -> row = row;
	board -> col = col;
	board -> row_width = row_width;
	board -> col_width = col_width;
	
	board -> base_color = point_info -> base_color;  
	board -> selected_color = point_info -> selected_color;
	board -> selected_index = 0;

	board -> userdata = userdata;	
	/* ------ <NOTICE THAT IT'S FIRST TIME TO CREATE WIDGET> ------- */
	if (point_info -> x_from_origin == 0 && point_info -> y_from_origin == 0) {

		board -> point_info = (POINT_INFO*) malloc (sizeof (POINT_INFO));
		board -> point_info -> origin_x = point_info -> origin_x;
		board -> point_info -> origin_y = point_info -> origin_y;
		board -> point_info -> base_color = point_info -> base_color;
		board -> point_info -> selected_color = point_info -> selected_color;
		board -> point_info -> x_from_origin = maxX - (board -> point_info -> origin_x);
		board -> point_info -> y_from_origin = maxY - (board -> point_info -> origin_y);
		
		board -> origin_point_info = (POINT_INFO*) malloc (sizeof (POINT_INFO));
		board -> origin_point_info -> origin_x = board -> point_info -> origin_x;
		board -> origin_point_info -> origin_y = board -> point_info -> origin_y;
		board -> origin_point_info -> base_color = board -> point_info -> base_color;
		board -> origin_point_info -> selected_color = board -> point_info -> selected_color;
		board -> origin_point_info -> x_from_origin = board -> point_info -> x_from_origin;
		board -> origin_point_info -> y_from_origin = board -> point_info -> y_from_origin;

	}
	/* ------ </NOTICE THAT IT'S FIRST TIME TO CREATE WIDGET> ------- */

	/* ------ <NOTICE TO REMEMBER LAST WIDGET WHICH IS CLEARED> ------ */
	else {

		board -> origin_point_info = (POINT_INFO*) malloc (sizeof (POINT_INFO));
		board -> origin_point_info -> origin_x = point_info -> origin_x;
		board -> origin_point_info -> origin_y = point_info -> origin_y;
		board -> origin_point_info -> base_color = point_info -> base_color;
		board -> origin_point_info -> selected_color = point_info -> selected_color;
		board -> origin_point_info -> x_from_origin = point_info -> x_from_origin;
		board -> origin_point_info -> y_from_origin = point_info -> y_from_origin;
		
		board -> point_info = (POINT_INFO*) malloc (sizeof (POINT_INFO));
		board -> point_info -> origin_x = point_info -> origin_x;
		board -> point_info -> origin_y = point_info -> origin_y;
		board -> point_info -> base_color = point_info -> base_color;
		board -> point_info -> selected_color = point_info -> selected_color;
		board -> point_info -> x_from_origin = maxX - (board -> point_info -> origin_x);
		board -> point_info -> y_from_origin = maxY - (board -> point_info -> origin_y);

	}
	/* ------ </NOTICE TO REMEMBER LAST WIDGET WHICH IS CLEARED> ------ */

	int add_row = 
		board -> point_info -> y_from_origin - board -> origin_point_info -> y_from_origin;
	
	add_row = ((int) (add_row / row_width) > 0) ? (int) (add_row / row_width) : 0;
	
	int add_col_width = 
		board -> point_info -> x_from_origin - board -> origin_point_info -> x_from_origin;
	
	add_col_width = (add_col_width > 0) ? add_col_width : 0;

	board -> dataTable = dataTable;
	board -> printHeader = printHeader;
	board -> printData = printData;

	board -> wndFlag = true;
	board -> dataFlag = true;

	/* Create new main window */
	board -> mainWnd = newwin (
						board -> row * board -> row_width + 
						add_row * row_width + 3,
						board -> col * board -> col_width + 
						add_col_width + 2,
					   	board -> point_info -> origin_y,
					   	board -> point_info -> origin_x);

	wbkgd (board -> mainWnd, board -> base_color);
	box (board -> mainWnd, ACS_VLINE, ACS_HLINE);
	board -> wndTable = g_ptr_array_new ();

	/* Create new header window */
	board -> headerWnd = subwin (board -> mainWnd, 1, board -> col_width * board -> col + add_col_width, 
								board -> point_info -> origin_y + 1, board -> point_info -> origin_x + 1);
	for (j = 0; j < board -> col; j++) {
		wbkgd (board -> headerWnd, board -> base_color | A_BOLD);
		board -> printHeader (board -> headerWnd, j);
		wrefresh (board -> headerWnd);	
	}
	/* Declare container which have windows in a specific row */
	WINDOW** rowContainer;

	/* Create new subwindow for dataTable */
	for (i = 0; i < board -> row + add_row; i++) {
		rowContainer = (WINDOW**) malloc (col * sizeof (WINDOW *));
		for (j = 0; j < board -> col; j++) {
			rowContainer [j] = subwin (board -> mainWnd, board -> row_width, 
										board -> col_width + add_col_width,
										i * board -> row_width + board -> point_info -> origin_y + 2, 
										j * board -> col_width + board -> point_info -> origin_x + 1);
		}
		g_ptr_array_add (board -> wndTable, rowContainer);
	}

	board -> firstrow_index = 0;
	board -> lastrow_index = (board -> wndTable -> len < board -> dataTable -> len) ?
								((int) board -> wndTable -> len - 1) :
								((int) board -> dataTable -> len - 1);


	return board;
}

void set_base_color (BOARD_WIDGET* board, chtype color) {

	board -> base_color = color;

}

void set_selected_color (BOARD_WIDGET* board, chtype color) {

	board -> selected_color = color;

}

void set_rowIndex (BOARD_WIDGET* board, int index) {

	int remember_num = board -> lastrow_index - board -> firstrow_index;

	/* ------------- <SET selected_index> ----------------- */
	guint length = board -> wndTable -> len;
	if (index <= 0) {
		board -> selected_index = 0;
	}
	else if (index > (int) length - 1) {
		board -> selected_index = length - 1;

	}
	else board -> selected_index = index;


	/* ------------- </SET selected_index> ------------------ */

	/* ------------- <SET firstrow_index and lastrow_index> -------------- */
	if (index <= 0) {
		board -> firstrow_index = ((int) (board -> firstrow_index) + index < 0) ? 0 : (int) board -> firstrow_index + index;
	/*	board -> lastrow_index = (board -> dataTable -> len < board -> wndTable -> len) ?
			board -> firstrow_index + board -> dataTable -> len -1 :
			board -> firstrow_index + board -> wndTable -> len -1; */

		board -> lastrow_index = board -> firstrow_index + ((int) length - 1);
		board -> lastrow_index = (board -> lastrow_index < ((int) (board -> dataTable -> len) - 1) ?
									board -> lastrow_index : (int) (board -> dataTable -> len) - 1);
		
	}

	else if (index >= ((int) length - 1)) {
		board -> lastrow_index = ((int) (board -> lastrow_index) + (index - ((int) length - 1)) > board -> dataTable -> len - 1) ?
			board -> dataTable -> len - 1 : (int) (board -> lastrow_index) + (index - ((int) length - 1));
	/*	board -> firstrow_index = (board -> dataTable -> len < board -> wndTable -> len) ?
			board -> lastrow_index - (board -> dataTable -> len - 1) :
			board -> lastrow_index - (board -> wndTable -> len - 1);	*/
		int temp = (remember_num < ((int) length - 1)) ? remember_num : ((int) length - 1);
		board -> firstrow_index = board -> lastrow_index - remember_num;
		board -> firstrow_index = (board -> firstrow_index > 0) ?
									board -> firstrow_index : 0;

	}

	/* ------------- </SET firstrow_index and lastrow_index> -------------- */
	
	if (board -> selected_index > (board -> lastrow_index - board -> firstrow_index)) {
				board -> selected_index =
					(board -> lastrow_index - board -> firstrow_index);
	}

}

/* clear_board must be called before new setting board !! */
void clear_board (BOARD_WIDGET* board) {

	WINDOW** rowContainer;
	int i, j;
	/* ------------- <CLEAR VIEW> ------------- */
	if (board -> wndFlag == true) {
		rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable , board -> selected_index);
		for (j = 0; j < board -> col; j++) {
			wbkgd (rowContainer [j], board -> base_color | A_BOLD);
			wrefresh (rowContainer [j]);
		}
	}


	/* ------------- </CLEAR VIEW> ------------- */


	/* ------------- <CLEAR DATA> ---------------- */
	if (board -> dataFlag == true) {
		for (i = 0; i < board -> wndTable -> len; i++) { /* debug : i < row */
			rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable, i);
			for (j = 0; j < board -> col; j++) {
				werase (rowContainer [j]); /* clear data */
				/* wmove (rowContainer [j], 0 , 0); */
				/* wrefresh (rowContainer [j]); */
			}
		}
	}


	/* ------------- </CLEAR DATA> ---------------- */
}
void update_board (BOARD_WIDGET* board) {

	int i, j, k;
	WINDOW** rowContainer;	

	/* ------------- <VIEW UPDATE> --------------- */

	if (board -> wndFlag == true) {
		rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable, board -> selected_index);

		for (j = 0; j < board -> col; j++) {
			wbkgd (rowContainer [j], board -> selected_color | A_BOLD);
			wrefresh (rowContainer [j]);
		}
		wrefresh (board -> mainWnd); 
	}

	/* ------------- </VIEW UPDATE> -------------- */

	/* ------------- <DATA UPDATE> -------------- */

	if (board -> dataFlag == true) {
		gpointer recordset;
		int firstrow_index = (int) board -> firstrow_index;
		int lastrow_index = (int) board -> lastrow_index;

		for (i = board -> firstrow_index, k = 0; i < board -> lastrow_index + 1; i++, k++) {
			rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable, k);
			recordset = g_ptr_array_index (board -> dataTable, i);
			for (j = 0; j < board -> col; j++) {
				wattron (rowContainer [j], A_BOLD);
				board -> printData (rowContainer [j], (gpointer) recordset, j);
				wrefresh (rowContainer [j]);
			}
		}
	}

	/* ------------- </DATA UPDATE> -------------- */

	/* ------------- <SET FLAG MEMBERS> ------------ */

	board -> wndFlag = true;
	board -> dataFlag = true;

	/* ------------- </SET FLAG MEMBERS> ------------ */

}			

void scrolldown_handler (BOARD_WIDGET* board) {

	/* ------- <SET FLAG MEMBERS> --------  */		
	if (board -> selected_index == board -> wndTable -> len - 1)
		board -> wndFlag = false;
	else board -> wndFlag = true;

	if (board -> lastrow_index == board -> dataTable -> len - 1)
		board -> dataFlag = false;
	else board -> dataFlag = true;

	/* -------- </SET FLAG MEMBERS> --------- */

	/* ------ <INCREASE VIEW AND DATA> ------ */
	
	int index = board -> selected_index;
	index ++;
	clear_board (board);
	set_rowIndex (board, index);
	update_board (board);

	/* ------ </INCREASE VIEW AND DATA> ------ */

}

void scrollup_handler (BOARD_WIDGET* board) {

	/* ------- <SET FLAG MEMBERS> -------- */	

	if (board -> selected_index == 0)
		board -> wndFlag = false;
	else board -> wndFlag = true;

	if (board -> firstrow_index == 0)
		board -> dataFlag = false;
	else board -> dataFlag = true;

	/* ------- </SET FLAG MEMBERS> -------- */

	/* ------- <Decrease selected_index in the view> ------ */

	int index = board -> selected_index;
	index --;
	clear_board (board);
	set_rowIndex (board, index);
	update_board (board);

	/* ------- </Decrease selected_index in the view> ------ */

	/* ------ <Increase fistrow_index in the data> ------ */

	/* ------ </Increase firstrow_index in the data> ----- */

}

void pageup_handler (BOARD_WIDGET* board) {

	/* ------- <SET FLAG MEMBERS> -------- */	

	if (board -> selected_index == 0)
		board -> wndFlag = false;
	else board -> wndFlag = true;

	if (board -> firstrow_index == 0)
		board -> dataFlag = false;
	else board -> dataFlag = true;

	/* ------- </SET FLAG MEMBERS> -------- */

	int index = -1 * (int) (board -> wndTable -> len);
	clear_board (board);
	set_rowIndex (board, index);
	update_board (board);

}

void pagedown_handler (BOARD_WIDGET* board) {

	/* ------- <SET FLAG MEMBERS> --------  */		
	if (board -> selected_index == board -> wndTable -> len - 1)
		board -> wndFlag = false;
	else board -> wndFlag = true;

	if (board -> lastrow_index == board -> dataTable -> len - 1)
		board -> dataFlag = false;
	else board -> dataFlag = true;

	/* -------- </SET FLAG MEMBERS> --------- */

	int index = (int) (board -> wndTable -> len) * 2 - 1;
	clear_board (board);
	set_rowIndex (board, index);
	update_board (board);

}

void resize_handler (BOARD_WIDGET* board) {

	WINDOW* mainWnd = board -> mainWnd;
	WINDOW* headerWnd = board -> headerWnd;

	GPtrArray* wndTable = board -> wndTable;
	GPtrArray* dataTable = board -> dataTable;
	PRINT_HEADER_FUNC printHeader = board -> printHeader;
	PRINT_DATA_FUNC printData = board -> printData;
	guint firstrow_index = board -> firstrow_index;
	guint lastrow_index = board -> lastrow_index;

	int row = board -> row;
	int col = board -> col;
	int row_width = board -> row_width;
	int col_width = board -> col_width;
	chtype base_color = board -> base_color;
	chtype selected_color = board -> selected_color;
	guint selected_index = board -> selected_index;

	/* bool wndFlag;
	*  bool dataFlag;	*/

	refresh (); /* It's very essential!! */

	POINT_INFO origin_point_info;
	origin_point_info.origin_x = board -> origin_point_info -> origin_x;
	origin_point_info.origin_y = board -> origin_point_info -> origin_y;
	
	origin_point_info.base_color = board -> point_info -> base_color;			/* <get recent color infomation> */
	origin_point_info.selected_color = board -> point_info -> selected_color;   /* </get recent color information> */

	origin_point_info.x_from_origin = board -> origin_point_info -> x_from_origin;
	origin_point_info.y_from_origin = board -> origin_point_info -> y_from_origin;
	void* userdata = board -> userdata;
	del_board (board); /* delete board */

	board = new_board (board, row, col, row_width, col_width, &origin_point_info
						,dataTable, printHeader, printData, userdata); 

	guint length = board -> wndTable -> len;

	board -> firstrow_index = firstrow_index; 	/* remembers */
	/* board -> lastrow_index = lastrow_index; */	/* past board data */

	board -> lastrow_index = (board -> firstrow_index + ((int) length - 1));
	board -> lastrow_index = (board -> lastrow_index < ((int) (board -> dataTable -> len) - 1)) ?
								board -> lastrow_index : ((int) (board -> dataTable -> len) - 1);
	
	/* set_rowIndex (board, 0); */ /* update firstrow_index and lastrow_index !! : overflow critical window line - debug  */

	/* set_rowIndex (board, selected_index); */
	
	/* ------------- <SET selected_index> ----------------- */
	
	if (selected_index <= 0) {
		board -> selected_index = 0;
	}
	else if (selected_index > (int) length - 1) {
		board -> selected_index = length - 1;
	}
	else board -> selected_index = selected_index;
	
	/* ------------- </SET selected_index> ------------------ */

	set_selected_color (board, selected_color);
	set_base_color (board, base_color);
	bkgd (COLOR_PAIR (0));
	wrefresh (stdscr);

	update_board (board);

	refresh ();
}

void activate_board (BOARD_WIDGET* board) {

	int j;
	WINDOW** rowContainer;

	/* ------------- <VIEW UPDATE> --------------- */

	rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable, board -> selected_index);

	for (j = 0; j < board -> col; j++) {
		wbkgd (rowContainer [j], board -> selected_color | A_BOLD);
		wrefresh (rowContainer [j]);
	}
	wrefresh (board -> mainWnd); 

	/* ------------- </VIEW UPDATE> -------------- */

}

void inactivate_board (BOARD_WIDGET* board) {

	int j;
	WINDOW** rowContainer;

	/* ------------- <CLEAR VIEW> ------------- */
	rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable , board -> selected_index);
	for (j = 0; j < board -> col; j++) {
		wbkgd (rowContainer [j], board -> base_color | A_BOLD);
		wrefresh (rowContainer [j]);
	}
	/* ------------- </CLEAR VIEW> ------------- */


}


void set_colors (BOARD_WIDGET* board, chtype base_color, chtype selected_color) {

	WINDOW* mainWnd = board -> mainWnd;
	WINDOW* headerWnd = board -> headerWnd;

	GPtrArray* wndTable = board -> wndTable;
	GPtrArray* dataTable = board -> dataTable;
	PRINT_HEADER_FUNC printHeader = board -> printHeader;
	PRINT_DATA_FUNC printData = board -> printData;
	guint firstrow_index = board -> firstrow_index;
	guint lastrow_index = board -> lastrow_index;

	int row = board -> row;
	int col = board -> col;
	int row_width = board -> row_width;
	int col_width = board -> col_width;

	guint selected_index = board -> selected_index;

	/* bool wndFlag;
	*  bool dataFlag;	*/

	refresh (); /* It's very essential!! */

	POINT_INFO origin_point_info;
	origin_point_info.origin_x = board -> origin_point_info -> origin_x;
	origin_point_info.origin_y = board -> origin_point_info -> origin_y;
	
	origin_point_info.base_color = base_color;			/* <get recent color infomation> */
	origin_point_info.selected_color = selected_color;   /* </get recent color information> */

	origin_point_info.x_from_origin = board -> origin_point_info -> x_from_origin;
	origin_point_info.y_from_origin = board -> origin_point_info -> y_from_origin;

	void* userdata = board -> userdata;
	del_board (board); /* delete board */
	board = new_board (board, row, col, row_width, col_width, &origin_point_info
						,dataTable, printHeader, printData, userdata); 

	guint length = board -> wndTable -> len;

	board -> firstrow_index = firstrow_index; 	/* remembers */
	/* board -> lastrow_index = lastrow_index; */	/* past board data */

	board -> lastrow_index = (board -> firstrow_index + ((int) length - 1));
	board -> lastrow_index = (board -> lastrow_index < ((int) (board -> dataTable -> len) - 1)) ?
								board -> lastrow_index : ((int) (board -> dataTable -> len) - 1);
	
	/* set_rowIndex (board, 0); */ /* update firstrow_index and lastrow_index !! : overflow critical window line - debug  */

	/* set_rowIndex (board, selected_index); */
	
	/* ------------- <SET selected_index> ----------------- */
	
	if (selected_index <= 0) {
		board -> selected_index = 0;
	}
	else if (selected_index > (int) length - 1) {
		board -> selected_index = length - 1;
	}
	else board -> selected_index = selected_index;
	
	/* ------------- </SET selected_index> ------------------ */

	set_selected_color (board, selected_color);
	set_base_color (board, base_color);
	update_board (board);


	refresh ();
}

void board_eventhandler (BOARD_WIDGET* board, GNode* root) {
	
	int ch;
	int selected_data_index;
	TreeElement* temp;
	GNode* temp_node;
	GNode* market;
	int flag;
	int remember_index;
	int i;

	activate_board (board);

	while ((ch = getch ()) != 'q') {		
		switch (ch) {

			case KEY_UP :
				scrollup_handler (board);
				break;

			case KEY_DOWN :
				scrolldown_handler (board);
				break;

			case KEY_PPAGE :
				pageup_handler (board);
				break;

			case KEY_NPAGE :
				pagedown_handler (board);
				break;

			case KEY_RESIZE :
				/* clear_board (board); */
				resize_handler (board);
				break;
			
			case '\n' :

				selected_data_index = board -> firstrow_index +
					board -> selected_index;
				temp = (TreeElement*) g_ptr_array_index (
						board -> dataTable, selected_data_index);
				if ((temp -> state_info & BASIS_LEAF) == IS_NOT_LEAF) {
					market = g_node_find (root, G_LEVEL_ORDER, 
							G_TRAVERSE_NON_LEAVES, (gpointer) temp);
					temp_node = g_node_first_child (market);
					flag = (temp -> state_info & BASIS_OPENED);
					if (flag == IS_OPENED) {
						flag = IS_CLOSED;
					}
					else flag = IS_OPENED;

					open_close_branch (market, flag);

					clear_board (board);

					g_ptr_array_unref (board -> dataTable);
					board -> dataTable = node_to_array (root, 
							board -> dataTable);

					remember_index = board -> selected_index;
					set_rowIndex (board, 0);
					/*set_rowIndex (board, -1);*/
					set_rowIndex (board, remember_index);
					board -> wndFlag = true;
					board -> dataFlag = true;
					update_board (board);
				}
				break;
			case 'o' :
				option_handler (board);
				break;
		}
		usleep (500);
	}
	
	inactivate_board (board);

}


void del_board (BOARD_WIDGET* board) {
	WINDOW** rowContainer;
	int i, j;
	for (i = 0; i < board -> wndTable -> len; i++) {
		rowContainer = (WINDOW**) g_ptr_array_index (board -> wndTable, i);
		for (j = 0; j < board -> col; j++) {
	/*		wclear (rowContainer [j]);
			wrefresh (rowContainer [j]); */
			delwin (rowContainer [j]);
		}
	}
	
	wclear (board -> headerWnd);
	wrefresh (board -> headerWnd);
	delwin (board -> headerWnd);
	wclear (board -> mainWnd);
	wrefresh (board -> mainWnd);
	delwin (board -> mainWnd);
	g_ptr_array_free (board -> wndTable, TRUE);
	free (board -> point_info);
	free (board -> origin_point_info);
	free (board);
}

/* ------ <option handler> -------- */

void option_handler (BOARD_WIDGET* board) {
	
	int ch; 
	int i;
	while ((ch = getch ()) != '\n') {

	wdeleteln (board -> headerWnd);
	wmove (board -> headerWnd, 0, 0);
	wattron (board -> headerWnd, A_BOLD);
	wprintw (board -> headerWnd, "set options [bs]");
	wrefresh (board -> headerWnd);

		switch (ch) {
			case 'b' :
				i = 0;
				werase (board -> headerWnd);
				wbkgd (board -> headerWnd, COLOR_PAIR (i) | A_BOLD);
				wprintw (board -> headerWnd, "Base_col [0-7] : %d", i);
				wrefresh (board -> headerWnd);
				
				while ((ch = getch ()) != '\n') {
					switch (ch) {
						case KEY_UP :
							i = (++i < 8) ? i : 7;
							werase (board -> headerWnd);
							wbkgd (board -> headerWnd, COLOR_PAIR (i) | A_BOLD);
							wprintw (board -> headerWnd, "Base_color [0-7] : %d", i);
							wrefresh (board -> headerWnd);
							break;

						case KEY_DOWN :
							i = (--i > 0) ? i : 0;
							werase (board -> headerWnd);
							wbkgd (board -> headerWnd, COLOR_PAIR (i) | A_BOLD);
							wprintw (board -> headerWnd, "Base_color [0-7] : %d", i);
							wrefresh (board -> headerWnd);
							break;
					}
					usleep (1000);
				}
				set_colors (board, COLOR_PAIR (i), board -> point_info -> selected_color);
				break;

			case 's' :
				i = 0;
				werase (board -> headerWnd);
				wbkgd (board -> headerWnd, COLOR_PAIR (i) | A_BOLD);
				wprintw (board -> headerWnd, "selected_color [0-7] : %d", i);
				wrefresh (board -> headerWnd);
				
				while ((ch = getch ()) != '\n') {
					switch (ch) {
						case KEY_UP :
							i = (++i < 8) ? i : 7;
							werase (board -> headerWnd);
							wbkgd (board -> headerWnd, COLOR_PAIR (i) | A_BOLD);
							wprintw (board -> headerWnd, "selected_color [0-7] : %d", i);
							wrefresh (board -> headerWnd);
							break;

						case KEY_DOWN :
							i = (--i > 0) ? i : 0;
							werase (board -> headerWnd);
							wbkgd (board -> headerWnd, COLOR_PAIR (i) | A_BOLD);
							wprintw (board -> headerWnd, "selected_color [0-7] : %d", i);
							wrefresh (board -> headerWnd);
							break;
					}
					usleep (1000);
				}
				set_colors (board, board -> point_info -> base_color, COLOR_PAIR (i));
				break;

		}
		usleep (1000);
	}
	resize_handler (board);
}
/* ------ </option handler> -------- */

