#ifndef DATABASE_H
#define DATABASE_H
#define WORD_SIZE	0X20

typedef struct _entry {
	void*		DB;

	unsigned long	ID;
	char 		position[WORD_SIZE];
	char 		firstName[WORD_SIZE];
	char		secondName[WORD_SIZE];
	char 		lastName[WORD_SIZE];
	unsigned long 	payPerHour;
	unsigned long 	hours;
	unsigned long 	pay;
} entry;

typedef struct _database {
	entry*		headEntry;
	unsigned long	occupiedMem;
	unsigned long	allMem;
} database;
entry*		getHeadDB(database*);
entry*		getTailDB(database*);
unsigned long	getSizeDB(database*);
database*	makeDatabase(void);
void		demakeDatabase(database*);

typedef struct _list_entry	{
	entry**		list;
	unsigned long	list_s;
} listEntry;
listEntry*	createList(void);
int		addList(listEntry*, entry*);
void		removeList(listEntry*); 

typedef enum _attributes 
	{none, pos, name1, name2, name3, payperh, hrs, id, pay} 
attributes;

/*newEntry() will return a new object, if was succesfully, else will return NULL;*/
entry*		newEntry(database*);
entry*		remEntry(entry*);
void		setData(entry*, void*, attributes);
void*		getData(entry*, attributes);

/*search by string(names) or ID*/
listEntry*	findData(database*, void* data, attributes);

/*chkID() return 1, if id is already exists in base, 0 is in other case;*/
int		checkID(database*, unsigned long);
int		checkIDexept(database*, unsigned long exept, unsigned long id);
/*enoughMem() return 1, if memory is enough for new entry;*/
int		enoughMem(database*);
int		toomanyMem(database*);
int		isemptyDB(database*);
/*variable growDir is one in case, if have be need to allocating memory,*/
/*and negative, if have be need to deallocating;*/
int		pageRealloc(database*, int);
void		swapEntries(entry*, entry*);
void		copyEntries(entry*, entry*);

#endif
