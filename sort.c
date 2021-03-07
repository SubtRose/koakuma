#include "sort.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define FARR_SIZE	0X08

typedef int (*compare)(const void*, const void*);

static compare	get_fcmp(attributes);
static void 	init_fcmps(compare*);

static int cmp_pos(const void*, const void*);
static int cmp_fname(const void*, const void*);
static int cmp_sname(const void*, const void*);
static int cmp_lname(const void*, const void*);
static int cmp_id(const void*, const void*);
static int cmp_pph(const void*, const void*);
static int cmp_hrs(const void*, const void*);
static int cmp_pay(const void*, const void*);

static int cmp_uint(unsigned long, unsigned long);

void bubbleSort(database* db, attributes attr)	{
	unsigned long nitems = db->occupiedMem / sizeof(entry), i;
	int sorted = 0;
	compare cmp;
	entry	*head 	= db->headEntry,
		*ptr_j	= NULL,
		*ptr_k	= NULL,
		*ptr_lim= head + nitems;
		;
	cmp = get_fcmp(attr);

	for(i = 0; i < nitems && !sorted; i++)	{
		sorted = 1;
		for(ptr_j = head + 1; ptr_j < ptr_lim; ptr_j++)	{
			ptr_k = ptr_j - 1;
			if(cmp(ptr_k, ptr_j) > 0)	{
				swapEntries(ptr_k, ptr_j);
				sorted = 0;
			}	
		}
	}

	return;
}

static unsigned long upower(unsigned long num, unsigned long pow)	{
	if(!pow)
		return 1;
	for(unsigned long i=1; i<pow; i++)	
		num *= num;
	return num;
}

static unsigned long* hibbardMethod(unsigned long n)	{
	unsigned long *resr, *res, s, d, ulong_s, i, j;	

	i=0;
	s=0;
	ulong_s = sizeof(unsigned long);
	resr=NULL;
	while(1)	{
		d = upower(2, i) - 1;
		if(d<n)	{
			++s;
			resr = (unsigned long*)realloc(resr, s * ulong_s);
			resr[i] = d;
			++i;
		}
		else break;
	}

	res = (unsigned long*)malloc(s * ulong_s);
	i=0;
	j=s-1;
	while(i<s)	{
		res[i] = resr[j];
		i++;
		j--;
	}

	free(resr);
	return res;
}

void shellSort(database* db, attributes attr)	{
	/**************************************************************************/
	/*GET A NUMBER OF ENTRIES IN BASE*/
	/*SET A SEQUENSE OF GAPS BY HIBBARD-METHOD*/
	/*FETCH A COMPARE-FUNCTION BY ATTR(CMP=)*/
	/*FOREACH(GAP):	*/
		/*FOR(I=0; I<N; I++):	*/
			/*FOR(J=I+GAP; J<N; J += GAP):	*/
				/*CMP(BASE[I], BASE[J])*/
				/* IF CMP(..)>0 THEN SWAP(BASE[I], BASE[J])*/
	/**************************************************************************/

	unsigned long N, *gaps, *pgaps, gap;
	compare cmp;
	entry *ptrI, *ptrJ, *head, *lim;
	int res;

	N = getSizeDB(db);
	gaps = hibbardMethod(N);
	cmp = get_fcmp(attr);

	head = getHeadDB(db);
	lim = getTailDB(db) + 1;
	for(pgaps = gaps; *pgaps; pgaps++)	{
		gap = *pgaps;
		for(ptrI = head; ptrI<lim; ptrI++)	{
			for(ptrJ = ptrI + gap; ptrJ<lim; ptrJ += gap)	{
				res = cmp(ptrI, ptrJ);
				if(res>0)	{
					swapEntries(ptrI, ptrJ);
				}
			}
		}
	}

	free(gaps);
}


void quickSort(database* db, attributes attr)	{
	unsigned int nitems = db->occupiedMem / sizeof(entry);
	unsigned long entry_s = sizeof(entry);
	entry* ptr_head = db->headEntry;
	compare cmp = NULL;

	cmp = get_fcmp(attr);
	qsort(ptr_head, nitems, entry_s, cmp);

	return;
}

static compare get_fcmp(attributes attr)	{
	compare farr[FARR_SIZE];
	init_fcmps(farr);
	return farr[attr];
}

static void init_fcmps(compare* arr)	{
	arr[pos]	= cmp_pos;
	arr[name1]	= cmp_fname;
	arr[name2]	= cmp_sname;
	arr[name3]	= cmp_lname;
	arr[payperh]	= cmp_pph;
	arr[hrs]	= cmp_hrs;
	arr[id]		= cmp_id;
	arr[pay]	= cmp_pay;
	return;
}

static int cmp_pos(const void* ent0, const void* ent1)	{
	return strncmp(((entry*)ent0)->position, ((entry*)ent1)->position, WORD_SIZE);
}
static int cmp_fname(const void* ent0, const void* ent1)	{
	return strncmp(((entry*)ent0)->firstName, ((entry*)ent1)->firstName, WORD_SIZE);
}
static int cmp_sname(const void* ent0, const void* ent1)	{
	return strncmp(((entry*)ent0)->secondName, ((entry*)ent1)->secondName, WORD_SIZE);
}
static int cmp_lname(const void* ent0, const void* ent1)	{
	return strncmp(((entry*)ent0)->lastName, ((entry*)ent1)->lastName, WORD_SIZE);
}
static int cmp_id(const void* ent0, const void* ent1)	{
	return cmp_uint(((entry*)ent0)->ID, ((entry*)ent1)->ID);
}
static int cmp_pph(const void* ent0, const void* ent1)	{
	return cmp_uint(((entry*)ent0)->payPerHour, ((entry*)ent1)->payPerHour);
}
static int cmp_hrs(const void* ent0, const void* ent1)	{
	return cmp_uint(((entry*)ent0)->hours, ((entry*)ent1)->hours);
}
static int cmp_pay(const void* ent0, const void* ent1)	{
	return cmp_uint(((entry*)ent0)->pay, ((entry*)ent1)->pay);
}

/*if s0<s1 strcmp(s0,s1) return negative, else - positive;*/
static int cmp_uint(unsigned long val0, unsigned long val1)	{
	return (int)(val0 - val1);
}
