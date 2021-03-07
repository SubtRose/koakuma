#ifndef TESTIO_H
#define TESTIO_H
#include <time.h>
#define NUMBER_TRIES 0x08
#define NAMEFUNC_MAX 0X10

typedef struct _rate_info	{
	unsigned int	sizeBase;
	unsigned int	tryN;
	clock_t		listRates[NUMBER_TRIES];
	double		averageRate;
} rateInfo;

typedef struct _result_test	{
	char		sortingFunc[NAMEFUNC_MAX];
	rateInfo*	listTest;
	unsigned int	listTestSize;
} resultTest;
resultTest	*initTest(const char *);
void		addRate(resultTest*, unsigned int size, clock_t *newrate, unsigned int);
void		printResult(resultTest*);
void		removeResult(resultTest*);

char* initTestMenu(void);
/*return negative value of unsuccesfulled case*/
int getTestKey(void);

#endif
