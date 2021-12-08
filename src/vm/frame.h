#include <hash.h>
#include <list.h>

struct frame 
{
  struct hash_elem hash_elem; 
  struct list_elem list_elem;
  void* kernel_page;
  void* user_page;
  
}

