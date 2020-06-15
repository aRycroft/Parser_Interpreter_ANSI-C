#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include "NAL.h"

#ifndef TESTING
int main(int argc, char* argv[]){
	FILE *stream;
	Program* p;
	#ifdef INTERP
	mvm* m;
	int i;:wq
	Program* prevP[MAXPROGS];
	#endif
	if(argc != 2){
		fprintf(stderr, "Provide file name\n");
		exit(EXIT_FAILURE);
	}
	stream = getStream(argv[1]);
	#ifdef INTERP
	/*Loop to NULL array of pointers to programs, used for freeing mallocs later*/
	for(i = 0; i < MAXPROGS; i++){
		prevP[i] = NULL;
	}
	m = mvm_init();
	p = program_init(m, prevP);
	p->prevP[0] = p;
	srand(time(NULL));
	#endif	
	#ifndef INTERP
	p = program_init_parse();
	#endif
	tok(p, stream);
	fclose(stream);
	prog(p);
	#ifdef INTERP
	mvm_free(&m);
	#endif
	#ifndef INTERP
	printf("PARSED OK\n");
	#endif
	return 0;
}
#endif
#ifdef TESTING
int main(void){
	fprintf(stderr, "|-------TESTING STARTED--------|\n");
	basicTest();
	fileTest();
	fprintf(stderr, "|-------TESTING FINISHED-------|\n");
	return 0;
}

void basicTest(void){
	char rotTest[] = "7848";
	char rotTest2[] = "UryyB!";
	char quoteTest[] = "\"Hello\"";
	char quoteTest2[] = "#12345#";

	assert(var("%VAR"));
	assert(var("$VAR"));
	assert(!var("%var"));
	assert(!var("%"));

	assert(con("\"CONST\""));
	assert(con("251.234"));
	assert(!con("CONST"));
	assert(!con("$A"));

	assert(strVar("$STRVAR"));
	assert(!strVar("%NUMVAR"));
	assert(numVar("%NUMVAR"));
	assert(!numVar("$STRVAR"));

	assert(strCon("\"constant string\""));
	assert(strCon("#2618#"));
	assert(!strCon("const str  \""));

	assert(numCon("52154"));
	assert(!numCon("a25125"));

	rot18(rotTest);
	rot18(rotTest2);

	assert(strcmp(rotTest, "2393") == 0);
	assert(strcmp(rotTest2, "HellO!") == 0);

	removeQuotes(quoteTest);
	removeQuotes(quoteTest2);

	assert(strcmp(quoteTest, "Hello") == 0);
	assert(strcmp(quoteTest2, "12345") == 0);

	assert(myStrCmp("#URY#", "HEL") == 0);
	assert(myStrCmp("\"AHHH\"", "AHHH") == 0);
	assert(myStrCmp("\"AHH\"", "AHHH") != 0);
}

