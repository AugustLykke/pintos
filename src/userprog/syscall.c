#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/user/syscall.h"
#include "threads/synch.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);
struct lock filesys_lock;

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&filesys_lock);
}

static int
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
        : "=$a" (result) : "m" (*uaddr));
  return result;
}

static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;

  asm ("movl $1f, %0; movb %b2, %1; 1:"
        : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code  != -1;
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
  struct process *proc =  thread_current()->process;
  int final_status = status == 0 ? 0 : -1;
  proc->status = final_status;
  thread_exit();
}


pid_t
sys_exec(void *esp)
{ 
  char* cmd_line = *(char*)(esp+4);
  pid_t succes = process_execute(cmd_line);
  return succes;
}


int
sys_write(void *esp)
{
  int fd = *(int*)(esp + 4);
  void *buffer = *(void **)(esp + 8);
  unsigned size = *(unsigned *)(esp + 12);
  if(fd == 1){
    putbuf(buffer, size);
    return size;
  }
}

bool
sys_create(void * esp)
{
    
}


void
check_pointer(void* esp, int argc)
{
 if(esp + 4 * (argc+1) > PHYS_BASE)
   sys_exit(-1);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	void *esp = f->esp;
	uint32_t *eax = f->eax;
  int intr_num = *(int*)esp;
	switch (intr_num)
	{
    case SYS_HALT:
      check_pointer(esp, 0);
      sys_halt();
      break;
    case SYS_EXIT:
      check_pointer(esp, 1);
      sys_exit(*(int*)(esp + 4));
      break;
    case SYS_EXEC:
      check_pointer(esp, 1);
      *eax = sys_exec(esp);
      break;
    case SYS_WRITE:
      check_pointer(esp, 3);
      *eax = sys_write(esp);
      break;
    case SYS_CREATE:
      check_pointer(esp, 2);
      sys_create(esp); 
      break;
	}
  //thread_exit ();
}
