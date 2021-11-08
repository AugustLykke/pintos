#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

typedef int tid_t;

struct open_file 
{
  int fd;
  struct file* file;
  struct list_elem elem;
};

struct process
{
  tid_t pid;
  struct process* parent;
  struct file *exec_file;
  struct list child_list;
  struct list_elem child_elem;
  struct semaphore sema_wait; 
  tid_t waiting_for;
  enum status status;
  int exec_success;
  bool been_waited;

  struct list file_list;
  int next_fd;
};

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
struct process* get_current_process(void);
struct open_file* find_open_file(int);

#endif /* userprog/process.h */

