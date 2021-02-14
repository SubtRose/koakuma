#include "database.h"

#include <malloc.h>
#include <string.h>
#include <unistd.h>

static void		set_char_data(entry*, char*, attributes);
static void		set_uint_data(entry*, unsigned long, attributes);
static char*		get_char_data(entry*, attributes);
static unsigned long*	get_uint_data(entry*, attributes);

/**********************************************************************************/

database* makeDatabase(void)	{
	 return (database*)calloc(sizeof(database), 1);
}

void demakeDatabase(database* db)	{
	if(db->headEntry)	{
		free(db->headEntry);
		db->headEntry = NULL;
	}
	free(db);
	return;
}

entry* newEntry(database* db)	{
	entry* ptr = db->headEntry;
	unsigned long entry_s = sizeof(entry);
	ptr += (db->occupiedMem / entry_s);
	db->occupiedMem += entry_s;
	ptr->DB = db;
	return ptr;
}

entry* remEntry(entry* ent)	{
	database* db = ent->DB;
	unsigned long entry_s = sizeof(entry);
	entry* tail_ent = db->headEntry + db->occupiedMem / entry_s;
	swapEntries(ent, tail_ent);
	db->occupiedMem -= entry_s;
	db = NULL;
	return (tail_ent - 1);
}

void setData(entry* ent, void* newdata, attributes attr)	{
	(attr < payperh) ?
		set_char_data(ent, (char*)newdata, attr) : 
		set_uint_data(ent, *((unsigned long*)(newdata)), attr);
	return;
}

static void set_char_data(entry* ent, char* str, attributes attr)	{
	char* fld = NULL;
	if(attr == pos)		fld = ent->position;
	else if(attr == name1)	fld = ent->firstName;
	else if(attr == name2)	fld = ent->secondName;
	else if(attr == name3)	fld = ent->lastName;
	strncpy(fld, str, WORD_SIZE);
	fld = NULL;
	return;
}
static void set_uint_data(entry* ent, unsigned long val, attributes attr)	{
	if(attr == id)	
		ent->ID = val;
	else	{
		if(attr == payperh)	
			ent->payPerHour = val;
		else	if(attr == hrs)	
			ent->hours = val;
		ent->pay = ent->payPerHour * ent->hours;
	}
	return;
}

void* getData(entry* ent, attributes attr)	{
	return (attr < payperh) ? 	(void*)get_char_data(ent, attr) : 
					(void*)get_uint_data(ent, attr);
}

static char* get_char_data(entry* ent, attributes attr)	{
	if(attr == pos)		return ent->position;
	if(attr == name1)	return ent->firstName;
	if(attr == name2)	return ent->secondName;
	if(attr == name3)	return ent->lastName;
	return NULL;
}
static unsigned long* get_uint_data(entry* ent, attributes attr)	{
	if(attr == payperh)	return &(ent->payPerHour);
	if(attr == hrs)		return &(ent->hours);
	if(attr == id)		return &(ent->ID);
	if(attr == pay)		return &(ent->pay);
	return NULL;
}

int checkID(database* db, unsigned long id)	{
	unsigned int n = db->occupiedMem / sizeof(entry);
	entry* ptr = db->headEntry;
	int retv = 0;
	for(unsigned int i = 0; i<n && !(retv = ((ptr+i)->ID==id)); i++);
	ptr = NULL;
	return retv;
}

int enoughMem(database* db)	{
	return (int)(sizeof(entry) <= (db->allMem - db->occupiedMem));
}
int toomanyMem(database* db)	{
	return (int)(((long)(db->allMem) - (long)(db->occupiedMem)) 
			> 
			(long)getpagesize());
}
int isemptyDB(database* db)	{
	return (int)(!(db->headEntry && db->occupiedMem));
}

int pageRealloc(database* db, int growDir)	{
	entry* ptr = db->headEntry;
	unsigned long page_s = getpagesize();
	if((db->allMem < page_s) && (growDir < 0))
		return 1;
	unsigned long new_size = db->allMem + growDir * page_s;
	int retv;
	ptr = (entry*)realloc(ptr, new_size);
	if(ptr)	{
		db->headEntry = ptr;
		ptr = NULL;
		db->allMem = new_size;
		retv = 0;
	}
	else
		retv = -1;
	return retv;
}

void swapEntries(entry* ent0, entry* ent1)	{
	entry tmp, *p_tmp = &tmp;
	unsigned long entry_s = sizeof(entry);
	memmove(p_tmp, ent0, entry_s);
	memmove(ent0, ent1, entry_s);
	memmove(ent1, p_tmp, entry_s);
	p_tmp = NULL;
	return;
}
