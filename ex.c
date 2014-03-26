#include <stdio.h>

typedef struct ex_t {
	char* ch;
} ext;

ext var = {"robert"};

int main(void)
{
	ext* temp = &var;
	temp -> ch = "kim";
	printf ("%s\n", temp -> ch);
	return 0;
}