void fileTest(void){
	#ifdef INTERP
	char testNames[50][40];
	FILE *stream;
	int i;
	char testline[MAXTOKENS];
	mvm* m;
	Program* p;
	Program* prevP[MAXPROGS];
	for(i = 0; i < MAXPROGS; i++){
		prevP[i] = NULL;
	}
	m = mvm_init();
	p = program_init(m, prevP);
	#endif
	#ifndef INTERP
	assert(checkFileParse("test1.nal", "Parsed OK"));
	assert(checkFileParse("test2.nal", "Parsed OK"));
	assert(checkFileParse("test3.nal", "Parsed OK"));
	assert(checkFileParse("test4.nal", "Parsed OK"));
	assert(checkFileParse("test5.nal", "Parsed OK"));
	assert(checkFileParse("test6.nal", "Parsed OK"));

	assert(checkFileParse("testing/fileParse1.nal", "Parsed OK"));
	assert(checkFileParse("testing/inputParse1.nal", "Parsed OK"));
	assert(checkFileParse("testing/jumpParse.nal", "Parsed OK"));
	assert(checkFileParse("testing/printParse1.nal", "Parsed OK"));
	assert(checkFileParse("testing/printParse3.nal", "Parsed OK"));
	assert(checkFileParse("testing/randParse1.nal", "Parsed OK"));
	assert(checkFileParse("testing/ifParse1.nal", "Parsed OK"));
	assert(checkFileParse("testing/setParse1.nal", "Parsed OK"));

	/*If parse errors occur the output won't just be Parsed OK*/
	assert(!checkFileParse("testing/fileParse2.nal", "Parsed OK"));
	assert(!checkFileParse("testing/inputParse2.nal", "Parsed OK"));
	assert(!checkFileParse("testing/printParse2.nal", "Parsed OK"));
	assert(!checkFileParse("testing/randParse2.nal", "Parsed OK"));
	assert(!checkFileParse("testing/ifParse2.nal", "Parsed OK"));
	assert(!checkFileParse("testing/setParse2.nal", "Parsed OK"));
	#endif
	#ifdef INTERP
	strcpy(testNames[0], "testing/fileInterp.nal");
	strcpy(testNames[1], "testing/inputInterp.nal");
	strcpy(testNames[2], "testing/randInterp.nal");
	strcpy(testNames[3], "testing/jumpInterp.nal");
	strcpy(testNames[4], "testing/printInterp.nal");
	strcpy(testNames[5], "testing/ifInterp1.nal");
	strcpy(testNames[6], "testing/extraInterp1.nal");

	/*All output from running the test files is sent to testdump.txt*/
	stream = freopen("testing/testdump.txt", "w", stdout);
	if(stream == NULL){
		fprintf(stderr, "Cannot open testdump file\n");
		exit(2);
	}
	for(i = 0; i < 7; i++){
		newFile(p, testNames[i]);
	}
	fclose(stream);
	stream = getStream("testing/testdump.txt");
	i = 0;
	/*INPUT TEST*/
	assert(strcmp("\"Hello\"", mvm_search(m, "$A")) == 0);
	assert(strcmp("123", mvm_search(m, "%C")) == 0);	
	assert(atoi(mvm_search(m, "%NUM")) < 100);
	assert(atoi(mvm_search(m, "%NUM")) > 0);
	assert(atoi(mvm_search(m, "%INUM")) == atoi(mvm_search(m, "%NUM")) + 1);
	while(fgets(testline, MAXTOKENS, (FILE*)stream) != NULL){
		/*Check each line of testdump file*/
		/*FILE TEST*/	
		if(i == 0){
			assert(strcmp(testline, "In File\n") == 0);
		}
		if(i == 1){
			assert(strcmp(testline, "Out of File\n") == 0);
		}
		/*INPUT TEST*/
		if(i == 2){
			assert(strcmp(testline, "Equal vars\n") == 0);
		}
		if(i == 3){
			assert(strcmp(testline, "Equal rot5\n") == 0);
		}
		if(i == 4){
			assert(strcmp(testline, "Equal rot13\n") == 0);
		}
		/*JUMP TEST*/
		if(i == 5){
			assert(strcmp(testline, "Jump works\n") == 0);
		}
		/*PRINT TEST*/
		if(i == 6){
			assert(strcmp(testline, "HelloWorld!\n") == 0);
		}
		if(i == 7){
			assert(strcmp(testline, "TEST\n") == 0);
		}
		if(i == 8){
			assert(strcmp(testline, "1234\n") == 0);
		}
		if(i == 9){
			assert(strcmp(testline, "Hello!\n") == 0);
		}
		if(i == 10){
			assert(strcmp(testline, "678\n") == 0);	
		}
		if(i == 11){
			assert(strcmp(testline, "11\n") == 0);	
		}
		if(i == 12){
			assert(strcmp(testline, "12\n") == 0);	
		}
		if(i == 13){
			assert(strcmp(testline, "INC, Variable not defined ") == 0);	
		}
	i++;
	}
	fclose(stream);
	mvm_free(&m);
	free_program(&p);
	#endif
	/*Remove testdumpfile after testing*/
	if(remove("testing/testdump.txt") != 0){
		ON_FAIL("Failed to delete testdump file");
	}
}

