#ifndef _estring__
#define _estring__
#define _GNU_SOURCE
#include <string.h>
#include <ctype.h>
char *strchrnul(const char *s,int c);

/*两个匹配结构体*/
typedef struct _match {
  int l;
  char **lstr;
} match;
typedef struct _match_ptr {
  int len;
  char *str;
} match_ptr;

/*提供以字符ch为分割单位的字符匹配
*例如：“liga=”<-->“liga=zjr” ch="="
*"export host=102.."<-->"export" ch=" "
*/
int string_eq(char *obj1,char *obj2,int ch);

/*生产一个指针obj开头,长度为len的字串*/
char *newstr(char *obj,int len);

/*和newstr功能类似,但会删除其中的空格,所以结果
长度<len.例如:"liga zjr love!"-->"ligazjrlove!"*/
char *ns_newstr(char *obj,int len);
/*不會刪除雙引號“”中的空格*/
char *nns_newstr(char *obj,int len);
/*創建一個新的字符串,但會刪除開頭的空格*/
char *head_ns_newstr(char *obj,int len);

/*和ns_newstr的区别在于不指定长度*/
char *delspace(char *obj);

/*打印出一个以NULL结尾的字符串*/
void show_list_str(char **lstr);

/*释放一个以NULL结尾的字符串的内存*/
void freelist(char **lstr);

/*复制一个以NULL结尾的字符串内容*/
char **copy_list_str(char **lstr);

/*拼接两个字符串*/
char *comStr(char *head,char *tail);

/*和comStr的区别在于指定拼接字串的长度*/
char *comStr_l(char *h,char *t,int i,int j);

/*判断字串obj的前len个字符是否为数字
"+521"是一个数字字串,"-a45"不是*/
int is_digit_str(char *obj,int len);

/*不指定长度*/
int is_digit(char *obj);

/*<==>strrchr*/
char *str_end_chr(char *expr,int ch);

/*将数字串转换成int*/
int strToNum(char *obj,int len);

/*同strToNum,但不指定长度*/
int strToi(char *obj);

/*使得[a-z_]这类匹配成为可能*/
char *expPatten(char *origin);

/*正则匹配*/
int isRegMatch(char *obj,char *ptn);

/*返回正则匹配所有结果*/
match *stringMatch(char *obj,char *ptn);

/*返回正则匹配指针和匹配长度,匹配长度>=l*/
match_ptr *stringMatch_ptr(char *obj,char *ptn,int l);

/*将字串按空格拆分,[ .. ]和"..."除外
例如:> <liga dancer good>-><liga>,<dancer>,<good>
<horse=[ zjr love liga ] hello world >
-> <horse=[ zjr love liga ]>,<hello>,<world>*/
/*會自動去除前後冗餘空格*/
char **splitLine(char *q);

/*operand*/
/* " 33 + 2 -10"-->"25" 对数字串做四则运算*/
/*start end 是字符串的處末指針*/
char *operand(char *start,char *end);

/*"[25<=20+6]"->0  "[30 > 13*2 ]"-->1 對條件進行常規判斷*/
/*如果不是純數字串，那麼就按ASCII碼比較大小*/
int condition(char *expr);// [不能落下]

/*讀取一行內容，並去除頭尾的空格*/
char *getLine(FILE *fp,int interactive);
#endif
