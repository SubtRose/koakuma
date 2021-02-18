#ifndef IO_H
#define IO_H

#include <stdio.h>
#define DIALOGS_SIZE	0X08

void 		printerr(const char*, ...);
int		getKey(void);
char*		getnstr(char*, size_t);
int		getuint(unsigned long*);
char*		initFormatList(unsigned int);
char*		initUnderline(unsigned int);
char*		initSelectMenu(void);
char**		initDialogSet(void);


#endif