Bool checkFileParse(char* file, char* expectedOut){
	FILE *stream;
	char testline[MAXTOKENS];

	stream = freopen("testing/testdump.txt", "w", stdout);

	if(stream == NULL){
		fprintf(stderr, "Cannot open testdump file\n");
		exit(2);
	}

	fprintf(stdout, "%s", "Parsed OK");
	/*If file fails parse test, errors will be output into testdump file*/
	newFile(NULL, file);
	fclose(stream);

	stream = getStream("testing/testdump.txt");
	if(fgets(testline, MAXTOKENS, stream) != NULL){
		if(strcmp(testline, expectedOut) == 0){
			fclose(stream);
			return true;
		}
	}
	fclose(stream);
	return false;
}

#endif
FILE* getStream(char* file){
	FILE* stream;
	stream = fopen(file, "r");
	if(stream == NULL){
		#ifdef TESTING
		fprintf(stderr, "Can't find testing file %s\n", file);
		exit(2);
		#endif
		ON_FAIL("Can't find file");
	}
	return stream;
}


#if defined INTERP || TESTING
void newFile(__attribute__((unused))Program *p, char *file){	
	FILE *stream;
	Program* newP;
	#ifdef INTERP
	newP = program_init(p->vars, p->prevP);
	#endif
	#ifndef INTERP
	/*Cast to void to stop compiler warning about unused variable*/
	newP = program_init_parse();
	#endif
	stream = getStream(file);
	tok(newP, stream);
	fclose(stream);
	#ifndef TESTING
	free(file);
	#endif
	prog(newP);
}
#endif

void tok(Program *p, FILE *stream){
	/*wFlag set at start of word*/
	/*sFlag set at start of double quotes with spaces*/
	int i, j;
	Bool wFlag, sFlag;
	char c;
	Token* start;
	String* str;
	str = string_init();
	start = p->toks;
	i = j = 0;
	sFlag = wFlag = false;
	while((c = getc(stream)) != EOF){
		if(isHashorQuote(c)){
			if(sFlag){
				addChar(str, j, c);
			}
			sFlag = !sFlag;
		}
		if(!isSpace(c) || sFlag){
			if(isRegChar(c) || isBracket(c) || isHashorQuote(c)){
				wFlag = 1;
			}
			addChar(str, j++, c);
		}	
		else if(wFlag){
			addChar(str, j, '\0');
			j = 0;
			addWord(p->toks, str->arr);
			i++;
			wFlag = 0;
			free_str(&str);
			str = string_init();
		}
	}
	free_str(&str);
	p->toks = start;
	p->numwds = i;
}

void prog(Program *p){
	/*Head has empty string so skip 1*/
	SKIP(1);
	if(strcmp("{", CHECK(0)) != 0){
		ON_FAIL("EXPECTED {");
	}	
	SKIP(1);
	instr(p);
}

void instr(Program *p){
	if(strcmp("}", CHECK(0)) == 0){
		/*FREE PROGRAM*/
		if (p->toks->next == NULL){
			free_program(&p);
			return;
		}
		#ifdef INTERP
		return;
		#endif
	}
	instruct(p);
	SKIP(1);
	instr(p);
}

