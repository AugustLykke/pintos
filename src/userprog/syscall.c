#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	void *esp = f->esp;
	uint32_t* eax = f->eax;
  printf ("system call!\n");
  int intr_num = *esp;
  esp += 4;
	switch (intr_num)
	{
    case SYS_HALT:
      sys_halt();
      break;
    case SYS_EXIT:
      sys_exit(*(int*)esp);
      break;
    case SYS_EXEC:
      *eax = sys_exec(esp);
      break;
    case SYS_WRITE:
      *eax = sys_write(esp);
	}

  thread_exit ();
}


void
check_pointer(void* esp)
{
  if(!is_user_vaddr(esp) || esp+4 <= PHYS_BASE){
    sys_exit(-1);
  }
}

void
sys_halt()
{
  shutdown_power_off();
}

void
sys_exit(int status)
{ 
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}
/*
int
sys_exec(uint32_t *esp)
{ 
  char* cmd_line = read_pointer();
  process_execute(cmd_line);
}
*/

int
sys_write(void* esp)
{
  int fd = *(int*)esp;
  esp += 4;

}
