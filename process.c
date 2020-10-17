#include "liga.h"
#include "estring.h"
#include "variable.h"
#include "builtin.h"
#include "process.h"
#define le ->content->l
#define bf ->content->buf
#define ls ->content->lstr
process *setProcess(void){
  process *p=malloc(sizeof(process));
  p->cstr=NULL;
  p->Then=NULL;
  p->Else=NULL;
  p->Back=NULL;
  p->While=NULL;
  p->Next=NULL;
  p->is_break=0;
  p->content=malloc(sizeof(node));
  p le=0;
  p bf=BUF_BLOCK;
  p ls=malloc(C_SIZE*(BUF_BLOCK));
  return p;
}
int do_process(process *p){
  if (p==NULL)
    return 0;
  else{
    for (int i=0;i< p le;i++)
      do_cmd_line(p ls[i]);
    if (p->is_break)
      return 1;
    if (p->cstr==NULL){
      return 0;
    }
    else{
      if (p->While!=NULL){
        while (do_cmd_line(p->cstr))
          if (do_process(p->While))
            break;
      }
      else{
        if (do_cmd_line(p->cstr))
          do_process(p->Then);
        else{
          do_process(p->Else);
        }
      }
      return do_process(p->Next);
    }
  }
}
int liga(void){
  char **oldenv;
  oldenv=environ;
  environ=copy_list_str(environ);
  int on_process=0;
  signal(SIGINT,SIG_IGN);
  signal(SIGQUIT,SIG_IGN);
  int tail,result=0,flag=1;
  char *s;
  process *p=setProcess();
  while ((s=getLine(stdin,flag))!=NULL){
    if (*s==NL)
      continue;
    if (string_eq(s,"if",SPACE)){
      on_process=1;
      if (p->cstr==NULL){
        p->cstr=head_ns_newstr(s+2,strlen(s)-2);
        free(s);
      }
      else{
        process *tmp=setProcess();
        p->Next=tmp;
        tmp->cstr=head_ns_newstr(s+2,strlen(s)-2);
        free(s);
        tmp->Back=p->Back;
        p=tmp;
      }
    }
    else if (string_eq(s,"then",SPACE)){
      process *tmp=setProcess();
      p->Then=tmp;
      if ((tail=strlen(s)-4)>0)
        tmp ls[(tmp le)++]=head_ns_newstr(s+4,tail);
      tmp->Back=p;
      p=tmp;
    }
    else if (string_eq(s,"else",SPACE)){
      process *tmp=setProcess();
      p=p->Back;
      p->Else=tmp;
      if ((tail=strlen(s)-4)>0)
        tmp ls[(tmp le)++]=head_ns_newstr(s+4,tail);
      tmp->Back=p;
      p=tmp;
    }
    else if (string_eq(s,"while",SPACE)){
      on_process=1;
      if (p->cstr==NULL){
        p->cstr=head_ns_newstr(s+5,strlen(s)-5);
        process *tmp=setProcess();
        p->While=tmp;
        tmp->Back=p;
        p=tmp;
        free(s);
      }
      else{
        process *tmp=setProcess();
        process *tmp1=setProcess();
        p->Next=tmp;
        tmp->cstr=head_ns_newstr(s+5,strlen(s)-5);
        free(s);
        tmp->Back=p->Back;
        tmp->While=tmp1;
        tmp1->Back=tmp;
        p=tmp1;
      }
    }
    else if (string_eq(s,"fi",SPACE)){
      p=p->Back;
      if (p->Back==NULL){
        on_process=0;
        do_process(p);
        free_process(p);
        p=setProcess();
      }
    }
    else if (string_eq(s,"end",SPACE)){
      p=p->Back;
      if (p->Back==NULL){
        on_process=0;
        do_process(p);
        free_process(p);
        p=setProcess();
      }
    }
    else if (string_eq(s,"break",SPACE)){
      p->is_break=1;
    }
    else if (on_process==0){
      result=do_cmd_line(s);
      free(s);
    }
    else{
      if (p->cstr==NULL){
        if (p le>=p bf){
          p ls=realloc(p ls,(p bf+1)*C_SIZE);
          (p bf)++;
        }
        p ls[(p le)++]=newstr(s,strlen(s));
        free(s);
      }
      else{
        process *tmp=setProcess();
        p->Next=tmp;
        tmp ls[(tmp le)++]=newstr(s,strlen(s));
        tmp->Back=p->Back;
        p=tmp;
        free(s);
      }
    }
  }
  puts("\rLiga-Shell exited!!");
  return 0;
}
void free_process(process *p){
  if (p==NULL);
  else{
    free_node(p->content);
    free(p->cstr);
    free_process(p->Then);
    free_process(p->Else);
    free_process(p->Next);
    free_process(p->While);
    free(p);
  }
}
void free_node(node *q){
  if (q==NULL);
  else{
    for (int i=0;i<q->l;i++){
      free(q->lstr[i]);
    }
    free(q);
  }
}
