#include "liga.h"
#include "estring.h"
extern char PROMPT[];
int string_eq(char *obj1,char *obj2,int ch){
  char *c1=strchrnul(obj1,ch);
  char *c2=strchrnul(obj2,ch);
  if ((c1-obj1==c2-obj2)&&strncmp(obj1,obj2,c1-obj1)==0)
    return 1;
  return 0;
}
char *newstr(char *obj,int len){
  char *new=malloc(len+1);
  strncpy(new,obj,len);
  new[len]=NL;
  return new;
}
char *ns_newstr(char *obj,int len){
  char *new=malloc(len+1);
  int n=0;
  for (int i=0;i<len;i++){
    if (! IS_SPACE(*(obj+i)))
      new[n++]=*(obj+i);
  }
  new[n]=NL;
  return new;
}
char *nns_newstr(char *obj,int len){
  char *new=malloc(len+1);
  int n=0,dq=0;
  for (int i=0;i<len;i++){
    if (*(obj+i)==SPACE && dq==0){
      continue;
    }
    else if (*(obj+i)==DQ){
      dq=~dq;
    }
    new[n++]=*(obj+i);
  }
  new[n]=NL;
  return new;
}
char *delspace(char *obj){
  char *new=malloc(strlen(obj)+1);
  int n=0;
  int i=0;
  while (*(obj+i) != NL){
    if (! IS_SPACE(*(obj+i)))
      new[n++]=*(obj+i);
    i++;
  }
  new[n]=NL;
  return new;
}
void show_list_str(char **lstr){
  char **k=lstr;
  while ( *k )
    printf("<%s>\n",*k++);
}
void freelist(char **lstr){
  if (lstr==NULL);
  else{
    char **k=lstr;
    while ( *k )
      free(*k++);
    free(lstr);
  }
}
char **copy_list_str(char **lstr){
  char **new=malloc(C_SIZE*BUF_BLOCK);
  int buf=BUF_BLOCK,n=0;
  while ( *(lstr+n) ){
    if (n+1>=buf){
      new=realloc(new,(buf+1)*C_SIZE);
      buf += 1;
    }
    *(new+n)=newstr(*(lstr+n),strlen(*(lstr+n)));
    n++;
  }
  *(new+n)=NULL;
  return new;
}
char *comStr(char *head,char *tail){
  char *new=malloc(strlen(head)+strlen(tail)+1);
  strcpy(new,head);
  strcpy(new+strlen(head),tail);
  return new;
}
char *comStr_l(char *head,char *tail,int i,int j){
  char *new=malloc(i+j+1);
  strncpy(new,head,i);
  strncpy(new+i,tail,j);
  new[i+j]=NL;
  return new;
}
int is_digit_str(char *obj,int len){
  if (len==0)
    return 0;
  int j=0;
  if (*obj==SUBTR || *obj==PLUS){
    if (len==1)
      return 0;
    j=1;
  }
  for (;j<len;j++){
    if (isdigit(*(obj+j))==0)
      return 0;
  }
  return 1;
}
int is_digit(char *obj){
  return is_digit_str(obj,strlen(obj));
}
char *str_end_chr(char *expr,int ch){
  char *ptr=NULL;
  for (;*expr!=NL;expr++){
    if (ch==*expr)
      ptr=expr;
  }
  return ptr;
}
int strToNum(char *obj,int len){
  char *end=obj+len-1,*start=obj;
  int base=1,result=0,sign=1;
  if (*obj==SUBTR){
    start++;
    sign=-1;
  }
  if (*obj==PLUS)
    start++;
  for (;end>=start;end--){
    result += base*((int)*end-48);
    base *= 10;
  }
  return sign*result;
}
int strToi(char *obj){
  return strToNum(obj,strlen(obj));
}
char *expPatten(char *origin){
  int buf=BUF_BLOCK,n=0;
  char *new=malloc(buf);
  for(;;){
    if (*origin==SLASHES&&*(origin+1)==LP){
      if (n+2>=buf){
        new=realloc(new,buf+BUF_BLOCK);
        buf += BUF_BLOCK;
      }
      new[n++]=*origin++;
      new[n++]=*origin++;
    }
    else if (*origin==LP){
      if (n+2>=buf){
        new=realloc(new,buf+BUF_BLOCK);
        buf += BUF_BLOCK;
      }
      new[n++]=*origin++;
      new[n++]=*origin++;
      while (*origin !=RP&&*origin !=NL){
        if (*origin==SUBTR&&*(origin+1)!=RP){
          if (*(origin+1)>*(origin-1)){
            for (int c=*(origin-1);c<*(origin+1);c++){
              if (n+1>=buf){
                new=realloc(new,buf+BUF_BLOCK);
                buf += BUF_BLOCK;
              }
              new[n++]=c+1;
            }
          }
          else{
            for (int c=*(origin-1);c>*(origin+1);c--){
              if (n+1>=buf){
                new=realloc(new,buf+BUF_BLOCK);
                buf += BUF_BLOCK;
              }
              new[n++]=c+1;
            }
          }
          origin += 2;
        }
        else{
          if (n+1>=buf){
            new=realloc(new,buf+BUF_BLOCK);
            buf += BUF_BLOCK;
          }
          new[n++]=*origin++;
        }
      }
    }
    else{
      if (n+1>=buf){
        new=realloc(new,buf+BUF_BLOCK);
        buf += BUF_BLOCK;
      }
      new[n++]=*origin++;
    }
    if (*origin==NL){
      new[n]=NL;
      break;
    }
  }
  return new;
}
int isRegMatch(char *obj,char *ptn){
  if (*obj==NL&&*ptn==NL)
    return 1;
  else if (*ptn!=NL){
    if (*ptn==STAR){
      return isRegMatch(obj,ptn+1);
    }
    else if (*ptn==DOT&&*(ptn+1)==STAR){
      for(;;){
        if (isRegMatch(obj,ptn+2)){
          return 1;
        }
        if (*(obj++)==NL)
          return 0;
      }
    }
    else if (*ptn==DOT&&*(ptn+1)!=STAR){
      if (*obj!=NL)
        return isRegMatch(obj+1,ptn+1);
    }
    else if (*ptn==LP){
      int k=0;
      while (*(ptn+k+1)!=RP&&*(ptn+k+1)!=NL){
        k++;
      }
      if (*(ptn+k+1)==RP&&*(ptn+k+2)==STAR){
        char *ch=obj;
        for (int i=1;i<=k;i++){
          if (*ch==*(ptn+i)){
            do{
              if (isRegMatch(++ch,ptn+k+3)){
                return 1;
              }
            }while(*ch==*(ptn+i));
            break;
          }
        }
        return isRegMatch(obj,ptn+k+3);
      }
      else if (*(ptn+k+1)==RP&&*(ptn+k+2)==PLUS){
        char *ch=obj;
        int flag;
        do{
          flag=0;
          if (isRegMatch(ch,ptn+k+3)){
            return 1;
          }
          for (int i=1;i<=k;i++){
            if (*ch==*(ptn+i)){
              flag=1;
              break;
            }
          }
          ch++;
        }while(flag);
        return isRegMatch(obj,ptn+k+3);
      }
      else if (*(ptn+k+1)==RP){
        for (int i=1;i<=k;i++){
          if (*obj==*(ptn+i)){
            return isRegMatch(obj+1,ptn+k+2);
          }
        }
      }
      else if (*(ptn+k+1)==NL&&*(obj+1)==NL){
        for (int i=1;i<=k;i++){
          if (*obj==*(ptn+1)){
            return 1;
          }
        }
      }
    }
    else if (*ptn==SLASHES&&*(ptn+1)==LP&&*(ptn+2)==STAR){
      do {
        if (isRegMatch(obj,ptn+3)){
          return 1;
        }
      }while(*(obj++)==LP);
    }
    else if (*ptn==SLASHES&&*(ptn+1)==LP&&*(ptn+2)!=STAR){
      if (*obj==LP){
        return isRegMatch(obj+1,ptn+2);
      }
    }
    else if (*ptn==SLASHES&&*(ptn+1)==PLUS){
      return isRegMatch(obj,ptn+1);
    }
    else if (*ptn==SLASHES&&*(ptn+1)==SLASHES&&*(ptn+2)==STAR){
      do {
        if (isRegMatch(obj,ptn+3)){
          return 1;
        }
      }while(*(obj++)==SLASHES);
    }
    else if (*ptn==SLASHES&&*(ptn+1)==SLASHES&&*(ptn+2)!=STAR){
      if (*obj==SLASHES){
        return isRegMatch(obj+1,ptn+2);
      }
    }
    else if (*ptn==SLASHES&&*(ptn+1)==STAR&&*(ptn+2)==STAR){
      do {
        if (isRegMatch(obj,ptn+3)){
          return 1;
        }
      }while(*(obj++)==STAR);
    }
    else if (*ptn==SLASHES&&*(ptn+1)==STAR&&*(ptn+2)!=STAR){
      if (*obj==STAR){
        return isRegMatch(obj+1,ptn+2);
      }
    }
    else if (*ptn==SLASHES&&*(ptn+1)==DOT&&*(ptn+2)==STAR){
      do {
        if (isRegMatch(obj,ptn+3)){
          return 1;
        }
      }while(*(obj++)==DOT);
    }
    else if (*ptn==SLASHES&&*(ptn+1)==DOT&&*(ptn+2)!=STAR){
      if (*obj==DOT){
        return isRegMatch(obj+1,ptn+2);
      }
    }
    else if (*(ptn+1)==STAR){
      do {
        if (isRegMatch(obj,ptn+2)){
          return 1;
        }
      }while(*(obj++)==*ptn);
    }
    else if (*(ptn+1)!=STAR){
      if (*obj==*ptn){
        return isRegMatch(obj+1,ptn+1);
      }
    }
  }
  return 0;
}
match *stringMatch(char *obj, char *ptn){
  match *mt=malloc(sizeof(match));
  int buf=5;
  mt->lstr=malloc(C_SIZE*buf);
  char *s=malloc(strlen(obj)+1);
  strcpy(s,obj);
  char *q=s;
  char ch;
  int flag,end,count=0,start=0;
  do{
    end=0;
    flag=0;
    for (int i=0;*q != NL;i++){
      ch=*(++q);
      *q=NL;
      if (isRegMatch(s+start,ptn)){
        end=i;
        flag=1;
      }
      *q=ch;
    }
    if (flag){
      char *tmp=malloc(end+2);
      for (int k=0;k<end+1;++k)
        tmp[k]=s[start+k];
      tmp[end+1]=NL;
      if (count+1>=buf){
        mt->lstr=realloc(mt->lstr,(buf+BUF_BLOCK)*C_SIZE);
        buf += BUF_BLOCK;
      }
      mt->lstr[count++]=tmp;
    }
    start += end+1;
    q=&s[start];
    if (*q==NL)
      break;
  }while(1);
  mt->l=count;
  mt->lstr[count]=NULL;
  free(s);
  return mt;
}
match_ptr *stringMatch_ptr(char *obj,char *ptn,int l){
  match_ptr *result=malloc(sizeof(match_ptr));
  char *s=malloc(strlen(obj)+1);
  strcpy(s,obj);
  char ch,*q=s,*st=s;
  int i;
  int record=0;
  do{
    for (i=1;*q!=NL;i++){
      ch=*(++q);
      *q=NL;
      if (isRegMatch(s,ptn))
        record=i;
      *q=ch;
    }
    if (record>=l){
      result->str=s-st+obj;
      result->len=record;
      free(st);
      return result;
    }
    q=++s;
  }while(*s != NL);
  return NULL;
}
char **splitLine(char *q){
  if (*q==NL)
    return NULL;
  int buf=BUF_BLOCK;
  char **args=malloc(buf*C_SIZE);
  int argnum=0;
  char *start;
  int l;
  int flag=0;// []
  int dq=0;// ""
  while (*q != NL){
    while ( IS_SPACE(*q) )
      q++;
    if (*q==NL)
      break;
    if (argnum+1>=buf){
      args=realloc(args,(buf+BUF_BLOCK)*C_SIZE);
      buf += BUF_BLOCK;
    }
    start = q;
    l=0;
    while ((flag||dq||!IS_SPACE(*q)) && *q!=NL){
      l++;
      if (*q==LP)
        flag++;
      else if (*q==RP)
        flag--;
      else if (*q==DQ&&*(q-1)!=SLASHES){
        dq=~dq;
      }
      q++;
    }
    args[argnum++]=newstr(start,l);
  }
  args[argnum]=NULL;
  return args;
}
char *add(char *a,char *b){
  int m=strlen(a);
  int n=strlen(b);
  int l=(m>n?m:n);
  char *new=malloc(l+2);
  sprintf(new,"%d",strToNum(a,m)+strToNum(b,n));
  return new;
}
char *minus(char *a,char *b){
  int m=strlen(a);
  int n=strlen(b);
  int l=(m>n?m:n);
  char *new=malloc(l+2);
  sprintf(new,"%d",strToNum(a,m)-strToNum(b,n));
  return new;
}
char *multiply(char *a,char *b){
  int m=strlen(a);
  int n=strlen(b);
  char *new=malloc(m+n+2);
  sprintf(new,"%d",strToNum(a,m)*strToNum(b,n));
  return new;
}
char *divide(char *a,char *b){
  int m=strlen(a);
  int n=strlen(b);
  int l=(m>n?m:n);
  char *new=malloc(l+2);
  sprintf(new,"%d",strToNum(a,m)/strToNum(b,n));
  return new;
}
char *module(char *a,char *b){
  int m=strlen(a);
  int n=strlen(b);
  int l=(m<n?m:n);
  char *new=malloc(l+2);
  sprintf(new,"%d",strToNum(a,m)%strToNum(b,n));
  return new;
}
typedef char* (*func)(char *,char *);
char *operand(char *a,char *b){
  char *s=b;
  char *k;
  int flag=0;
  func f;
  while (b>=a){
    if (flag==0){
      switch(*b){
        case PLUS:
          f=add;flag=1;k=b;break;
        case SUBTR:
          f=minus;flag=1;k=b;break;
        case STAR:
          f=multiply;flag=1;k=b;break;
        case DIVIDE:
          f=divide;flag=1;k=b;break;
        case MODULE:
          f=module;flag=1;k=b;break;
      }
    }
    else{
      switch(*b){
        case PLUS:
          return add(operand(a,b-1),ns_newstr(b+1,s-b));
        case SUBTR:
          return minus(operand(a,b-1),ns_newstr(b+1,s-b));
        case STAR:
          return multiply(operand(a,b-1),ns_newstr(b+1,s-b));
        case DIVIDE:
          return divide(operand(a,b-1),ns_newstr(b+1,s-b));
        case MODULE:
          return module(operand(a,b-1),ns_newstr(b+1,s-b));
        case SPACE:
          break;
        default:
          return f(operand(a,b),ns_newstr(k+1,s-k));
      }
    }
    b--;
  }
  return ns_newstr(a,s-a+1);
}
int condition(char *expr){
  char *p;
  char *a,*b;
  char *end=strchr(expr,RP);
  if ((p=strchr(expr,'<'))!=NULL && *(p+1)=='='){
    a=ns_newstr(expr+1,p-expr-1);
    b=operand(p+2,end-1);
    if (is_digit_str(a,strlen(a))&&is_digit_str(b,strlen(b))){
      if (atoi(a)-atoi(b)<=0)
        return 1;
      return 0;
    }
    else{
      if (strcmp(a,b)<=0)
        return 1;
      return 0;
    }
  }
  else if ((p=strchr(expr,'<'))!=NULL && *(p+1)!='='){
    a=ns_newstr(expr+1,p-expr-1);
    b=operand(p+1,end-1);
    if (is_digit_str(a,strlen(a))&&is_digit_str(b,strlen(b))){
      if (atoi(a)-atoi(b)<0)
        return 1;
      return 0;
    }
    else{
      if (strcmp(a,b)<0)
        return 1;
      return 0;
    }
  }
  else if ((p=strchr(expr,'>'))!=NULL && *(p+1)!='='){
    a=ns_newstr(expr+1,p-expr-1);
    b=operand(p+1,end-1);
    if (is_digit_str(a,strlen(a))&&is_digit_str(b,strlen(b))){
      if (atoi(a)-atoi(b)>0)
        return 1;
      return 0;
    }
    else{
      if (strcmp(a,b)>0)
        return 1;
      return 0;
    }
  }
  else if ((p=strchr(expr,'>'))!=NULL && *(p+1)=='='){
    a=ns_newstr(expr+1,p-expr-1);
    b=operand(p+2,end-1);
    if (is_digit_str(a,strlen(a))&&is_digit_str(b,strlen(b))){
      if (atoi(a)-atoi(b)>=0)
        return 1;
      return 0;
    }
    else{
      if (strcmp(a,b)>=0)
        return 1;
      return 0;
    }
  }
  else if ((p=strchr(expr,'='))!=NULL && *(p+1)=='='){
    a=ns_newstr(expr+1,p-expr-1);
    b=operand(p+2,end-1);
    if (is_digit_str(a,strlen(a))&&is_digit_str(b,strlen(b))){
      if (atoi(a)-atoi(b)==0)
        return 1;
      return 0;
    }
    else{
      if (strcmp(a,b)==0)
        return 1;
      return 0;
    }
  }
  return 0;
}
char *getLine(FILE *fp){
  char *new;
  int ch,n=0,buf=0,flag=1,ss=-1;
//  if ( PROMPT != NULL )
    printf("%s",PROMPT);
  while((ch=getc(fp)) != EOF){
    if (n+1>=buf)
      new=malloc(BUF_BLOCK);
    else
      new=realloc(new,buf+BUF_BLOCK);
    buf += BUF_BLOCK;
    if (ch=='\n')
      break;
    if (flag && IS_SPACE(ch))
      continue;
    else
      flag=0;
    if (! IS_SPACE(ch))
      ss=n;
    new[n++]=ch;
  }
  if (ch==EOF)
    return NULL;
  new[ss+1]=NL;
  return new;
}
char *head_ns_newstr(char *obj,int len){
  int k=0;
  while (IS_SPACE(*(obj+k))){
    k++;
  }
  char *new=malloc(len-k+1);
  strncpy(new,obj+k,len-k);
  new[len-k]=NL;
  return new;
}
//int main(void){
//  char *s="  \" jj l \"k kj jfdf";
//  char *pString[]={"python","java","c++","c#","javascript","ruby",NULL};
//  char **ps;
//  ps=copy_list_str(pString);
//  show_list_str(ps);
//  printf("<%s>\n",newstr(s,strlen(s)));
//  printf("<%s>\n",ns_newstr(s,strlen(s)));
//  printf("<%s>\n",nns_newstr(s,strlen(s)));
//  printf("<%s>\n",head_ns_newstr(s,strlen(s)));
//  printf("<%s>\n",delspace(s));
//  printf("<%d>\n",string_eq("kfc love","kfc",SPACE));
//  printf("<%d>\n",string_eq("kfc","kf",SPACE));
//  printf("<%d>\n",string_eq("kfc+ff","kfc+hh",PLUS));
//  printf("<%d>\n",is_digit_str("-123",strlen("-123")));
//  printf("<%d>\n",is_digit_str("- 123",strlen("- 123")));
//  printf("<%d>\n",is_digit_str("-",strlen("-")));
//  printf("<%d>\n",is_digit_str("5",strlen("5")));
//  printf("<%d>\n",is_digit_str("",strlen("")));
//  printf("<%d>\n",is_digit("+345"));
//  printf("<%s>+<%s>=<%s>\n","java","script",comStr("java","script"));
//  printf("<%s>+<%s>=<%s>\n","java","script",comStr_l("javakkkk","scriptffa",4,6));
//  printf("<%d>\n",strToNum("+345",strlen("+345")));
//  printf("<%d>\n",strToNum("-1180",strlen("-1180")));
//  printf("<%d>\n",strToi("-1180"));
//  freelist(ps);
//  ps=splitLine(" java   script  \" liga beauty +\"  [ c plus ] hh  ");
//  show_list_str(ps);
//  char *st=" -- 33 + 22 - -11 ";
//  char *end=st+strlen(st)-1;
//  puts(operand(st,end));
//  return 0;
//}
