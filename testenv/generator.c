#include "testenv/generator.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define ALPHA_SIZE	('z'-'a'+1)
#define MAXINT		1000000
#define DRAND		(RAND_MAX / ALPHA_SIZE)
#define DUINT		(RAND_MAX / MAXINT)

static char rand_char(void)	{
	unsigned long res;
	res = rand();
	res /= DRAND;
	res += 'a';
	return (char)((rand()%2) ?tolower(res):toupper(res));
}

static unsigned long rand_uint(void)	{
	return (unsigned long)(rand() / DUINT);
}

static void rand_string(char *dest)	{
	register char *lim = dest + WORD_SIZE - 2, *ptr = dest;
	memset(dest+WORD_SIZE-1, 0, 1);
	for(; ptr<lim; ptr++)	{
		*ptr = rand_char();
	}
}

static entry* random_entry(void)	{
	entry *new = malloc(sizeof(entry));
	if(new)	{
		new->ID = rand_uint();
		new->payPerHour = rand_uint();
		new->hours = rand_uint();
		new->pay = new->payPerHour * new->hours;
		rand_string(new->position);
		rand_string(new->firstName);
		rand_string(new->secondName);
		rand_string(new->lastName);
	}
	return new;
}

database* generateBase(unsigned long base_size)	{
	/*create new database*/	
	database *db = makeDatabase();
	entry *ptr=NULL, *pbuff=NULL;
	srand(time(NULL));
	for(register unsigned long i=0; i<base_size; i++)	{
		if(!enoughMem(db))	{
			pageRealloc(db, 1);
		}
		ptr = newEntry(db);
		pbuff = random_entry();
		copyEntries(ptr, pbuff);
		free(pbuff);
		pbuff=NULL;
	}
	bindToDB(db);
	return db;
}

