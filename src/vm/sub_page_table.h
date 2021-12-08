enum page_status 
{
  FRAME,
  SWAP,
  FILE,
  ZERO
};

struct sub_page 
{
  void *user_page;
  void *kernel_page;
  struct hash_elem elem;
  enum page_status staus;
};

