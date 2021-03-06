#include "testio.h"
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#define MENU	"g Generate new base\nt Launch test\n"
#define KEYSET	"nefdsrvcqgt"
#define BUFSIZE	0x100

char* initTestMenu(void)	{
	size_t size = strlen(MENU)+1;
	char *new = malloc(size);
	strncpy(new, MENU, size);
	return new;
}

int getTestKey(void)	{
	char buffer[BUFSIZE];
	int res;
	fgets(buffer, BUFSIZE, stdin);
	res = (int)tolower(buffer[0]);
	return strchr(KEYSET, res) ? res : (-1); 
}

 
resultTest	*initTest(const char *sort)	{
	resultTest *new;
	unsigned long sizeName = NAMEFUNC_MAX;

	new = (resultTest*)calloc(sizeof(resultTest),1);
	if(new)	{
		strncpy(new->sortingFunc, sort, sizeName);
	}

	return new;
}

static double get_aver_rate(rateInfo* p)	{
	unsigned int n = p->tryN;
	double *dp = p->listRates, *max = dp + n, res;
	for(res=0; dp<max; dp++)	{
		res += *dp;
	}
	return (res/n);
}

void	addRate(resultTest* target, unsigned int sizeDB, double *newrate, unsigned int ntry)	{
	rateInfo *dp;
	unsigned int listsize;

	dp = target->listTest;
	listsize = target->listTestSize;
	dp = (rateInfo*)realloc(dp, (++listsize) * sizeof(rateInfo));
	target->listTest = dp;
	target->listTestSize = listsize;

	dp = &((target->listTest)[listsize-1]);
	dp->sizeBase = sizeDB;
	dp->tryN = ntry;
	memmove(dp->listRates, newrate, NUMBER_TRIES);
	dp->averageRate = get_aver_rate(dp);
}

void	printResult(resultTest *target)	{
	unsigned int i, iMax;
	rateInfo* crt;

	i=0;
	iMax = target->listTestSize;
	printf("%s\t", target->sortingFunc);
	for(; i<iMax; i++)	{
		crt = &(target->listTest)[i];
		printf("%f (%u)\t", crt->averageRate, crt->sizeBase);
	}
	printf("\n");
}

void	removeResult(resultTest *target)	{
	if(target->listTestSize)	{
		free(target->listTest);
		target->listTest=NULL;
	}
	free(target);
}
