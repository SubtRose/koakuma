#include "fileio.h"

#include <malloc.h>


FILE* createdb(const char *fname)	{
	FILE* res=NULL;	
	res = fopen(fname, "rb");
	if(res)	{
		fclose(res);
	}
	else	{
		res = fopen(fname, "wb+");
	}
	return res;
}

FILE* opendb(const char* fname)	{
	return fopen(fname, "rb+");
}

void closedb(FILE* db)	{
	if(db)
		fclose(db);
}

int writeToFile(FILE* db, const void* data, size_t nbytes)	{
	return (int)fwrite(data, nbytes, 1, db);
}

int readFromFile(FILE* db, void* data, size_t nbytes)	{
	return (int)fread(data, nbytes,1, db);
}
