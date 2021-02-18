#include "scheduler.h"

#include "database.h"
#include "fileio.h"
#include "io.h"

#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	FUNCT_S			'v'-'e'
#define UNDERLINE_LENGTH	0X40
#define AUTOSAVENAME		"db/autosave.db"
typedef int(*funct)(void);

static unsigned long	bufferNumber;
static database*	currentDatabase;
static FILE*		currentFile;
static funct		functs[FUNCT_S];
static char*		formatList;
static char*		underline;
static char*		menu;
static char**		dialogs;
unsigned long		wordSize;
int			errnum;

static void	init_functs(void);
static void	uninit_functs(void);
static void	init_scheduler(void);
static int	uninit_scheduler(int);
static int	autosave_database(void);

static int new_entry(void);
static int edit_data(void);
static int find_entry(void);
static int delete_data(void);
static int save_to_file(void);
static int load_from_file(void);
static int show_all(void);
static int sort_data(void);
static int quit(void);

static void free2dptr(void**, size_t, size_t);
/************************************************/
int scheduler(void)	{
	
	while(1)	{
		procs[key]();
	}
	
}
/************************************************/
static void init_functs(void)	{
	functs['n'] = new_entry;
	functs['e'] = edit_data;
	functs['f'] = find_entry;
	functs['d'] = delete_data;
	functs['s'] = save_to_file;
	functs['l'] = load_from_file;
	functs['v'] = show_all;
	functs['c'] = sort_data;
	functs['q'] = quit;
}
static void uninit_functs(void)	{
	memset(functs, 0x00, FUNCTS_S * sizeof(funct));
}

static void init_scheduler(void)	{
	currentDatabase=NULL;
	currentFile=NULL;
	errno=0;
	errnum=0;
	wordSize = WORD_SIZE;
	init_functs();
	formatList = initFormatList(wordSize);
	underline = initUnderline(UNDERLINE_LENGTH);
	menu = initSelectMenu();
	dialogs	= initDialogSet();
	if(!(formatList && underline && menu && dialogs))	{
		errnum = errno;
		perror(__func__);
		exit(errnum);
	}
	perror(__func__);
	return;
}

static void free2dptr(void** ptr, size_t size, size_t membn)	{
	void **start = ptr, **top = ptr+size*membn;
	for(;ptr<top;ptr++)	{
		free(*ptr);
		*ptr=NULL;
	}
	free(start);
}

static int uninit_scheduler(int err_code)	{
	int res;
	/*save current database(s)(in case it's exists) into save-file;*/
	if(currentDatabase)	{
		if((currentDatabase->headEntry))	{
			res = autosave_database();
			if(res)
				 printerr("Autosaving's error. Written %d bytes\n", res);
			demakeDatabase(currentDatabase);
			currentDatabase=NULL;
		}
	}
	/*deallocating and uninitaliazing global objects;*/
	uninit_functs();
	free(formatList);
	free(underline);
	free(menu);
	free2dptr(dialogs, DIALOGS_SIZE);
	formatList=underline=menu=dialogs;
	/*close all file descriptors;*/
	if(currentFile)	{
		closedb(currentFile);
	}
	/*print a report by err_code;*/
	printerr("%s", strerror(err_code));
	/*call an exit() or abort();*/
	exit(err_code);
}

static int autosave_database(void)	{
	FILE* fd=NULL;
	int res;
	fd = createdb(AUTOSAVENAME);
	if(!fd)	{
		fd = opendb(AUTOSAVENAME);
		if(!fd)
			return -1;
	}
	res = writeToFile(fd, currentDatabase->headEntry, currentDatabase->occupiedMem);
	closedb(fd);
	return ((int)(currentDatabase->occupiedMem) - res);
}
/*************************************************/
