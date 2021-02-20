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
static char **dialog;

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
static int inputData(entry*); /*1 - input error; (-1) - invalid input;*/
/*********************************************************************************/


/*SCHEDULER'S IMPLEMENTATION*/
/*********************************************************************************/
int  scheduler(void)	{
	int resultProc, key;

	initScheduler();
	while(1)	i{
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
	if(!(menu && dialogs))	{
		errcode = errno;
		perror(__func__);
		exit(errcode);
	}
	initInputOrder();
}
static void uninitScheduler(int errcode){
	if(currentDatabase)	{
		/*save database to file(INSERT)*/
		demakeDatabase(currentBase);
		curretnBase=NULL;
	}
	if(currentFile)	{
		closedb(currentFile);
	}
	free(menu);
	free(dialog);
	menu=dialog=NULL;
}

static int newWorker(void)	{
	entry buffer, *target=NULL;
	int res;

	/*fill buffer*/
	res = inputData(&buffer);
	/*checkInput*/
	if(res)	{
		/*res==1?(TYPE2: INPUT_ERROR):(TYPE3);*/
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
			/*TYPE3: ALLOCATION MEMORY*/
			/*EXIT FROM FUNCT() TO UNINIT()*/
		}
	}
	if(!enoughMem(currentDatabase))	{
		res = pageRealloc(currentDatabase, 1);
		if(res)	{
			/*TYPE3: ALLOCATION MEMORY*/
			/*EXIT FROM...*/
		}
	}
	target = newEntry(currentDatabase);
	/*copy(new entry, buffer)*/
	copy(target, &buffer);
	return 0;
}
static int editData(void)	{}
static int findWorker(void)	{}
static int deleteData(void)	{}
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
/*********************************************************************************/