void instruct(Program *p){
	/*FILE*/
	if(strcmp("FILE", CHECK(0)) == 0){
		file(p);
		return;
	}
	/*INPUT*/
	if(strcmp("IN2STR", CHECK(0)) == 0){
		int2str(p);
		return; 
	}
	if(strcmp("INNUM", CHECK(0)) == 0){
		innum(p);
		return;
	}
	/*ABORT*/
	if(strcmp("ABORT", CHECK(0)) == 0){
		#ifdef INTERP
		myAbort(p);
		#endif
		return;
	}
	/*IFCON*/
	if(strcmp("IFEQUAL", CHECK(0)) == 0){
		ifequal(p);
		return;
	}
	if(strcmp("IFGREATER", CHECK(0)) == 0){
		ifgreater(p);
		return;
	}
	/*PRINT*/
	if(strcmp("PRINT", CHECK(0)) == 0){
		print(p, true);
		return;
	}
	/*PRINTN*/
	if(strcmp("PRINTN", CHECK(0)) == 0){
		print(p, false);
		return;
	}
	/*SET*/
	if(strVar(CHECK(0)) || numVar(CHECK(0))){
		set(p);
		return;
	}
	/*JUMP*/
	if(strcmp("JUMP", CHECK(0)) == 0){
		jump(p);
		return;
	}
	/*RND*/
	if(strcmp("RND", CHECK(0)) == 0){
		rnd(p);
		return;
	}
	/*INC*/
	if(strcmp("INC", CHECK(0)) == 0){
		inc(p);
		return;
	}
	
	#ifndef TESTING
	/* '}' is the only char that can be ignored after all instructions*/
	if(strcmp("}", CHECK(0)) != 0){
		printf("%s\n", CHECK(0));
		ON_FAIL("Unexpected character");
	}
	#endif
}

void file(Program *p){
	#ifdef INTERP
	char *file;
	#endif
	if(!strCon(CHECK(1))){
		ON_FAIL("In Func: FILE, Expected file name");
	}
	#ifdef INTERP
	file = (char*)malloc(sizeof(char) * strlen(CHECK(1)) + 1);
	checkMalloc(file);
	strcpy(file, CHECK(1));
	removeQuotes(file);
	newFile(p, file);
	#endif
	SKIP(1);
}

void int2str(Program *p){
	#ifdef INTERP
	char str1[DEFSTR], str2[DEFSTR];
	#endif
	if(strcmp("(", CHECK(1)) != 0){
		ON_FAIL("In Func: INT2STR, Expected opening '('");
	}
	if(!strVar(CHECK(2))){
		ON_FAIL("In Func: INT2STR, Expected strvar after '('");
	}
	if(strcmp(",", CHECK(3)) != 0){
		ON_FAIL("In Func: INT2STR, Expected ','");
	}
	if(!strVar(CHECK(4))){
		ON_FAIL("In Func: INT2STR, Expected strvar after ','");
	}
	if(strcmp(")", CHECK(5)) != 0){
		ON_FAIL("In Func: INT2STR, Expected closing ')'");
	}
	/*Check if var is already filled*/
	#ifdef INTERP
	if(scanf("%100s", str1) == 1){
		if(mvm_search(p->vars, CHECK(2)) != NULL){
			strcpy(mvm_search(p->vars, CHECK(2)), str1);
		}
		else{
			mvm_insert(p->vars, CHECK(2), str1);
		}
	}

	if(scanf("%100s", str2) == 1){
		if(mvm_search(p->vars, CHECK(4)) != NULL){
			strcpy(mvm_search(p->vars, CHECK(4)), str2);
		}
		else{
			mvm_insert(p->vars, CHECK(4), str2);
		}
	}
	#endif
	SKIP(5);
}

