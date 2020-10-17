#include "liga.h"
#include "estring.h"
#include "variable.h"
#include "builtin.h"
#define FDBACK(x) (((x)==0)?1:0)
void fatal(char *estr,int errnum){
  fprintf(stderr,"error:>%s\n",estr);
  exit(errnum);
}
/*[ ... ]條件判斷
var x=$liga or var x=talent 賦值
x = $y+2 四則運算
unset x 刪除變量
export x=chinese y 添加環境變量
*/
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
