#include "scheduler.h"

#include "database.h"
#include "fileio.h"
#include "io.h"

#include <errno.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCHEDPROCSIZE ('v' + 1)
#define FILENAMESIZE	0X30

#define AUTOSAVENAME	"autosave%d.db"
#define MAXNFILES	0X8

/*ERROR VALUES*/
#define	INVALID_INPUT		0X100
#define EXIST_ID		0X101
#define NODATA			0X102
#define	NOTHING_FOUND		0x103
#define CREATEFILE_ERROR	0x104
#define WRITEFILE_ERROR		0x105
#define OPENFILE_ERROR		0x106
#define	EMPTY_FILE		0x107
#define READFILE_ERROR		0x108

typedef int (*procedure)(void);

static procedure schedulerProcedures[SCHEDPROCSIZE];
static void initSchedulerProcedures(void);

static char *menu;
static char *formatOutput;
static char **dialogs;
static char *underline;
static char *tabs;

static database *currentDatabase;
static FILE *currentFile;

static attributes inputOrderAttrs[DIALOGS_SIZE];
static void initInputOrder(void);

/*IMPORTED DATA*/
/*********************************************************************************/
size_t wordSize;
/*********************************************************************************/

/*MAIN PROCEDURES PROTOTYPES*/
/*********************************************************************************/
/*Initalizaition static objects*/
static void	initScheduler(void);
/*Conversally. Get an error code*/
static void	uninitScheduler(const char*, int);
static int	autosave(void);

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
	int key;

	initScheduler();
	while(1)	{
		puts(menu);
		key = getChkey();
		if(key<0)	{
			puts("Be more carefully\n");
			continue;
		}
		schedulerProcedures[key]();
	}

	uninitScheduler(__func__, 0);
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
	dialogs = initDialogSet();
	formatOutput = initFormatList(wordSize);
	underline = initUnderline(wordSize);
	tabs = initTabs(wordSize);
	if(!(menu && dialogs && formatOutput && underline && tabs))	{
			errcode=errno;
			errno=0;
			uninitScheduler(__func__, errcode);
	}
	initInputOrder();
}
static void uninitScheduler(const char* func, int errcode){
	char functionName[FILENAMESIZE];

	if(currentDatabase)	{
		autosave();
		demakeDatabase(currentDatabase);
		currentDatabase=NULL;
	}
	if(currentFile)	{
		closedb(currentFile);
	}
	free(menu);
	free(dialogs);
	free(formatOutput);
	free(underline);
	free(tabs);
	menu=formatOutput=underline=tabs = NULL;
	dialogs=NULL;
	strncpy(functionName, func ? func : __func__, FILENAMESIZE);
	funcPerror(functionName, errcode);
	exit(errcode);
}

static int autosave(void)	{
	char filename[FILENAMESIZE];
	FILE *file=NULL;
	int i;
	entry* source;
	unsigned long nbytes, wrtbytes;
	if(isemptyDB(currentDatabase))
		return 0;
	for(i=0;i<MAXNFILES && !file; i++)	{
		snprintf(filename, FILENAMESIZE, AUTOSAVENAME,i);
		file = createdb(filename);		
	}
	source = getHeadDB(currentDatabase);
	nbytes = currentDatabase->occupiedMem;
	wrtbytes = writeToFile(file, source, nbytes);	
	closedb(file);
	return (int)(wrtbytes-nbytes);
}

