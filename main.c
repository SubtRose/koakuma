#include "database.h"
#include "fileio.h"
#include "io.h"
#include "sort.h"
#include "test.h"

#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCHEDPROCSIZE ('v' + 1)
#define FILENAMESIZE	0X30

#define AUTOSAVENAME	"db/autosave%d.db"
#define MAXNFILES	0X8

#define SAVE_IT		1
#define NOSAVE		0

/*ERROR VALUES*/
#define NOERROR			0X0
#define	INVALID_INPUT		0X100
#define EXIST_ID		0X101
#define NODATA			0X102
#define	NOTHING_FOUND		0x103
#define CREATEFILE_ERROR	0x104
#define WRITEFILE_ERROR		0x105
#define OPENFILE_ERROR		0x106
#define	EMPTY_FILE		0x107
#define READFILE_ERROR		0x108
#define INVALID_FILETYPE	0x109
#define SORT_ERROR		0x1a0

typedef int (*procedure)(void);
typedef void (*sortFunction)(database*, attributes);

static procedure procedures[SCHEDPROCSIZE];
static void initProcedures(void);

static char *menu;
static char *formatOutput;
static char **dialogs;
static char *underline;
static char *tabs;

static database *currentDatabase;
static FILE *currentFile;

static attributes inputOrderAttrs[ATTRS_NUM];
static void initInputOrder(void);

#ifdef TEST_H
static char *testMenu;
static int randomBase(void);
static int testing(void);
#define GENERATE_ERROR	0x200
#endif

/*IMPORTED DATA*/
/*********************************************************************************/
size_t wordSize;
/*********************************************************************************/

/*MAIN PROCEDURES PROTOTYPES*/
/*********************************************************************************/
/*Initalizaition static objects*/
static void	initalization(void);
/*Conversally. Get an error code*/
static void	uninitalization(const char*, int, int);
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
static int		inputData(entry*); /*1 - input error; (-1) - invalid input;*/
static void		displayEntry(entry*);
static void		displayDatabase(database*);
static void		displayList(listEntry*);

static int		wrongFileSize(unsigned long); /*return 0 is correct;*/
static attributes	attributeSelector(void);
/*********************************************************************************/


/*MAIN IMPLEMENTATIONS*/
/*********************************************************************************/
int  main(void)	{
	int key;

	initalization();

#ifndef TEST_H
	while(1)	{
		puts(menu);
		key = getChkey();
		if(key<0)	{
			puts("Be more carefully\n");
			continue;
		}
		procedures[key]();
	}
#else
	while(1)	{
		puts(menu);
		puts(testMenu);
		key = getTestKey();
		if(key<0)	{
			puts("Be more carefully\n");
			continue;
		}
		procedures[key]();
	}
#endif
}
/*********************************************************************************/


/*MAIN PROCEDURES IMPLEMENTATIONS*/
/*********************************************************************************/
static void initalization(void)	{
	int errcode;
	initProcedures();
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
			uninitalization(__func__, errcode, NOSAVE);
	}
	initInputOrder();
#ifdef TEST_H
	testMenu = initTestMenu();
