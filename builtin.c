#include "liga.h"
#include "estring.h"
#include "variable.h"
#include "builtin.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/mman.h>
#define PROMPT_SYMBOL "➣ "
#define FDBACK(x) (((x)==0)?1:0)
#define FLAG_MM PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,-1,0
char PROMPT[2*PATH_MAX];
int REP='#';
typedef struct _path_tr {
  char *buf;
  size_t num_alloc;
  char *ptr_start;
} path_tr;
typedef struct _dev_ino {
  ino_t st_ino;
  dev_t st_dev;
} dev_ino;
void fatal(char *estr,int errnum){
  fprintf(stderr,"error:>%s\n",estr);
  exit(errnum);
}
static dev_ino *get_root_dino(dev_ino *rootdi){
  struct stat tmp;
  if (lstat("/",&tmp))
    return NULL;
  rootdi->st_ino=tmp.st_ino;
  rootdi->st_dev=tmp.st_dev;
  return rootdi;
}
static void free_path_tr(path_tr *ptr){
  free(ptr->buf);
  free(ptr);
}
static path_tr *init_path_ptr(void){
  path_tr *ptr=malloc(sizeof(path_tr));
  ptr->num_alloc=2*PATH_MAX;
  ptr->buf=malloc(ptr->num_alloc);
  ptr->ptr_start=ptr->buf+ptr->num_alloc-1;
  ptr->ptr_start[0]=NL;
  return ptr;
}
static void prepend_path(path_tr *origin,const char *name,size_t name_len){
  size_t num_free=origin->ptr_start - origin->buf;
  if (num_free < 1 + name_len){
    size_t new_alloc=origin->num_alloc + 1 + name_len;
    char *new_buf=malloc(new_alloc);
    size_t num_used=origin->num_alloc - num_free;
    origin->ptr_start=new_buf + new_alloc - num_used;
    memcpy(origin->ptr_start,origin->buf + num_free,num_used);
    free(origin->buf);
    origin->buf=new_buf;
    origin->num_alloc=new_alloc;
  }
  origin->ptr_start -= 1 + name_len;
  origin->ptr_start[0]='/';
  memcpy(origin->ptr_start+1,name,name_len);
}
static char *nth_parent_dir(size_t n){
  char *new=malloc(3*n);
  char *var=new;
  for (size_t i=0;i<n;i++){
    memcpy(var,"../",3);
    var += 3;
  }
  var[-1]=NL;
  return new;
}
static void current_dir(struct stat *now_st,path_tr *path){
  DIR *dir_ptr;
  int fd;
  struct stat parent_st;
  int is_diff_dev=0;
  dir_ptr=opendir("..");
  if (dir_ptr==NULL)
    fatal("open parent_st failed",1);
  fd=dirfd(dir_ptr);
  if ((0<=fd ? fchdir(fd):chdir(".."))<0)
    fatal("cannot change cwd",1);
  if ((0<=fd ? fstat(fd,&parent_st):stat(".",&parent_st))<0)
    fatal("cannot change cwd",1);
  is_diff_dev=(parent_st.st_dev != now_st->st_dev);
  while (1) {
    struct dirent const *dpt;
    struct stat entry_st;
    ino_t ino;
    errno = 0;
    if ((dpt=readdir(dir_ptr))==NULL){
      if ( errno ){
        int e=errno;
        closedir(dir_ptr);
        errno=e;
      }
      break;
    }
    if (strcmp(dpt->d_name,".")==0 || strcmp(dpt->d_name,"..")==0)
      continue;
    ino=dpt->d_ino;
    if ( is_diff_dev ){
      if (lstat(dpt->d_name,&entry_st) < 0)
        continue;
      ino=entry_st.st_ino;
    }
    if (ino != now_st->st_ino)
      continue;
    if ( ! is_diff_dev || entry_st.st_dev==now_st->st_dev){
      prepend_path(path,dpt->d_name,strlen(dpt->d_name));
      break;
    }
  }
  if (errno || closedir(dir_ptr) !=0)
    fatal("error",errno);
  *now_st=parent_st;// only struct can do this
}
static void robust_getcwd(path_tr *path){
  dev_ino dev_ino_buf;
  dev_ino *rootdi=get_root_dino(&dev_ino_buf);
  struct stat cwd_st;
  if (rootdi==NULL)
    fatal("cannot get root ino and dev",1);
  if (stat(".",&cwd_st)<0)
    fatal("cannot get status of cwd",1);
  while (1) {
    if (cwd_st.st_dev==rootdi->st_dev && cwd_st.st_ino==rootdi->st_ino)
      break;
    current_dir(&cwd_st,path);
  }
  if (path->ptr_start[0]=='\0')// already in root
    prepend_path(path,"",0);
}
char *work_path(void){
  int child;
  char *path_buf=mmap(NULL,sizeof(char)*2*PATH_MAX,FLAG_MM);
  pid_t pid=fork();
  if (pid<0){
    fatal("fork error",1);
  }
  else if (pid==0){
    path_tr *path=init_path_ptr();
    robust_getcwd(path);
    strcpy(path_buf,path->ptr_start);
    free_path_tr(path);
    exit(0);
  }
  else {
    if (wait(&child)==-1){
      fatal("wait error",1);
    }
  }
  sprintf(PROMPT,"\e[41m%s\e[0m~\e[35m%s\e[0m",path_buf,PROMPT_SYMBOL);
  munmap(path_buf,sizeof(char)*2*PATH_MAX);
  return PROMPT;
}
int unset(char *expr){
  char **s=splitLine(expr);
  for (int i=0;*(s+i);i++)
    delVar(*(s+i));
  freelist(s);
  return 1;
}
int Export(char *expr){
  char **s=splitLine(expr);
  for (int i=0;*(s+i);i++)
    putVar(*(s+i),1);
  varToEnv(&environ);
  freelist(s);
  return 1;
}
int value(char *expr){
  char *eq=strchr(expr,'=');
  char *variable=ns_newstr(expr,eq-expr);
  char *value=operand(eq+1,expr+strlen(expr)-1);
  char *result=malloc(strlen(variable)+strlen(value)+2);
  strcpy(result,variable);
  strcpy(result+strlen(variable),"=");
  strcpy(result+strlen(variable)+1,value);
  putVar(result,0);
  free(value);
  free(variable);
  free(result);
  return 1;
}
int display(char *expr){
  showVar();
  return 1;
}
int replace(char *obj,char* patten);
int cddir(char *expr){
  int restat;
  restat=chdir(expr);
  if (restat==0){
    /*change successful!*/
    if (*expr=='/'){
      /*noting to do*/
      sprintf(PROMPT,"\e[41m%s\e[0m~\e[35m%s\e[0m",expr,PROMPT_SYMBOL);
      PROMPT[strlen(expr)+21]=NL;
    }
    else{
      char *var=expr;
      char *tilda=strchr(PROMPT,'~')-5;
      while(1){
        if (*var=='/'){
          var++;
          continue;
        }
        else if (*var==DOT && *(var+1)==DOT){
          while ((tilda-PROMPT)>=5 && *tilda-- != '/');
          var += 2;
        }
        else if (*var!=NL){
          *++tilda='/';
          int l=strlen(var);
          memcpy(tilda+1,var,l);
          tilda=tilda+1+l;
          strcpy(tilda,"\e[0m~\e[35m➣ \e[0m");
          break;
        }
        else{
          tilda=tilda+1;
          strcpy(tilda,"\e[0m~\e[35m➣ \e[0m");
          break;
        }
      }
    }
    return 1;
  }
  puts("cd error");
  return 0;
}
int choice(char *expr){
  if (string_eq(expr,"export",SPACE)){
    /*6=strlen("export")*/
    return Export(expr+6);
  }
  else if (string_eq(expr,"unset",SPACE)){
    /*5=strlen("unset")*/
    return unset(expr+5);
  }
  else if (string_eq(expr,"var",SPACE)){
    /*3=strlen("var")*/
    putVar(nns_newstr(expr+3,strlen(expr)-3),0);
    return 1;
  }
  else if (string_eq(expr,"cd",SPACE)){
    expr+=2;
    while (IS_SPACE(*expr))
      expr++;
    return cddir(expr);
  }
  else if (isRegMatch(expr,expPatten("[a-z_][a-z_]+ *=.*"))){
    return value(expr);
  }
  else if (isRegMatch(expr,"\\[.*]")){
    return condition(expr);
  }
  else if (string_eq(expr,"display",SPACE)){
    return display(NULL);
  }
  else
    return -1;
}
int do_cmd(enum _logic x,int status,char *cmd){
  /*do_cmd_line maybe create a space ""*/
  if (*cmd==NL){
    return status;
  }
  if ((x==AND && status==0)||(x==OR && status==1))
    return status;
  int child_info;
  int pid;
  int result;
  /*$name-->value*/
  cmd=substVar(cmd);
  if ((result=choice(cmd))>=0){
    return result;
  }
  else{
    char **args=splitLine(cmd);
    if ((pid=fork())==-1){
      fatal("fork()",1);
    }
    else if (pid==0){
      signal(SIGINT,SIG_DFL);
      signal(SIGQUIT,SIG_DFL);
      execvp(args[0],args);
      fprintf(stderr,"cannot do %s\n",args[0]);
      exit(1);
    }
    else{
      if (wait(&child_info)==-1)
        fatal("wait()",1);
      if (x==AND)
        status &=FDBACK(child_info);
      else if (x==OR)
        status |=FDBACK(child_info);
      else
        status = FDBACK(child_info);
    }
  freelist(args);
  }
  return status;
}
int do_cmd_line(char *e){
  int status=1,k;
  char *s=e;
  char *p=NULL;
  enum _logic logic=ANY;
  do{
    if (*e=='&' && *(e+1)=='&'){
      k=0;
      while (IS_SPACE(*(e-k-1)))
        k++;
      p=newstr(s,e-s-k);
      status=do_cmd(logic,status,p);
      free(p);
      e += 2;
      while (IS_SPACE(*e))
        e++;
      s=e;
      logic=AND;
    }
    else if (*e=='|' && *(e+1)=='|'){
      k=0;
      while (IS_SPACE(*(e-k-1)))
        k++;
      p=newstr(s,e-s-k);
      status=do_cmd(logic,status,p);
      free(p);
      e += 2;
      while (IS_SPACE(*e))
        e++;
      s=e;
      logic=OR;
    }
    else if (*e==';'){
      k=0;
      while (IS_SPACE(*(e-k-1)))
        k++;
      p=newstr(s,e-s-k);
      status=do_cmd(logic,status,p);
      free(p);
      e++;
      while (IS_SPACE(*e))
        e++;
      s=e;
      logic=ANY;
    }
    else if (*e=='\0'){
      p=newstr(s,(e-s));
      status=do_cmd(logic,status,p);
      free(p);
      break;
    }
    else{
      e++;
    }
  }while(1);
  return status;
}
//int main(void){
//  char *ss;
//  if ((ss=work_path())!=NULL){
//    printf("<%s>\n",ss);
//    exit(0);
//  }
//    printf("<%s>\n",ss);
//  return 0;
//}
