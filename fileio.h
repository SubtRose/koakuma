#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>

/*createdb() make a file, and open it.
 * If this file already exists, it will return NULL.*/
FILE*	createdb(const char*);
/*Function opendb() open an existed file. If this file doesn't exists, 
 * function return NULL.*/
FILE*	opendb(const char*);
size_t	getsizefile(FILE*);
void 	remdb(const char*);
void	closedb(FILE*);
/*Next two functions return written/read bytes.*/
int	writeToFile(FILE*, const void*, size_t);
int	readFromFile(FILE*, void*, size_t);

#endif