#endif
}
static void uninitalization(const char* func, int errcode, int needSave){
	char functionName[FILENAMESIZE];

	if(currentDatabase)	{
		if(needSave)
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
#ifdef TEST_H
	free(testMenu);
	testMenu=NULL;
#endif
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
			uninitalization(__func__, errcode, NOSAVE);
		}
	}
	if(!enoughMem(currentDatabase))	{
		res = pageRealloc(currentDatabase, 1);
		if(res)	{
			errcode=errno;
			errno=0;
			uninitalization(__func__, errcode, NOSAVE);
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
	puts("Enter Data to search:\t");
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
	if(List->list_s)	{
		puts(tabs);
		puts(underline);
		worker = *(List->list);
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
			uninitalization(__func__, errcode, NOSAVE);
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
	/*manually or auto?*/
	puts("Do save it manually?(y/n, n - autosave)\t");
	res = yesOrNo();
	if(!res)	{
		res = autosave();
		if(res)	{
			puts("Autosave error\n");
			return WRITEFILE_ERROR;
		}
		else	{
			puts("Database is succesfully saved\n");
			return 0;
		}
	}
	/*input filename*/
	puts("Enter file name:\t");
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
			res = saveToFile();
			if(res)	{
				printf("Failed to save file. Returned %d\n", res);
				return res;
			}
		}
		demakeDatabase(currentDatabase);
	}
	/*input filname*/
	puts("Enter file name to loading:\t");
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
		uninitalization(__func__, errcode, NOSAVE);
	}
	filesize = getsizefile(file);
	if(!filesize || wrongFileSize(filesize))	{
		if(!filesize)	{
			printf("File %s is empty\n", filename);
			res = EMPTY_FILE;
		}
		else	{
			puts("Wrong file type\n");
			res = INVALID_FILETYPE;
		}
		closedb(file);
		demakeDatabase(currentDatabase);
		currentDatabase=NULL;
		return res;
	}
	for(; currentDatabase->allMem < filesize;)	{
		res = pageRealloc(currentDatabase, 1);
		if(res)	{
			errcode=errno;
			errno=0;
			uninitalization(__func__, errcode, NOSAVE);
		}
	}
	rdbytes = readFromFile(file, getHeadDB(currentDatabase), filesize);
	if(rdbytes-filesize)	{
		printf("File reading error: %s\n", filename);
		res = READFILE_ERROR;
	}
	else	{
		puts("Database is succesfully downloaded\n");
		currentDatabase->occupiedMem += (unsigned long)rdbytes;
		bindToDB(currentDatabase);
		res=0;
	}
	closedb(file);
	return res;
}

static int showAllWorkers(void)	{
	if(!currentDatabase || isemptyDB(currentDatabase))	{
		puts("No data,lol\n");
		return NODATA;
	}
	puts(tabs);
	puts(underline);
	displayDatabase(currentDatabase);
	return 0;
}

static int sortData(void)	{
#ifdef TEST_H
	const char sortKeySet[] = "bsq";
	const sortFunction setFunctions[] = 
		{bubbleSort, shellSort, quickSort};
	const unsigned long inputSize = 0x40;
	sortFunction psort;
	char input[inputSize];
	int key;
	void *res;
	attributes attr;

	if(!currentDatabase || isemptyDB(currentDatabase))	{
		puts("No data, lol\n");
		return NODATA;
	}

	puts("Select sort:\t");
	fgets(input, inputSize, stdin);
	key = input[0];
	res = strchr(sortKeySet, tolower(key));
	if(!res)	{
		puts("Invalid Key\n");
		return INVALID_INPUT;
	}
	psort = setFunctions[(char*)res - sortKeySet];
	attr = attributeSelector();
	psort(currentDatabase, attr);
#else	
	attributes attr;

	if(!currentDatabase || currentDatabase->occupiedMem<2)	{
		puts("No data, lol\n");
		return NODATA;
	}
	
	attr = attributeSelector();
	if(attr!=none)	
		quickSort(currentDatabase, attr);
	puts("Ready\n");
#endif
	return 0;
}

static int quit(void)		{
	int saveopt = NOSAVE, res;
	if(currentDatabase)	{
		puts("Do save it?(y/n)\t");
		res = yesOrNo();
		saveopt = res ? SAVE_IT : NOSAVE;
	}
	uninitalization(NULL, NOERROR, saveopt);
	return 0xfffff;
}

#ifdef TEST_H
static int randomBase(void)	{
	unsigned long baseSize;
	int errcode=0;
	if(currentDatabase)	{
		demakeDatabase(currentDatabase);
		currentDatabase = NULL;
	}
	puts("Enter a size of new base:\t");
	getuint(&baseSize);
	currentDatabase = generateBase(baseSize);
	if(!currentDatabase)	{
		puts("Failed to generate database\n");
		errcode = GENERATE_ERROR;
	}
	return errcode;
}