void innum(Program *p){
	#ifdef INTERP
	char num[DEFSTR];
	#endif
	if(strcmp("(", CHECK(1)) != 0){
		ON_FAIL("In Func: INNUM, Expected opening '('");
	}
	if(!numVar(CHECK(2))){
		ON_FAIL("In Func: INNUM, Expected number variable");
	}
	if(strcmp(")", CHECK(3)) != 0){
		ON_FAIL("In Func: INNUM, Expected closing ')'");
	}
	/*CHECK IF VAR ALREADY FILLED*/
	#ifdef INTERP
	if(scanf("%19s", num) == 1){
		if(mvm_search(p->vars, CHECK(2)) != NULL){
			strcpy(mvm_search(p->vars, CHECK(2)), num);
		}
		else{
			mvm_insert(p->vars, CHECK(2), num);
		}
	}
	#endif
	SKIP(3);
}
#ifdef INTERP
void myAbort(Program *p){
	int i, j;
	Program* progs[MAXPROGS];
	i = j = 0;
	mvm_free(&p->vars);
	while(p->prevP[i] != NULL){
		progs[i] = p->prevP[i];
		i++;
	}
	while(j < i){
		free_program(&progs[j++]);
	}
	exit(EXIT_SUCCESS);
}
#endif
void ifequal(Program *p){
	#ifdef INTERP
	Bool equal;
	#endif
	ifstatementParse(p);
	#ifdef INTERP
	equal = ifstatementInterp(p, CHECK(2), CHECK(4), 0);
	callFunctionifTrue(p, equal);
	#endif
	#ifndef INTERP
	SKIP(6);
	#endif
}

void ifgreater(Program *p){
	#ifdef INTERP
	Bool greater;
	#endif
	ifstatementParse(p);
	#ifdef INTERP
	greater = ifstatementInterp(p, CHECK(2), CHECK(4), 1);
	callFunctionifTrue(p, greater);
	#endif
	#ifndef INTERP
	SKIP(6);
	#endif
}

void ifstatementParse(Program *p){
	if(strcmp("(", CHECK(1)) != 0){
		ON_FAIL("In Func: IFGREATER or IFEQUAL, Expected '(' before variables");
	}
	varcon(CHECK(2));
	if(strcmp(",", CHECK(3)) != 0){
		ON_FAIL("In Func: IFGREATER or IFEQUAL, Expected ',' between variables");
	}
	varcon(CHECK(4));
	if(strcmp(")", CHECK(5)) != 0){
		ON_FAIL("In Func: IFGREATER or IFEQUAL, Expected ')' after variables");
	}
	if(strcmp("{", CHECK(6)) != 0){
		ON_FAIL("In Func: IFGREATER or IFEQUAL, Expected '{' after if statement");
		}
	unmatchedVars(CHECK(2), CHECK(4));
}

#ifdef INTERP
Bool ifstatementInterp(Program *p, char* in1, char* in2, int type){
	/*True == Greater, False == Equal*/
	char *var1, *var2;
	Bool result;

	if(var(in1)){
		var1 = searchVars(p->vars, in1);
	}
	else{
		var1 = in1;
	}

	if(var(in2)){
		var2 = searchVars(p->vars, in2);
	}
	else{
		var2 = in2;
	}
	result = ifStateCalc(p, var1, var2, type);
	return result;
}

Bool ifStateCalc(Program *p, char* var1, char* var2, int type){
	/*If either variable is undefined just return false*/
	if(var1 == NULL || var2 == NULL){
		return false;
	}
	if(type){
		if(strVar(CHECK(2))){
			if(myStrCmp(var1, var2) > 0){
				return true;
			}
		}
		else{
			if(atol(var1) > atol(var2)){
				return true;
			}
		}
		return false;
	}
	else{
		if(strVar(CHECK(2))){
			if(myStrCmp(var1, var2) == 0){
				return true;
			}
		}
		else{
			if(atol(var1) == atol(var2)){
				return true;
			}
		}
		return false;
	}
	return false;
}

void callFunctionifTrue(Program *p, Bool isTrue){
	if(isTrue){
		SKIP(7);
		instr(p);	
	}
	else{
		SKIP(7);
		skipFunction(p);
	}
}
#endif

void print(Program *p, __attribute__((unused)) Bool newLine){
	#ifdef INTERP
	char *str, *printStr;
	#endif
	varcon(CHECK(1));
	#ifdef INTERP
	if(con(CHECK(1))){	
		str = CHECK(1);
	}
	else{
		str = mvm_search(p->vars, CHECK(1));
	}
	
	if(str == NULL){
		SKIP(1);
		return;
	}

	printStr = (char*)malloc(sizeof(char) * strlen(str) + 1);
	checkMalloc(printStr);
	strcpy(printStr, str);

	if(printStr != NULL){
		if(printStr[0] == '#'){
			rot18(printStr);
			removeQuotes(printStr);
		}
		else{
			removeQuotes(printStr);
		}
		if(newLine){
			printf("%s\n", printStr);
		}
		else{
			printf("%s", printStr);
		}
	}
	free(printStr);
	#endif
	SKIP(1);
}

