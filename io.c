#include "io.h"

#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#define	BUFINPUTSIZE	0X800
#define KEYS		"befdsrvcq"
#define PREFORMATLIST	"%%8.lu%%%udlu%%%udlu%%%udlu\
			%%udlu%%8.lu%%8.lu%%8.lu\n"	

void errorMessage(char* msg, ...)	{
	va_list args;
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
	return;
}

int dialog(entryIO* io)	{
	int res, retv = 0;
	puts("Enter data:\n");
	if(io->pID)	{
		puts("id:");
		res = scanf("%lu", io->pID);
		/*analys a res*/
	}
	if(io->surname)	{
		puts("Surname:");
		io->surname = fgets(io->surname, io->word_s, stdin);

	}
	if(io->name)	{
		puts("Name:");
		io->name = fgets(io->name, io->word_s, stdin);
	}
	if(io->patronymic)	{
		puts("patronymic:");
		io->patronymic = fgets(io->patronymic, io->word_s, stdin);
	}
	if(io->post)	{
		puts("Post:");
		io->post = fgets(io->post, io->word_s, stdin);
	}
	if(io->p_hr_pay)	{
		puts("Hr.pay:");
		res = scanf("%lu", io->p_hr_pay);
		/*ananlys a res*/
	}	
	if(io->p_hrs)	{
		puts("Hours:");
		res = scanf("%lu",io->p_hrs);
		/*ananlys a res*/
	}
	return retv;
}

int getKey(void)	{
	char input[BUFINPUTSIZE], *p = NULL;
	p = fgets(input, BUFINPUTSIZE, stdin);
	/*analys a ptr p*/
	return *p;
}

int verifyInput(const char* keys, int ch)	{
	return (ptrdiff_t)strchr(keys, ch);
}

char* initFormatList(unsigned int word_size)	{
	unsigned long s = strlen(PREFORMATLIST) + 0x40;
	char *format = calloc(s, sizeof(char));
	if(format)
		snprintf(format, s, PREFORMATLIST, 
				word_size, word_size, word_size, word_size);
	return format;
}

char* initUnderline(unsigned int underline_length)	{
	char* underline = calloc(sizeof(char), underline_length + 1);
	if(underline)	
		underline = memset(underline, '_', underline_length);
	return underline;
}

entryIO* initPackIO(void)	{
	return (entryIO*)calloc(sizeof(entryIO), 1);
}

/*******************************************************************************/
