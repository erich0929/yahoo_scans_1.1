#ifndef BOARDWIDGET_H

#define BOARDWIDGET_H

#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include "../treeapi/treeapi.h"
#include "../stockapi/stockapi.h"

typedef struct _POINT_INFO {

	int origin_x;
	int origin_y;
	chtype base_color;
	chtype selected_color;
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

	void* userdata;
} BOARD_WIDGET;

BOARD_WIDGET* new_board (BOARD_WIDGET* board, int row, int col, int row_width, int col_width, POINT_INFO* point_info, GPtrArray* dataTable, PRINT_HEADER_FUNC printHeader, PRINT_DATA_FUNC printData, void* userdata);

void set_base_color (BOARD_WIDGET* board, chtype color);
void set_selected_color (BOARD_WIDGET* board, chtype color);
void set_rowIndex (BOARD_WIDGET* board, int index);
void clear_board (BOARD_WIDGET* board);
void update_board (BOARD_WIDGET* board);
void scrolldown_handler (BOARD_WIDGET* board);
void scrollup_handler (BOARD_WIDGET* board);
void pageup_handler (BOARD_WIDGET* board);
void pagedown_handler (BOARD_WIDGET* board);
void resize_handler (BOARD_WIDGET* board);
void activate_board (BOARD_WIDGET* board);
void inactivate_board (BOARD_WIDGET* board);
void set_colors (BOARD_WIDGET* board, chtype base_color, chtype selected_color);
void board_eventhandler (BOARD_WIDGET* board, GNode* root);
void del_board (BOARD_WIDGET* board);

void option_handler (BOARD_WIDGET* board);
void init_scr ();
#endif /* end of include guard: BOARDWIDGET_H */


