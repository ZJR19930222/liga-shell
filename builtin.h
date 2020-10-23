#ifndef _builtin__
#define _builtin__
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
//typedef int (*func)(char*);
enum _logic {AND,OR,ANY};
void fatal(char *estr,int errnum);
int unset(char *expr);
int Export(char *expr);
int value(char *expr);
int choice(char *expr);
int cddir(char *expr);
char *work_path(void);
int do_cmd(enum _logic x,int status,char *cmd);
int do_cmd_line(char *expr);
int replace(char *expr);
int setREP(char *expr);
#endif