void set(Program *p){
	if(!var(CHECK(0))){
		ON_FAIL("In Func: SET, Expected variable name");
	}
	if(strcmp(CHECK(1), "=") != 0){
		ON_FAIL("In Func: SET, Expected '=' after variable");
	}
	varcon(CHECK(2));
	unmatchedVars(CHECK(0), CHECK(2));
	#ifdef INTERP
	addToVars(p->vars, CHECK(0), CHECK(2));
	#endif
	SKIP(2);
}

void jump(Program *p){
	#ifdef INTERP
	int jumpPos;
	#endif
	if(!numCon(CHECK(1))){
		ON_FAIL("In Func: JUMP, Expected number constant");
	}
	SKIP(1);
	#ifdef INTERP
	jumpPos = atoi(CHECK(0));
	p->toks = p->head;
	SKIP(jumpPos);
	#endif
}

void rnd(Program *p){
	#ifdef INTERP
	char var[4];
	#endif
	if(strcmp(CHECK(1), "(") != 0){
		ON_FAIL("In Func: RND, Expected opening '('");
	}
	if(!numVar(CHECK(2))){	
		ON_FAIL("In Func: RND, Expected number variable");
	}
	if(strcmp(CHECK(3), ")") != 0){
		ON_FAIL("In Func: INC, Expected closing ')'");
	}
	#ifdef INTERP
	sprintf(var, "%d", (rand() % 99));
	if(mvm_search(p->vars, CHECK(2)) != NULL){
		strcpy(mvm_search(p->vars, CHECK(2)), var);
	}
	else{
		mvm_insert(p->vars, CHECK(2), var);
	}
	#endif
	SKIP(3);
}

void inc(Program *p){
	#ifdef INTERP
	char *var;
	int intVar;
	#endif
	if(strcmp(CHECK(1), "(") != 0){
		ON_FAIL("In Func: INC, Expected opening '('");
	}
	if(!numVar(CHECK(2))){	
		ON_FAIL("In Func: INC, Expected number variable");
	}
	if(strcmp(CHECK(3), ")") != 0){
		ON_FAIL("In Func: INC, Expected closing ')'");
	}
	#ifdef INTERP
	var = mvm_search(p->vars, CHECK(2));

	if(var == NULL){
		ON_FAIL("INC, Variable not defined");
	}
	else{
		intVar = atol(var);
		intVar++;
		sprintf(var, "%d", intVar);
	}
	#endif	
	SKIP(3);
}

void varcon(char *str){
	if(var(str) + con(str) == 0){
		ON_FAIL("Expected variable or constant");
	}
}

Bool var(char *str){
	if(strlen(str) <= 1){
		return false;
	}
	if (strVar(str) || numVar(str)){
		return true;
	}
	return false;
}

Bool con(char *str){
	if(strlen(str) == 0){
		return false;
	}
	if(strCon(str) || numCon(str)){
		return true;
	}
	return false;
}

Bool strVar(char *str){
	unsigned i;
	if(str[0] != '$'){
		return false;
	}
	for(i = 1; i < strlen(str); i++){ 
		if(!isupper(str[i])){	
			return false;
		}
	}
	return true;
}

Bool numVar(char *str){
	unsigned i;
	if(str[0] != '%'){
		return false;
	}
	for(i = 1; i < strlen(str); i++){
		if(!isupper(str[i])){	
			return false;
		}
	}
	return true;
}

Bool strCon(char *str){
	if(!isString(str)){
		return false;
	}
	return true;
}

