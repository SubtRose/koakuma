#include "io.h"

#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#define	BUFINPUTSIZE	0X800
#define ASCIIOFS	0x30
#define KEYS		"befdsrvcq"
#define PREFORMATLIST	"%%8.lu%%%udlu%%%udlu%%%udlu%%udlu%%8.lu%%8.lu%%8.lu\n"	
#define	SELECTMENU	"Select an action:\n\
			N New worker.\n\
			E Edit data.\n\
			F Find worker.\n\
			D Delete data.\n\
			S Save to file.\n\
			R Load from file.\n\
			V Show all workers.\n\
			C Sort data.\n"
#define	DIA1		"Enter data:\n"
#define DIA2		"id:"
#define DIA3		"Surname:"
#define DIA4		"Name:"
#define DIA5		"Patronymic:"
#define DIA6		"Post:"
#define DIA7		"Hr.pay:"
#define DIA8		"Hours:"

static int verify_chkey(int);
static int atold(unsigned long*, char*);

void printerr(const char* msg, ...)	{
	va_list args;
	va_start(args, msg);
	fprintf(stderr, msg, args);
	va_end(args);
}

/*Return negative number, if no function is succesfully completed.*/
int getKey(void)	{
	char input[BUFINPUTSIZE], *p = NULL;
	int res;
	p = fgets(input, BUFINPUTSIZE, stdin);
	res = (int)*p;
	if(verify_chkey(res))
		res = -1;
	return res;
}

/*Buffer must be alloted for string, buffer's size must be equaled at least 
 * word_size.*/
char* getnstr(char* buf, size_t word_size)	{
	return fgets(buf, word_size, stdin);
}

int getuint(unsigned long* num)	{
	int err_code;
	char input[BUFINPUTSIZE], *p=NULL;
	p = getnstr(input, BUFINPUTSIZE);
	err_code = atold(num, p);
	return err_code;
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

char* initSelectMenu(void)	{
	size_t res_s = strlen(SELECTMENU)+1;
	char* res = calloc(sizeof(char), res_s);
	if(res)	{
		res = memmove(res, SELECTMENU, res_s);
	}
	return res;
}

char** initDialogSet(void)	{
	char **res = malloc(sizeof(char*) * DIALOGS_SIZE);
	if(res)	{
		res[0]=DIA1;
		res[1]=DIA2;
		res[2]=DIA3;
		res[3]=DIA4;
		res[4]=DIA5;
		res[5]=DIA6;
		res[6]=DIA7;
		res[7]=DIA8;
	}
	return res;
}
/*******************************************************************************/
static int verify_chkey(int ch)	{
	return !((ptrdiff_t)strchr(KEYS, ch));
}
/*atold() return -1, in case const char* w isn't a number.*/
static int atold(unsigned long* num, char* str)	{
	int err_code, no_err;
	unsigned long num_cp = *num;
	char* p=NULL;

	*num=0;
	err_code=0;
	for(p = str; *p && (no_err = isdigit(*p)); p++)	{
		*num = *num * 10 + (*p - ASCIIOFS);	
	}
	if(!no_err)	{
		err_code = -1;
		*num = num_cp;
	}

	return err_code;
}