static int testing(void)	{
	
	const void *fetchSort[] = {	bubbleSort, 
					shellSort, 
					quickSort, 
					0x00,
					"Bubble", 
					"Shell", 
					"Quick", 
					0x00
				};
	const attributes keyAttribute = name2;
	unsigned long i,j, n, baseSize, step, limit; 
	int res;
	clock_t *buffer;
	resultTest* result[3];
	clock_t timer[2], dtimer;

	if(currentDatabase)	{
		demakeDatabase(currentDatabase);
		currentDatabase=NULL;
	}

	n = NUMBER_TRIES;
       	buffer = (clock_t*)malloc(n*sizeof(clock_t));
	printf("Limit test(default = %u):\t", TEST_LIMIT);
	res = getuint(&limit);
	if(res)	
		limit = TEST_LIMIT;
	printf("Step test(defult = %u):\t", TEST_STEP);
	res = getuint(&step);
	if(res)
		step = TEST_STEP;

	for(i=0; fetchSort[i]; i++)	{
		result[i] = initTest((char*)(fetchSort[i+4]));
		printf("Initalization test for %s-sorting\n", (char*)(fetchSort[i+4]));
		for(baseSize=1; baseSize<limit; baseSize *= step)	{
			for(j=0; j<n;j++)	{
				printf("Generating a base, size of base = %lu\n", baseSize);
				currentDatabase = generateBase((unsigned long)baseSize);
				timer[0] = clock();
				printf("Sorting(%s)..\n", (char*)(fetchSort[i+4]));
				((sortFunction)(fetchSort[i]))(currentDatabase, keyAttribute);
				timer[1] = clock();
				demakeDatabase(currentDatabase);
				currentDatabase=NULL;
				dtimer = timer[1] - timer[0];			
				buffer[j] = dtimer;
				printf("Sorting is finished, clock:\t%Lf\n", (long double)buffer[j]);
				puts(underline);
			}
			addRate(result[i], baseSize, buffer, n);
		}
	}

	n=3;
	for(i=0; i<n;i++)	{
		printResult(result[i]);
		removeResult(result[i]);
		result[i]=0;
	}

	free(buffer);
	return 0;
}
#endif
/*********************************************************************************/

/*OTHER FUNCTIONS IMPLEMETNATIONS*/
/*********************************************************************************/
static void initProcedures(void)	{
	procedures['n']= newWorker;
	procedures['e']= editData;
	procedures['f']= findWorker;
	procedures['d']= deleteData;
	procedures['s']= saveToFile;
	procedures['r']= loadFromFile;
	procedures['v']= showAllWorkers;
	procedures['c']= sortData;
	procedures['q']= quit;
#ifdef TEST_H
	procedures['g']= randomBase;
	procedures['t']= testing;
#endif
}

static void initInputOrder(void)	{	
	inputOrderAttrs[0]=none;
	inputOrderAttrs[1]=id;
	inputOrderAttrs[2]=name3;
	inputOrderAttrs[3]=name1;
	inputOrderAttrs[4]=name2;
	inputOrderAttrs[5]=pos;
	inputOrderAttrs[6]=payperh;
	inputOrderAttrs[7]=hrs;
	inputOrderAttrs[8]=pay;
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
	entry **ptr, **lim;
	if(le && le->list)	{
		ptr = le->list;
		lim = ptr + le->list_s;
		for(;ptr<lim;ptr++)
			displayEntry(*ptr);
	}
}

static int wrongFileSize(unsigned long fsize)	{
	return fsize % sizeof(entry);
}

static attributes attributeSelector(void)	{
	int res;
	unsigned long key;

	puts("Select a key-attribute to sorting\n");
	puts("1 ID\n2 Surname\n3 Name\n4 Patronymic\n5 Position\n6 Pay/h\n7 Hours\n8 Salary:\t");
	res = getuint(&key);
	if(res<0 || !(key>0 && key<9))	{
		puts("Invalid input\n");
		return none;
	}
	return inputOrderAttrs[key];
}
/*********************************************************************************/
