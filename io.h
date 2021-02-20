#ifndef IO_H
#define IO_H

#include <stdio.h>
#define DIALOGS_SIZE	0X08

void 		printerr(const char*, ...);
int		getKey(void);
/*Buffer must be alloted for string, buffer's size must be equaled at least 
 * word_size.*/
char*		getnstr(char*, size_t);
/*Return zero, if it's succesfuly compled, 1 - input's error, (-1) - invalid input(not number)*/
int		getuint(unsigned long*);
char*		initFormatList(unsigned int);
char*		initUnderline(unsigned int);
char*		initSelectMenu(void);
char**		initDialogSet(void);


#endif
