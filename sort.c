#include "sort.h"

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define FARR_SIZE	0X08

typedef int (*compare)(const void*, const void*);
typedef struct _nums_seq {
	unsigned int* seq;
	unsigned int seq_s;
} nums_seq;
static nums_seq* hibbard_method(unsigned int);

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

static void inverse_arr(void*, size_t, size_t);

void bubbleSort(database* db, attributes attr)	{
	unsigned long nitems = db->allMem / db->occupiedMem, i;
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

void shellSort(database* db, attributes attr)	{
	unsigned int gap, *pseq=NULL, *seq_lim = NULL;
	compare cmp=NULL;
	nums_seq* seq_gaps = NULL;
	entry	*ptr_head	= db->headEntry,
		*ptr_i		= NULL,
		*ptr_gap	= NULL,
		*ptr_lim	= ptr_head + db->occupiedMem;
		;

	seq_gaps = hibbard_method((unsigned int)(db->allMem / db->occupiedMem));
	if(!seq_gaps)
		return;
	cmp = get_fcmp(attr);
	pseq = seq_gaps->seq;
	pseq = pseq + seq_gaps->seq_s;
	for(; pseq < seq_lim; pseq++)	{
		gap = *pseq;
		for(ptr_i = ptr_head;; ptr_i++)	{
			ptr_gap = ptr_i + gap;
			if(ptr_gap > ptr_lim)
				break;
			if(cmp(ptr_i, ptr_gap))
				swapEntries(ptr_i, ptr_gap);			
		}
	}

	free(seq_gaps);
	return;
}

static void inverse_arr(void* arr, size_t membn, size_t size)	{
	size_t arr_s = membn*size;
	char *ptr = arr;
	char *tmp = malloc(arr_s), *tmp_r=NULL;
	if(!tmp)
		return;
	memmove(tmp, ptr, arr_s);
	tmp_r += (arr_s - size);
	while(1)	{
		memmove(ptr, tmp_r, size);
		ptr += size;
		tmp_r -= size;
		if(tmp_r < tmp)
			break;
	}
	free(tmp);
	return;
}

static nums_seq* hibbard_method(unsigned int n)	{
	unsigned int *arr_rev = NULL, d;
	unsigned long s = 1, i = 0, uint_s = sizeof(unsigned int);
	nums_seq* res = NULL;

	while(1)	{
		d = (2^s) - 1;
		if(d>n)
			break;
		arr_rev = (unsigned int*)realloc(arr_rev, s * uint_s);
		if(!arr_rev)	{
			s = 0;
			break;
		}
		arr_rev[i] = d;
		i++;
		s++;
	}
	if(s)	{
		res = (nums_seq*)malloc(sizeof(nums_seq));
		if(res)	{
			inverse_arr(arr_rev, s, uint_s);
			res->seq = arr_rev;
			res->seq_s = s;
		}
	}

	return res;
}

void quickSort(database* db, attributes attr)	{
	unsigned int nitems = db->allMem / db->occupiedMem;
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
