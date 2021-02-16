#ifndef IO_H
#define IO_H

#include <stdio.h>

#define	MENU	"Choose action:\n\
			N New worker.\n\
			E Edit data.\n\
			F Find worker.\n\
			D Delete data.\n\
			S Save to file.\n\
			R Load from file.\n\
			V Show all workers.\n\
			C Sort data.\n\
			Q Quit.\n"

typedef struct _entry_io	{
	unsigned long	*pID;
	char		*surname;
	char		*name;
	char		*patronymic;
	char		*post;
	unsigned long	*p_hr_pay;
	unsigned long	*p_hrs;
	unsigned long	word_s;
} entryIO;

void		errorMessage(char*, ...);		
/*buffers must be alloted!!!, if ptr==NULL, then data will not passed!*/
int 		dialog(entryIO*);
int		getKey(void);
int		verifyInput(const char*, int);
char*		initFormatList(unsigned int);
char*		initUnderline(unsigned int);
entryIO*	initPackIO(void);

#endif
