#include "liga.h"
#include "estring.h"
#define _Var__
#include "variable.h"
#define MAXVARS 100
/*自定义variable数据结构*/
var varTable[MAXVARS];
int varnum=0;
/***********************/
/*测试*/
//int main(void){
//  char *ps[]={"lang=china","liga=mylove","c++=c plus",NULL};
//  puts("---------");
//  putVar("javascript=art",0);
//  putVar("son=script",1);
//  putVar("c=c++",1);
//  putVar("liga=nightmuse",0);
//  putVar("ll2=11",0);
//  puts("---------");
//  puts(substVar("hh=${java$son}+$java$son +$c / $javascript  "));
//  return 0;
//}
int putVar(char *expr,int Bool){
  char *eq=strchr(expr,'=');
  for (int i=0;i<varnum;i++){
    if (string_eq(varTable[i].str,expr,'=')){
      /*匹配表中的项*/
      if (eq!=NULL){
        free(varTable[i].str);
        varTable[i].str=newstr(expr,strlen(expr));
      }
      if (Bool) // global=0,1
        varTable[i].global=1;
      return 1;
    }
  }
  if (eq==NULL)
    return 0;
  varTable[varnum].str=newstr(expr,strlen(expr));
  if (Bool)
    varTable[varnum].global=1;
  else
    varTable[varnum].global=0;
  varnum++;
  return 1;
}
char *getVar(char *expr){
  char *s;
  char *t;
  for (int i=0;i<varnum;i++){
    s=varTable[i].str;
    t=strchr(s,'=');
    if (string_eq(expr,s,'=')){
      t++;
      return newstr(t,strlen(s)-(t-s));
    }
  }
  return NULL;
}
void delVar(char *expr){
  for (int i=0;i<varnum;i++){
    if (string_eq(varTable[i].str,expr,'=')){
      free(varTable[i].str);
      for (int j=i;j+1<varnum;j++){
        varTable[j].str=varTable[j+1].str;
        varTable[j].global=varTable[j+1].global;
      }
      varTable[varnum-1].str=NULL;
      varTable[varnum-1].global=0;
      varnum--;
      break;
    }
  }
}
void showVar(void){
  for (int i=0;i<varnum;i++){
    if (varTable[i].global)
      printf("*%s\n",varTable[i].str);
    else
      printf("%s\n",varTable[i].str);
  }
}
int add_env(char ***s,char *q){
  char *t=newstr(q,strlen(q));
  char *p1=newstr(t,(strchr(t,'=')-t+1));
  char *p2;
  int i;
  for (i=0;*(*s+i);i++){
    p2=newstr(*(*s+i),strchr(*(*s+i),'=')-*(*s+i)+1);
    if (strcmp(p2,p1)==0){
      *(*s+i)=t;
      free(p1);
      free(p2);
      return 1;
    }
    free(p2);
}
free(p1);
*(*s+i)=t;
*s=realloc(*s,(i+2)*C_SIZE);
*(*s+i+1)=NULL;
return 1;
}
void varToEnv(char ***s){
  for (int i=0;i<varnum;i++){
    if (varTable[i].global){
      add_env(s,varTable[i].str);
    }
  }
}
char *substVar(char *a){
  char *p=strchr(a,VARSUB);
  char *k,*s=p+1;
  int l=1;
  if (p==NULL)
    return a;
  if (*(p+1)=='{'){
    while (l !=0){
      s++;
      if (*s=='}')
        l--;
      else if (*s=='{')
        l++;
    }/*9[a]57$[p]{javascript}[s]jjj*/
    if ((k=getVar(substVar(newstr(p+2,s-p-2))))==NULL)
      return comStr(newstr(a,s-a+1),substVar(s+1));
    return comStr(comStr(newstr(a,p-a),k),substVar(s+1));
  }
  else{
    while(l){
      switch(*s){
        case SPACE:
        case PLUS:
        case SUBTR:
        case MODULE:
        case DIVIDE:
        case STAR:
        case VARSUB:
        case NL:
          l=0;break;
        default:
          s++;
      }
    }
    /*W[a]BC$[p]zjr+[s]liga*/
    if ((k=getVar(newstr(p+1,s-p-1)))==NULL)
      return comStr(newstr(a,s-a),substVar(s));
    return comStr(comStr(newstr(a,p-a),k),substVar(s));
  }
}
