typedef struct process
{
  struct process *next;       
  char **argv;                 
  char completed;             
  char stopped;               
  int status;                 
} process;