Bool numCon(char *str){
	unsigned i;
	for(i = 0; i < strlen(str); i++){
		if(!isdigit(str[i]) && str[i] != '.'){
			return false;
		}
	}
	return true;		
}

Bool isRegChar(char c){
	if(isalnum(c) || c == ',' || c == '='){
		return true;
	}
	return false;
}

Bool isHashorQuote(char c){
	if(c == '"' || c == '#'){
		return true;
	}
	return false;
}

Bool isBracket(char c){
	if(c == '{' || c == '}' || c == '(' || c == ')'){
		return true;
	}
	return false;
}

Bool isSpace(char c){
	if(c == ' ' || c == '\n' || c == '\r' || c == '\t'){
		return true;
	}
	return false;
}

Bool isString(char *c){
	if(c == NULL){
		return false;
	}
	if(c[0] == '"' && c[strlen(c) - 1] == '"'){
		return true;
	}
	if(c[0] == '#' && c[strlen(c) - 1] == '#'){
		return true;
	}
	return false;
}

void unmatchedVars(char *var1, char *var2){
	if((strVar(var1) && numCon(var2)) || (numVar(var1) && strCon(var2))){
		ON_FAIL("Unmatched var types in code i.e. ($A = %B)");
	}
	if((strVar(var1) && numVar(var2)) || (numVar(var1) && strVar(var2))){
		ON_FAIL("Unmatched var types in code i.e. ($A = %B)");
	}
}

void removeQuotes(char *str){
	unsigned i;
	if(isString(str)){
		for (i = 0; i < strlen(str); i++){
			str[i] = str[i + 1];
		}
		str[i - 2] = '\0';
	}
	
}

void rot18(char *str){
	unsigned i;
	int rot;
	char c, offset;
	if(str == NULL){
		return;
	}
	if(strlen(str) == 0){
		return;
	}
	for(i = 0; i < strlen(str); i++){
		c = str[i];
		if(isdigit(c)){
			offset = '0';
			rot = 5;
			c -= offset;
			c += rot;
			c = c % 10;
			c += offset;
		}
		else{
			if(isRegChar(c)){
				rot = 13;
				if(isupper(c)){
					offset = 'A';			
				}
				else{
					offset = 'a';
				}
				c -= offset;
				c += rot;
				c = c % 26;
				c += offset;
			}
		}
		str[i] = c;
	}
}

char* searchVars(mvm *vars, char *cw){
	char *search, *ret;
	search = (char*)malloc(sizeof(char) * strlen(cw) + 1);
	checkMalloc(search);
	strcpy(search, cw);
	removeQuotes(search);
	ret = mvm_search(vars, search);
	free(search);
	return ret;
}

void addToVars(mvm *vars, char* var1, char* var2){
	if(mvm_search(vars, var1) != NULL){
		if(var(var2)){
			strcpy(mvm_search(vars, var1), searchVars(vars, var2));
		}
		else{	
			strcpy(mvm_search(vars, var1), var2);
		}
	}
		else{
		if(var(var2)){
			mvm_insert(vars, var1, searchVars(vars, var2));
		}
		else{
			mvm_insert(vars, var1, var2);
		}
	}
}

int myStrCmp(char *str1, char *str2){
	int result;
	char *var1, *var2;
	
	var1 = (char*)malloc(sizeof(char) * strlen(str1) + 1);
	var2 = (char*)malloc(sizeof(char) * strlen(str2) + 1);

	checkMalloc(var1);
	checkMalloc(var2);
	
	strcpy(var1, str1);
	strcpy(var2, str2);

	if(var1[0] == '#'){
		rot18(var1);
	}
	if(var2[0] == '#'){
		rot18(var2);
	}

	removeQuotes(var1);
	removeQuotes(var2);

	result = strcmp(var1, var2);

	free(var1);
	free(var2);

	return result;
}

void checkMalloc(void *p){
	if(p == NULL){
		ON_FAIL("Malloc Failed");
	}
}

