#include "scheduler.h"

#include "database.h"
#include "fileio.h"
#include "io.h"

#include <errno.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SCHEDPROCSIZE ('v' - 'e' + 1)	

typedef (int)(*procedure)(void);

static procedure schedulerProcedures[SCHEDPROCSIZE];
static void initSchedulerProcedures(void);

static char *menu;
static char *formatOutput;
static char **dialog;
static char *underline;
static char *tabs;

static database *currentDatabase;
static FILE *currentFile;

static attributes orderInputAttrs[DIALOG_SIZE];
static void initInputOrder(void);

/*IMPORTED DATA*/
/*********************************************************************************/
size_t wordSize;
/*********************************************************************************/

/*MAIN PROCEDURES PROTOTYPES*/
/*********************************************************************************/
/*Initalizaition static objects*/
static void initScheduler(void);
/*Conversally. Get an error code*/
static void uninitScheduler(int);

static int newWorker(void);
static int editData(void);
static int findWorker(void);
static int deleteData(void);
static int saveToFile(void);
static int loadFromFile(void);
static int showAllWorkers(void);
static int sortData(void);
static int quit(void);
/*********************************************************************************/

/*OTHER PROTOTYPES*/
/*********************************************************************************/
static int	inputData(entry*); /*1 - input error; (-1) - invalid input;*/
static void	displayEntry(entry*);
static void	displayDatabase(database*);
static void	displayList(listEntry*);
/*********************************************************************************/


/*SCHEDULER'S IMPLEMENTATION*/
/*********************************************************************************/
int  scheduler(void)	{
	int resultProc, key;

	initScheduler();
	while(1)	{
		puts(menu);
		key = getKey();
		result = schedulerProcedures[key];
	}

	uninitScheduler(0);
}
/*********************************************************************************/


/*MAIN PROCEDURES IMPLEMENTATIONS*/
/*********************************************************************************/
static void initScheduler(void)	{
	int errcode;
	initSchedulerProcedures();
	currentDatabase=NULL;
	currentFile=NULL;
	wordSize = WORD_SIZE;
	menu = initSelectMenu();
	dialog = initDialogSet();
	formatOuptut = initFormatList(wordSize);
	underline = initUnderline(wordSize);
	tabs = initTabs(wordSIze);
	if(!(menu && dialogs && formatOutput && underline && tabs))	{
		errcode = errno;
		perror(__func__);
		exit(errcode);
	}
	initInputOrder();
}
static void uninitScheduler(int errcode){
	if(currentDatabase)	{
		/*save database to file(INSERT)*/
		demakeDatabase(currentDatabase);
		curretnDatabase=NULL;
	}
	if(currentFile)	{
		closedb(currentFile);
	}
	free(menu);
	free(dialog);
	free(formatOutput);
	free(underline);
	free(tabs);
	menu=dialog=formatOutput=underline=tabs = NULL;
}

static int newWorker(void)	{
	entry buffer, *target=NULL;
	int res;

	/*fill buffer*/
	res = inputData(&buffer);
	/*checkInput*/
	if(res)	{
		/*TYPE3: INVALID DATA*/
	}
	/*checkID*/
	res = currentDatabase ? checkID(currentDatabase, buffer.ID) : 0;
	if(res)	{
		/*TYPE3*/
	}
	/*allocation new entry in database*/
	if(!currentDatabase)	{
		currentDatabase = makeDatabase();
		if(!currentDatabase)	{
			/*TYPE1: ALLOCATION MEMORY*/
			/*EXIT FROM FUNCT() TO UNINIT()*/
		}
	}
	if(!enoughMem(currentDatabase))	{
		res = pageRealloc(currentDatabase, 1);
		if(res)	{
			/*TYPE1: ALLOCATION MEMORY*/
			/*EXIT FROM...*/
		}
	}
	target = newEntry(currentDatabase);
	/*copy(target, buffer)*/
	copyEntries(target, &buffer);
	puts("New worker is succersfully append\n");
	return 0;
}
static int editData(void)	{
	listEntry* List=NULL;
	entry buffer;
	int res;
	unsigned long ID;

	/*do exists any database and isnot empty?*/
	if(!(currentDatabase && isempty(currentDatabase)))	{
		/*TYPE2*/
	}
	/*input id for search*/
	puts("Enter worker's ID:\t");
	res = getuint(&ID);
	if(res)	{
		/*TYPE3*/
	}
	/*search an entry is need by id-attribute*/
	List = findData(currentDatabase, &ID, id);
	/*display this entry*/
	if(List->list_s == 0)	{
		puts("Worker is not found.\n");
		removeList(List);
		return /*NOTHING_FOUND*/;
	}
	puts();
	displayEntry(*List);
	/*fill buffer*/
	res = inputData(&buffer);
	/*check input*/
	if(res)	{
		/*TYPE3*/
	}
	/*check id*/
	res = checkIDexept(currentDatabase, getData(*List, id), getData(&buffer, id));
	if(res)	{
		/*TYPE3: INVALID ID*/
	}
	copyEntries(*List, &buffer);
	puts("Data is succesfully changed\n");

	removeList(List);
	return 0;
}
static int findWorker(void)	{
	char bufferString[wordSize];
	unsigned long bufferNumber;
	int res;
	void* data;
	attributes attr;
	listEntry* List;
	
	/*do exists current database and isnot empty?*/
	if(currentDatabase && isempty(currentDatabase))	{
		/*TYPE2*/
	}
	/*input a data for search*/
	getnstr(bufferString, wordSize);
	/*detection a type of data (string or id)*/
	res = atold(&bufferNumber, bufferString);
	if(res==-1)	{
		data = bufferString;
		attr = name1;
	}
	else	{
		data = &bufferNumber;
		attr = id;
	}
	/*search an entry, what is need, by data*/
	List = findData(currentDatabase, data, attr);
	/*display this entry*/
	if(List->list_s)	{
		puts(tabs);
		puts(underline);
		displayList(List);
	}
	else
		/*NOTHING_FOUND*/;

	removeList(List);
	return 0;
}
static int deleteData(void)	{
	
	/*DO EXISTS CURRENT BUFFER AND ISNOT EMPTY?*/
	/*INPUT DATA-ATTRIBUTES FOR SEARCH*/
	/*SEARCH AN ENTRY IS NEED BY DATA-ATTRIBUTES*/
	/*DISPLAY THIS ENTRY*/
	/*....*/
}
static int saveToFile(void)	{}
static int loadFromFile(void)	{}
static int showAllWorkers(void)	{}
static int sortData(void)	{}
static int quit(void)		{}
/*********************************************************************************/

