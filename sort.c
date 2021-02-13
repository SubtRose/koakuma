#include "sort.h"

#include "database.h"
#include <string.h>
#define CMPFUNS_N	0X08

typedef (int)(*compare_f)(entry*, entry*);

static void init_cmpfuns(compare_f**);

static int cmp_pos(entry*, entry*);
static int cmp_fname(entry*, entry*);
static int cmp sname(entry*, entry*);
static int cmp_lname(entry*, entry*);
static int cmp_id(entry*, entry*);
static int cmp_pph(entry*, entry*);
static int cmp_hrs(entry*, entry*);
static int cmp_pay(entry*, entry*);

static int cmp_uint(unsigned long, unsigned long);

void bubbleSort(database* db, attributes attr)	{
	if(!((db->headEntry) && occupiedMem))
		return;
	compare_f *cmpf[CMPFUNS_N];
	init_cmpfuns(cmpf);
	compare_f cmp =  cmpf[attr];
	unsigned long nitems = allMem / occupiedMem;
	entry*	ptr = db->headEntry;
	int i,j, sorted = 0;
	for(i = 0; i < nitems && !sorted; i++)	{
		sorted = 1;
		for(j = 1; j < nitems; j++)	{
			cmp(ptr+j-1, ptr+j);
			if(cmp(ptr+j-1, ptr+j) > 0)	{
				swapEntries(ptr+j-1, ptr+j);
				sorted = 0;
			}	
		}
	}
	cmp = ptr = NULL;
	return;
}

static void init_cmpfuns(compare_f** arr)	{
	arr[pos] = cmp_pos;
	arr[name1] = cmp_fname;
	arr[name2] = cmp_sname;
	arr[name3] = cmp_lname;
	arr[payperh] = cmp_pph;
	arr[hrs] = cmp_hrs;
	arr[id] = cmp_id;
	arr[pay]  = cmp_pay;
	return;
}

static int cmp_pos(entry* ent0, entry* ent1)	{
	return strncmp(ent0->pos, ent1->pos, WORD_SIZE);
}
static int cmp_fname(entry* ent0, entry* ent1)	{
	return strncmp(ent0->firstName, ent1->firstName, WORD_SIZE);
}
static int cmp_sname(entry* ent0, entry* ent1)	{
	return strncmp(ent0->secondName, ent1->secondName, WORD_SIZE);
}
static int cmp_lname(entry* ent0, entry* ent1)	{
	return strncmp(ent0->lastName, ent1->lastName, WORD_SIZE);
}
static int cmp_id(entry* ent0, entry* ent1)	{
	return cmp_uint(ent0->ID, ent1->ID);
}
static int cmp_pph(entry* ent0, entry* ent1)	{
	return cmp_uint(ent0->payPerHour, ent1->payPerHour);
}
static int cmp_hrs(entry* ent0, entry* ent1)	{
	return cmp_uint(ent0->hours, ent1->hours);
}
static int cmp_pay(entry* ent0, entry* ent1)	{
	return cmp_uint(ent0->pay, ent1->pay);
}

/*if s0<s1 strcmp(s0,s1) return negative, else - positive;*/
static int cmp_uint(unsigned long val0, unsigned long val1)	{
	return (int)(val0 - val1);
}


