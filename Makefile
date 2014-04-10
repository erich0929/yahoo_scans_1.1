all : stockapi/stockapi.o boardwidget/boardwidget.o treeapi/treeapi.o yahoo_scans.o
	gcc -g -o yahoo_scans $^ -lform -lncurses -lcurl `pkg-config --cflags --libs glib-2.0`

stockapi/stockapi.o :
	cd stockapi && make

boardwidget/boardwidget.o :
	cd boardwidget && make

treeapi/treeapi.o :
	cd treeapi && make

yahoo_scans.o : yahoo_scans.c
	gcc -g -c -o $@ $^ `pkg-config --cflags glib-2.0`

header :
	bash ./shell/makeKospiInfo.sh && bash ./shell/makeNyseInfo.sh

clean :
	rm -rf *.o yahoo_scans
	cd stockapi && make clean
	cd boardwidget && make clean
	cd treeapi && make clean
