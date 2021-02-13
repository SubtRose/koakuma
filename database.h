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

typedef enum _attributes 
	{pos, name1, name2, name3, payperh, hrs, id, pay} 
attributes;

/*That above functions don't controls for memory!*/

database*	makeDatabase(void);
void		demakeDatabase(database*);
entry*		newEntry(database*);
entry*		remEntry(entry*);
void		setData(entry*, void*, attributes);
void*		getData(entry*, attributes);

/*chkID() return 1, if id is already exists in base, 0 is in other case;*/
int		checkID(database*, unsigned long);
/*enoughMem() return 1, if memory is enough for new entry;*/
int		enoughMem(database*);
int		toomanyMem(database*);
/*variable growDir is one in case, if have be need to allocating memory,*/
/*and negative, if have be need to deallocating;*/
int		pageRealloc(database*, int);
void		swapEntries(entry*, entry*);

#endif
