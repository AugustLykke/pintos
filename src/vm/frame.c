#include <hash.h>
#include "vm/frame.h" 

struct hash frame_table;
struct list frame_queue;
struct list_elem clock_pointer;

unsigned
vm_frame_hash_func(const struct hash_elem *e, void *aux)
{
  struct frame = hash_entry(e, struct frame, hash_elem);
  return hash_byte( &frame->kernel_page, sizeof frame->kernel_page);
}

bool
vm_frame_less_func(const struct hash_elem *a, 
                   const struct hash_elem *b,
                   void *aux)
{
  struct frame_a = hash_entry(a, struct frame, hash_elem);
  struct frame_b = hash_entry(b, struct frame, hash_elem);
  return frame_a->kernel_page < frame_b->kernel_page;
}

void
vm_frame_init()
{
  hash_init(&frame_table);
  list_init(&frame_queue);
}

void*
vm_obtain_frame
{
  void* new_frame = palloc_get_page(PAL_USER);
  if(new_frame == NULL) 
  {

  }
  return new_frame;
}

void
evict_frame
{
  
}


void
vm_frame_table_init()
{
  hash_init(&frame_map, vm_frame_hash_func, frame_less_func);
  list_init(&frame_queue);
}