/*OTHER FUNCTIONS IMPLEMETNATIONS*/
/*********************************************************************************/
static void initSchedulerProcedures(void)	{
	schedulerProcedures['n']= newWorker;
	schedulerProcedures['e']= editData;
	schedulerProcedures['f']= findWorker;
	schedulerProcedures['d']= deleteData;
	schedulerProcedures['s']= saveToFile;
	schedulerProcedures['r']= loadFromFile;
	schedulerProcedures['v']= showAllWorkers;
	schedulerProcedures['c']= sortData;
	schedulerProcedures['q']= quit;
	return;
}

static void initInputOrder(void)	{	
	inputOrderAttrs[0]=none;
	inputOrderAttrs[1]=id;
	inputOrderAttrs[2]=name2;
	inputOrderAttrs[3]=name1;
	inputOrderAttrs[4]=name3;
	inputOrderAttrs[5]=pos;
	inputOrderAttrs[6]=payperh;
	inputOrderAttrs[7]=hrs;
}

static int inputData(entry* ent)	{
	size_t size;
	char bufferString[wordSize];
	unsigned long bufferNumber;
	attributes *pattr;
	char **pdialogs, **max;
	int res=0;

	size = DIALOGS_SIZE;
	pdialogs = dialogs;
	max = pdialogs + size;
	pattr = inputOrderAttrs;
	while(pdialogs<max)	{
		puts(*pdialogs);
		if(*pattr>none)	{
			/**pattr<payperh?getnstr():getuint()*/
			/*chkError*/
			if(*pattr<payperh)	{
				if(!getnstr(bufferString, wordSize))	{
					/*INPUT ERROR*/
					return 1;
				}
				setData(ent, bufferString, *pattr);
			}
			else if(*pattr>=payperh)	{
				res = getuint(bufferNumber);
				if(!res)	{
					setData(ent, bufferNumber, *pattr);
				}
				else	{
					if(res==1)	{/*INPUT ERROR*/}
					if(res==-1)	{/*INVALID INPUT*/}
					break;
				}
			}
		}			
		pdialogs++;
		pattrs++;
	}
	return res;
}

static void displayEntry(entry *ptr)	{
	unsigned long ID, pay_perh, hours, salary;
	char *post, *name, *surname, *pathr;
	if(ptr)	{
		ID = *getData(ptr, id);	
		pay_perh = *getData(ptr, payperh);
		hours = *getData(ptr, hrs);
		salary = *getData(ptr, pay);
		post = getData(ptr, pos);
		name = getData(ptr, name1);
		surname = getData(ptr, name3);
		pathr = getData(ptr, name2);
		printf(formatOutput, ID, surname, name, pathr, post, pay_perh,hours, salary);
	}
}
static void displayDatabase(database *db)	{
	entry *ptr, *lim;
	if(db && (ptr=getHeadDB(db)))	{
		lim = getTailDB(db) + 1;
		for(;ptr<lim; ptr++)	{
			displayEntry(ptr);
		}	
	}
}
static void displayList(listEntry* le)	{
	entry *ptr, *lim;
	if(le && le->list)	{
		ptr = *(le->list);
		lim = ptr + le->list_s;
		for(;ptr<lim;ptr++)
			displayEntry(ptr);
	}
}
/*********************************************************************************/
