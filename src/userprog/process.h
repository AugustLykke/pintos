#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

typedef int tid_t;

struct arguments
{
	char **argv;
	int argc;	
  struct process* proc;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
struct process* make_process (void);
struct process* make_main_process(void);

#endif /* userprog/process.h */

