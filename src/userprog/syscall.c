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
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);
struct lock filesys_lock;

void
check_user_pointer (void* ptr)
{ 
  if(ptr == NULL || ptr >= PHYS_BASE)
    sys_exit(-1);
}

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
  proc->status = status;
  thread_exit();
}


pid_t
sys_exec(void *esp)
{ 
  struct process *proc = get_current_process();
  proc->exec_success = -1; 
  lock_acquire(&filesys_lock);
  char* cmd_line = *(char**)(esp+4);
  check_user_pointer(cmd_line);
  pid_t pid = process_execute(cmd_line);
  lock_release(&filesys_lock);
    proc->waiting_for = pid; 
    sema_down(&proc->sema_wait);
  if(proc->exec_success == 0)
    pid = -1;
     
  return pid;
}

int
sys_wait(void *esp)
{ 
  pid_t pid = *(pid_t *)(esp+4);
  int status =  process_wait(pid);
  return status;
}

int
sys_write(void *esp)
{
  int fd = *(int*)(esp + 4);
  void *buffer = *(void **)(esp + 8);
  unsigned size = *(unsigned *)(esp + 12);
  check_user_pointer(buffer);
  if(fd == 1){
    putbuf(buffer, size);
    return size;
  } 
  struct open_file *of = find_open_file(fd);
  if(of == NULL)
    return -1;
  lock_acquire(&filesys_lock);
  int bytes_read = file_write(of->file, buffer, size);
  lock_release(&filesys_lock);
  return bytes_read;
}

bool
sys_create(void * esp)
{
  char *file = *(char **)(esp + 4);
  check_user_pointer(file);
  unsigned initial_size = (unsigned)get_user((esp + 8));
  lock_acquire(&filesys_lock);
  bool succes = filesys_create(file, initial_size); 
  lock_release(&filesys_lock);
  return succes;
}

bool
sys_remove(void *esp)
{

  char *file = *(char **)(esp + 4);
  check_user_pointer(file);
  lock_acquire(&filesys_lock);
  bool succes = filesys_remove(file); 
  lock_release(&filesys_lock);
  return succes; 
}

int
sys_open(void *esp)
{
  char *file_name = *(char **)(esp + 4);
  check_user_pointer(file_name);
  lock_acquire(&filesys_lock);
  int fd = -1;
  struct file *file = filesys_open(file_name); 
  if (file != NULL){
   struct open_file *of = (struct open_file*)malloc(sizeof(struct open_file));
   of->file = file; 
   of->fd = get_current_process()->next_fd;
   fd = of->fd;
   get_current_process()->next_fd++;
   list_push_front(&get_current_process()->file_list, &of->elem);
  }
  lock_release(&filesys_lock);
  return fd;
}

int
sys_filesize(void *esp)
{
  int fd = *(int*)(esp+4);
  struct open_file *of = find_open_file(fd);
  if (of == NULL)
    return -1;
  lock_acquire(&filesys_lock);
  int size = file_length (of->file); 
  lock_release(&filesys_lock);
  return size;
}

int 
sys_read (void* esp)
{
  int fd = *(int*)(esp + 4);
  void *buffer = *(void **)(esp + 8);
  unsigned size = *(unsigned *)(esp + 12);
  check_user_pointer(buffer);
  if(fd == 0){
    input_getc();
    return size;
  }

  struct open_file *of = find_open_file(fd);
  if(of == NULL)
    return -1;
  lock_acquire(&filesys_lock);
  int bytes_read = file_read(of->file, buffer, size);
  lock_release(&filesys_lock);
  return bytes_read;
}

void
sys_seek (void* esp)
{
  int fd = *(int*)(esp + 4);
  unsigned pos = *(unsigned *)(esp + 8);
  struct open_file *of = find_open_file(fd);
  if(of != NULL)
  {
    lock_acquire(&filesys_lock);
    file_seek(of->file, pos);
    lock_release(&filesys_lock);
  }
}

unsigned
sys_tell (void* esp)
{
  int fd = *(int*)(esp + 4);
  struct open_file *of = find_open_file(fd);
  if(of != NULL)
  {
    lock_acquire(&filesys_lock);
    unsigned pos =  file_tell(of->file);
    lock_release(&filesys_lock);
    return pos;
  }
}

void
sys_close (int fd)
{
  struct open_file *of = find_open_file(fd);
  if(of != NULL)
  {
    lock_acquire(&filesys_lock);
    file_close(of->file);
    list_remove(&of->elem); 
    free(of);
    lock_release(&filesys_lock);
  }
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
      f->eax = sys_exec(esp);
      break;
    case SYS_WAIT:
      check_pointer(esp, 1);
      f->eax = sys_wait(esp);
      break;
    case SYS_WRITE:
      check_pointer(esp, 3);
      f->eax = sys_write(esp);
      break;
    case SYS_CREATE:
      check_pointer(esp, 2);
      f->eax = sys_create(esp); 
      break;
     case SYS_REMOVE:
      check_pointer(esp, 1);
      f->eax = sys_remove(esp); 
      break;
     case SYS_OPEN:
      check_pointer(esp, 1);
      f->eax = sys_open(esp); 
      break;
     case SYS_FILESIZE:
      check_pointer(esp, 1);
      f->eax = sys_filesize(esp); 
      break;
     case SYS_READ:
      check_pointer(esp, 3);
      f->eax = sys_read(esp); 
      break;
     case SYS_SEEK:
      check_pointer(esp, 2);
      sys_seek(esp); 
      break;
     case SYS_TELL:
      check_pointer(esp, 1);
      f->eax = sys_tell(esp);
      break;
     case SYS_CLOSE:
      check_pointer(esp, 1);
      sys_close(*(int*)(esp + 4));
        break;
     default:
      sys_exit(-1); 
	}
  //thread_exit ();
}