void addWord(Token* t, char* str) {
	Token* next;
	next = tok_init(str);
	while (t->next != NULL) {
		t = t->next;
	}
	t->next = next;
}

void printTok(Program* p) {
	Token* start;
	p->cw = 0;
	start = p->toks;
	p->toks = p->toks->next;
	while (p->toks != NULL) {
		fprintf(stderr, "[");
		fprintf(stderr, "%s", p->toks->str->arr);
		fprintf(stderr, "]");
		p->toks = p->toks->next;
	}
	p->toks = start;
}

Token* skipWords(Program* p, int n) {
	int i;
	Token* t = p->toks;
	for (i = 0; i < n; i++) {
		if (t->next == NULL) {
			fprintf(stderr, "Unexpected end of file\n");
			free_program(&p);
			exit(0);
		}
		t = t->next;
	}
	return t;
}

void skipFunction(Program *p){
	int endCount = 0;
	while(strcmp(CHECK(0), "}") != 0 || endCount != 0){
		if(strcmp(CHECK(0), "{") == 0){
			endCount++;
		}
		if(strcmp(CHECK(0), "}") == 0){
			endCount--;
		}
		SKIP(1);
	}
}

char* checkWord(Program* p, int n) {
	int i;
	Token* t = p->toks;
	for (i = 0; i < n; i++) {
		if (t->next == NULL) {
			fprintf(stderr, "Unexpected end of file\n");
			free_program(&p);
			exit(0);
		}
		t = t->next;
	}
	return t->str->arr;
}
void cpyStr(Token* t, char* source) {
	int len;
	len = strlen(source);
	if (len >= t->str->sz) {
		t->str->arr = (char*)realloc(t->str->arr, sizeof(char) * (len + 1));
	}
	strcpy(t->str->arr, source);
}

void addChar(String* str, int n, char c) {
	if (n >= str->sz) {
		str->arr = (char*)realloc(str->arr, sizeof(char) * (n + 1));
		str->sz = n + 1;
	}
	str->arr[n] = c;
}

Token* tok_init(char* str) {
	Token* t;
	t = (Token*)malloc(sizeof(Token));
	checkMalloc(t);
	t->str = string_init();
	if (str != NULL) {
		cpyStr(t, str);
	}
	t->next = NULL;
	return t;
}

String* string_init(void) {
	String* str;
	str = (String*)malloc(sizeof(String));
	checkMalloc(str);
	str->arr = (char*)malloc(DEFSTR * sizeof(char));
	checkMalloc(str->arr);
	str->sz = 0;
	return str;
}

#if defined INTERP || TESTING
Program* program_init(mvm* vars, Program* prevP[MAXPROGS]) {
	Program* newProg;
	int i = 0;
	newProg = (Program*)malloc(sizeof(Program));
	checkMalloc(newProg);
	newProg->cw = 0;
	newProg->numwds = 0;
	newProg->vars = vars;
	newProg->toks = tok_init("");
	newProg->head = newProg->toks;
	memcpy(newProg->prevP, prevP, sizeof(Program*) * 10);
	while (prevP[i] != NULL) {
		i++;
	}
	newProg->prevP[i] = newProg;
	return newProg;
}
#endif

Program* program_init_parse(void) {
	Program* newProg;
	newProg = (Program*)malloc(sizeof(Program));
	checkMalloc(newProg);
	newProg->cw = 0;
	newProg->numwds = 0;
	newProg->toks = tok_init("");
	newProg->head = newProg->toks;
	return newProg;
}

void free_program(Program **p){
	Program *temp;
	temp = *p;
	free_words(temp->head);
	free(temp);
	*p = NULL;
}

void free_words(Token* t){	
	Token* temp;
	while(t != NULL){
		temp = t;
		t = t->next;
		free_str(&temp->str);
		free(temp);
	}
}
void free_str(String** str){	
	String *temp;
	temp = *str;
	free(temp->arr);
	free(temp);
	*str = NULL;
}
