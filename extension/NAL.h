#include <stdio.h>
#include "mvm.h"

#define MAXPROGS 50
#define DEFSTR 100
#define SKIP(N) p->toks = skipWords(p, N)
#define CHECK(N) checkWord(p, N)
#define ON_FAIL(STR) {fprintf(stderr, "%s, ERROR OCCURED in %s, on line %d\n", STR, __FILE__, __LINE__); exit(0);}
#ifdef TESTING
#undef ON_FAIL
#define ON_FAIL(STR) {fprintf(stdout, "%s \n", STR);}
#endif

typedef enum {false, true} Bool;

typedef enum {LT, GT, LE, GE} Op;

struct string{
	char* arr;
	int sz;
};
typedef struct string String;

struct token{
	String* str;
	struct token *next;
};
typedef struct token Token;

struct prog{
	Token* toks;
	Token* head;
	int cw;
	int numwds;
	#if defined INTERP || TESTING
	struct prog* prevP[MAXPROGS];
	mvm* vars;
	#endif
}; 
typedef struct prog Program;

void basicTest(void);
void fileTest(void);
Bool checkFileParse(char* file, char* expectedOut);

FILE* getStream(char* file);
void newFile(Program *p, char *file);
void tok(Program *p, FILE *stream);

void prog(Program *p);
void instr(Program *p);
void instruct(Program *p);
void file(Program *p);
void myAbort(Program *p);
void ifequal(Program *p);
void ifgreater(Program *p);
void ifstatementParse(Program *p);
Bool ifstatementInterp(Program *p, char* in1, char* in2, int type);
Bool ifStateCalc(Program *p, char* str1, char* str2, int type);
void callFunctionifTrue(Program *p, Bool isTrue);
void print(Program *p, Bool newLine);
void set(Program *p);
void jump(Program *p);
void rnd(Program *p);
void inc(Program *p, Bool up);
void mywhile(Program *p);
void loopFunc(Program* p, char* var1, char* var2, Op operator, Bool fLoop);
Bool checkLoopCond(Program *p, char* var1, char* var2, Op operator);
void myFor(Program *p);
void forLoop(Program *p, char* var1, char* var2, char* sVar1, char* sVar2, Op operator);
void whileLoop(Program *p, char* var1, char* var2, char* sVar1, char* sVar2, Op operator);
Op getOp(char* operator);

void varcon(char *str);
Bool var(char *str);
Bool con(char *str);
Bool strVar(char *str);
Bool numVar(char *str);
Bool strCon(char *str);
Bool numCon(char *str);
void int2str(Program *p);
void innum(Program *p);
Bool isRegChar(char c);
Bool isHashorQuote(char c);
Bool isBracket(char c);
Bool isSpace(char c);
Bool isString(char *str);
Bool isEquals(char c);
void unmatchedVars(char *var1, char *var2);
void removeQuotes(char *str);
void rot18(char *str);
char* searchVars(mvm *vars, char *cw);
void addToVars(mvm *vars, char* var1, char* var2);
int myStrCmp(char *str1, char *str2);
void checkMalloc(void *p);

void addWord(Token* t, char* str);
void printTok(Program *p);
Token* skipWords(Program *p, int n);
void skipFunction(Program *p);
char* checkWord(Program *p, int n);
void cpyStr(Token* t, char* source);
void addChar(String* str, int n, char c);

Token* tok_init(char* str);
String* string_init(void);
Program* program_init(mvm *vars, Program* prevP[MAXPROGS]);
Program* program_init_parse(void);

void free_program(Program **p);
void free_words(Token* t);
void free_str(String** str);
