#include "testio.h"
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#define MENU	"g Generate new base\n"
#define KEYSET	"gnefdsrvcq"
#define BUFSIZE	0x100

char* initTestMenu(void)	{
	size_t size = strlen(MENU)+1;
	char *new = malloc(size);
	strncpy(new, MENU, size);
	return new;
}

int getTestKey(void)	{
	char buffer[BUFSIZE];
	int res;
	fgets(buffer, BUFSIZE, stdin);
	res = (int)tolower(buffer[0]);
	return strchr(KEYSET, res) ? res : (-1); 
}
