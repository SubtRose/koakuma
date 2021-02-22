#include "io.h"

#include <ctype.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#define	BUFINPUTSIZE	0X800
#define ASCIIOFS	0x30
#define CHKEYS		"befdsrvcq"
#define COLONSIZE	0X08
#define PRETABS		"%%%ds%%%ds%%%ds%%%ds%%%ds%%%ds%%%ds%%%ds\n"
#define	PREFORMATLIST	"%%%dd%%%ds%%%ds%%%ds%%%ds%%%dd%%%dd%%%dd\n"
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

static int verify_chkey(int, const char* set_key);
static unsigned long get_bufsize(unsigned long);

void printerr(const char* msg, ...)	{
	va_list args;
	va_start(args, msg);
	fprintf(stderr, msg, args);
	va_end(args);
}

/*Return negative number, if no function is succesfully completed.*/
int getChkey(void)	{
	char input[BUFINPUTSIZE], *p = NULL;
	int res;
	p = fgets(input, BUFINPUTSIZE, stdin);
	res = (int)tolower(*p);
	if(verify_chkey(res, CHKEYS))
		res = -1;
	return res;
}

int yesOrNo(void)	{
	char input[BUFINPUTSIZE];
	int res;
	fgets(input, BUFINPUTSIZE, stdin);
	res = tolower(input[0]);
	if(res=='y')	return 1;
	if(res=='n')	return 0;
	else		return -1;
}

/*Buffer must be alloted for string, buffer's size must be equaled at least 
 * word_size.*/
char* getnstr(char* buf, size_t word_size)	{
	return fgets(buf, word_size, stdin);
}

int getuint(unsigned long* num)	{
	char input[BUFINPUTSIZE], *p=NULL;
	p = getnstr(input, BUFINPUTSIZE);
	if(!p)
		return 1;	
	return atold(num, p);
}

static unsigned long get_bufsize(unsigned long ws)	{
	return (4*COLONSIZE + 4*ws + 2);
}
char* initTabs(unsigned int word_size)	{
	unsigned long bs = get_bufsize(word_size);
	char* pretabs = calloc(bs, 1);
	char* tabs = calloc(bs, 1);
	if(pretabs)	{
		snprintf(pretabs,bs, PRETABS, COLONSIZE, word_size, word_size, word_sizeword_size, COLONSIZE,COLONSIZE,COLONSIZE, COLONSIZE);
		if(tabs)	{
			snprintf(tabs, "ID", "Surname", "Name", "Patronymic", "Post", "h/pay", "Hours", "Salary");
		}
		free(pretabs);
		pretabs=NULL;
	}
	return tabs;
}
char* initFormatList(unsigned int word_size)	{
	unsigned long bs = get_bufsize(word_size);
	char* format = calloc(sizeof(char), bs);
	if(format)	{
		snprintf(format, bs, PREFORMATLIST, COLONSIZE, word_size, word_size, word_size, word_size, COLONSIZE, COLONSIZE, COLONSIZE);
	}
	return format;
}

char* initUnderline(unsigned int word_size)	{
	unsigned long bs = get_bufsize(word_size);
	char* underline = calloc(bs,1);
	if(underline)	{
		memset(underline, '_', bs-2);
		underline[bs-1] = 0xa;
	}
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
static int verify_chkey(int ch, const char* set_keys)	{
	return !((ptrdiff_t)strchr(set_keys, ch));
}
/*atold() return -1, in case const char* w isn't a number.*/
int atold(unsigned long* num, char* str)	{
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