static int newWorker(void)	{
	entry buffer, *target=NULL;
	int res, errcode;

	/*fill buffer*/
	res = inputData(&buffer);
	/*checkInput*/
	if(res)	{
		puts("Invalid input\n");
		return INVALID_INPUT;
	}
	/*checkID*/
	res = currentDatabase ? checkID(currentDatabase, buffer.ID) : 0;
	if(res)	{
		printf("This ID(%lu) is already exists", buffer.ID);
		return EXIST_ID;
	}
	/*allocation new entry in database*/
	if(!currentDatabase)	{
		currentDatabase = makeDatabase();
		if(!currentDatabase)	{
			errcode=errno;
			errno=0;
			uninitScheduler(__func__, errcode);
		}
	}
	if(!enoughMem(currentDatabase))	{
		res = pageRealloc(currentDatabase, 1);
		if(res)	{
			errcode=errno;
			errno=0;
			uninitScheduler(__func__, errcode);
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
	entry buffer, *target;
	int res;
	unsigned long ID;

	/*do exists some database and isnot empty?*/
	if(!currentDatabase || isemptyDB(currentDatabase))	{
		puts("No data,lol\n");
		return NODATA;
	}
	/*input id for search*/
	puts("Enter worker's ID:\t");
	res = getuint(&ID);
	if(res)	{
		puts("It's not number\n");
		return INVALID_INPUT;
	}
	/*search an entry is need by id-attribute*/
	List = findData(currentDatabase, &ID, id);
	/*display this entry*/
	if(List->list_s == 0)	{
		puts("Worker is not found.\n");
		removeList(List);
		return NOTHING_FOUND;
	}
	target = *(List->list);
	puts(tabs);
	puts(underline);
	displayEntry(target);
	/*fill buffer*/
	res = inputData(&buffer);
	/*check input*/
	if(res)	{
		puts("Invalid input\n");
		return INVALID_INPUT;
	}
	/*check id*/
	res = checkIDexept(currentDatabase, *((unsigned long*)getData(target, id)), *((unsigned long*)getData(&buffer, id)));
	if(res)	{
		printf("This ID(%d) is already exists\n", id);
		return EXIST_ID;
	}
	copyEntries(target, &buffer);
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
	
	/*do exists some database and isnot empty?*/
	if(!currentDatabase || isemptyDB(currentDatabase))	{
		puts("No data,lol\n");
		return NODATA;
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
	else	{
		puts("Worker is not found.\n");
		removeList(List);
		return NOTHING_FOUND;
	}

	removeList(List);
	return 0;
}
static int deleteData(void)	{	
	unsigned long ID;
	int res, errcode;
	listEntry* List;
	entry* worker;

	/*do exists some database and isnot empty?*/
	if(!currentDatabase || isemptyDB(currentDatabase))	{		
		puts("No data,lol\n");
		return NODATA;
	}
	/*input ID for search*/
	puts("Enter ID:\t");
	res = getuint(&ID);
	if(res==-1)	{		
		puts("Invalid ID's format,lol\n");
		return INVALID_INPUT;
	}
	/*search an entry what is need by ID*/
	List = findData(currentDatabase, &ID, id);
	/*display this entry*/
	worker = *(List->list);
	if(List->list_s)	{
		puts(tabs);
		puts(underline);
		displayEntry(worker);
	}
	else	{
		puts("Worker isnot found.\n");
		removeList(List);
		return NOTHING_FOUND;
	}
	/*delete this entry*/
	remEntry(worker);
	for(; toomanyMem(currentDatabase);)	{
		res = pageRealloc(currentDatabase, -1);
		if(res)	{
			errcode=errno;
			errno=0;
			uninitScheduler(__func__, errcode);
		}
	}
	puts("Worker has been deleted.\n");
	removeList(List);
	return 0;
}
static int saveToFile(void)	{
	char filename[FILENAMESIZE];
	FILE *file;
	int res;
	unsigned long nbytes, wrtbytes;
	entry *source;

	/*do existst some database and its not empty?*/
	if(!currentDatabase || isemptyDB(currentDatabase))	{		
		puts("No data,lol\n");
		return NODATA;
	}
	/*input filename*/
	getnstr(filename, FILENAMESIZE);
	/*if this file already exists*/
	file = opendb(filename);
	if(file)	{
		/*exchange or not?*/
		puts("Such file is already exists. Do rewrite it or not?(y/n)\t");
		res = yesOrNo();
		if(res<0)	{
			puts("Invalid input\n");
			closedb(file);
			return INVALID_INPUT;
		}
		else if(!res)	{
			puts("Ok..can try again\n");
			closedb(file);
			return 0;
		}
		else if(res==1)	{
			closedb(file);
			remdb(filename);
		}
	}
	/*create a file*/
	file = createdb(filename);
	if(!file)	{
		puts("Failed to create file");
		return CREATEFILE_ERROR;
	}
	/*write to file*/
	source = getHeadDB(currentDatabase);
	nbytes = currentDatabase->occupiedMem;
	wrtbytes = writeToFile(file, source, nbytes);
	/*check ferror*/
	if(wrtbytes!=nbytes)	{		
		printf("Write file %s error\n", filename);
		return WRITEFILE_ERROR;
	}
	/*close a file*/
	closedb(file);
	printf("File %s is succesfully written\n", filename);
	return 0;
}
static int loadFromFile(void)	{
	int res, errcode;
	char filename[FILENAMESIZE];
	FILE* file;
	unsigned long rdbytes, filesize;

	/*do exists some database and its not empty?*/
	if(currentDatabase && !isemptyDB(currentDatabase))	{
		puts("Do save opened base?(y/n)\t");
		res = yesOrNo();
		if(res)	{
			puts("Autosave?(y/n)\t");
			res = yesOrNo();
			res = res ? autosave() : saveToFile();
			if(res)	{
				printf("Failed to save to file. Returned %d\n", res);
				return res;
			}
		}
		demakeDatabase(currentDatabase);
	}
	/*input filname*/
	getnstr(filename, FILENAMESIZE);
	file = opendb(filename);
	if(!file)	{
		printf("Failed to open file %s\n", filename);
		return OPENFILE_ERROR;
	}
	/*create new database*/
	currentDatabase = makeDatabase();
	if(!currentDatabase)	{
		errcode=errno;
		errno=0;
		uninitScheduler(__func__, errcode);
	}
	filesize = getsizefile(file);
	if(!filesize)	{
		printf("File %s is empty\n", filename);
		closedb(file);
		return EMPTY_FILE;
	}
	for(; currentDatabase->allMem < filesize;)	{
		res = pageRealloc(currentDatabase, 1);
		if(res)	{
			errcode=errno;
			errno=0;
			uninitScheduler(__func__, errcode);
		}
	}
	rdbytes = readFromFile(file, getHeadDB(currentDatabase), filesize);
	if((rdbytes-filesize))	{
		printf("File reading error: %s\n", filename);
		res = READFILE_ERROR;
	}
	else	{
		puts("Database is succesfully downloaded\n");
		res=0;
	}
	closedb(file);
	return res;
}
static int showAllWorkers(void)	{
	if(!currentDatabase)	{
		puts("No data,lol\n");
		return NODATA;
	}
	displayDatabase(currentDatabase);
	return 0;
}
static int sortData(void)	{return 0;}
static int quit(void)		{
	uninitScheduler(NULL, 0);
	return 0xfffff;
}
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
					return 1;
				}
				setData(ent, bufferString, *pattr);
			}
			else if(*pattr>=payperh)	{
				res = getuint(&bufferNumber);
				if(!res)	{
					setData(ent, &bufferNumber, *pattr);
				}
				else	
					break;
			}
		}			
		pdialogs++;
		pattr++;
	}
	return res;
}

static void displayEntry(entry *ptr)	{
	unsigned long ID, pay_perh, hours, salary;
	char *post, *name, *surname, *pathr;
	if(ptr)	{
		ID = *((unsigned long*)(getData(ptr, id)));	
		hours = *((unsigned long*)(getData(ptr, hrs)));	
		pay_perh= *((unsigned long*)(getData(ptr, payperh)));	
		salary= *((unsigned long*)(getData(ptr, pay)));	
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
