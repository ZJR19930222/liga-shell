#ifndef _process__
#define _process__
typedef struct _node {
  char **lstr;
  int l;
  int buf;
} node;
typedef struct _process {
  char *cstr;
  node *content;
  struct _process *Then;
  struct _process *Back;
  struct _process *Else;
  struct _process *While;
  struct _process *Next;
  int is_break;
} process;
process *setProcess(void);
int do_process(process *p);
void free_process(process *p);
void free_node(node *q);
int liga(void);
#endif
