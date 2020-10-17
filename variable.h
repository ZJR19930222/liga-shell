#ifndef _variable__
#define _variable__
extern char **environ;
typedef struct _var {
  char *str;
  int global;
} var;
#ifndef _Var__
extern var varTable[];
extern int varnum;
#endif
/*将一个赋值表达式子name=var添加到变量表varTable中去*/
/*name=是删除变量,name是更新global值，默认是-1不影响*/
int putVar(char *expr,int Bool);

/*查看变量表varTable,存在返回其value,否则返回NULL*/
char *getVar(char *expr);

void delVar(char *expr);
void showVar(void);
/*将值添加到环境变量当中去,add_env(&environ,"name=value")*/
int add_env(char ***s,char *q);

/*将varTable中的global=1的项更新到环境变量中去*/
void varToEnv(char ***s);

char *substVar(char *a);
#endif
