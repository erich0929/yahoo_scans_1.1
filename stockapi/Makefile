TARGET	:= stockapi.o
OBJS	:= $(wildcard *.c)

all :
		gcc -g -c -o $(TARGET) $(OBJS) `pkg-config --cflags glib-2.0`

clean :
		rm -rf *.o $(TARGET)
