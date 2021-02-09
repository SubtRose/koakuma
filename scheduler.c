#include "scheduler.h"
#define	PROCS_S	'v'-'e'
typedef int(*proc)(void);

static proc procs[PROCS_S];

static void init_procs(void);

static int new_entry(void);
static int edit_data(void);
static int find_entry(void);
static int delete_data(void);
static int save_to_file(void);
static int load_from_file(void);
static int show_all(void);
static int sort_data(void);
static int quit(void);

int scheduler(void)	{
	int key, error_code, res;

	init_procs();
	while(1)	{
		/*verification a key*/
		/********************/
		
		res = procs[key];
		/*analys res*/
		/************/
	}

	return error_code;
}

static void init_procs(void)	{
	procs['n'] = new_entry;
	procs['e'] = edit_data;
	procs['f'] = find_entry;
	procs['d'] = delete_data;
	procs['s'] = save_to_file;
	procs['l'] = load_from_file;
	procs['v'] = show_all;
	procs['c'] = sort_data;
	procs['q'] = quit;
}
