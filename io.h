#ifndef IO_H
#define IO_H

#include <stdio.h>

void		errorMessage(char*, ...);
int		getKey(void);
char*		getnstr(char*, size_t);
int		getuint(unsigned long*);
char*		initFormatList(unsigned int);
char*		initUnderline(unsigned int);
char*		initSelectMenu(void);
char**		initDialogSet(void);


#endif
