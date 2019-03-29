#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "symbols.h"

#define MAX 10001

int getNextToken();
int line = 1;
char *pchrCurr;
int currLine = 1;

void err(const char *fmt, ...)
	{
		
		va_list va;
		va_start(va,fmt);
		fprintf(stderr,"error: ");
		vfprintf(stderr,fmt,va);
		fputc('\n',stderr);
		va_end(va);
		exit(-1);
		
	}
	#define SAFEALLOC(var,Type) if((var=(Type*)malloc(sizeof(Type)))==NULL)err("not enough memory");

	typedef struct _Token{
	int code; // codul (numele)
	union{
		char *text; // folosit pentru ID, CT_STRING (alocat dinamic)
		long int i; // folosit pentru CT_INT, CT_CHAR
		double r; // folosit pentru CT_REAL
		};
	int line; // linia din fisierul de intrare
	struct _Token *next; // inlantuire la urmatorul AL
	}Token;

Token *lastToken = NULL, *first_tokens =NULL;

Token *addTk(int code)
{
	Token *tk;
	SAFEALLOC(tk,Token)
	tk->code=code;
	tk->line=line;
	tk->next=NULL;
	if(lastToken)
	{
		lastToken->next=tk;
	}else
	{
		first_tokens=tk;
	}
	lastToken=tk;		
	return tk;
}

char *getTokenCode(int code)
{
	switch(code)
	{
		case END:
			return "END\0";
		case ID:
			return "ID\0";
		case CT_INT:
			return "CT_INT\0";
		case CT_REAL:
			return "CT_REAL\0";
		case CT_STRING:
			return "CT_STRING\0";
		case CT_CHAR:
			return "CT_CHAR\0";
		case ADD:
			return "ADD\0";
		case SUB:
			return "SUB\0";
		case DIV:
			return "DIV\0";
		case MUL:
			return "MUL\0";
		case DOT:
			return "DOT\0";
		case AND:
			return "AND\0";
		case OR:
			return "OR\0";
		case NOT:
			return "NOT\0";
		case ASSIGN:
			return "ASSIGN\0";
		case EQUAL:
			return "EQUAL\0";
		case NOTEQ:
			return "NOTEQ\0";
		case LESS:
			return "LESS\0";
		case LESSEQ:
			return "LESSEQ\0";
		case GREATER:
			return "GREATER\0";
		case GREATEREQ:
			return "GREATEREQ\0";
		case COMMA:
			return "COMMA\0";
		case SEMICOLON:
			return "SEMICOLON\0";
		case LPAR:
			return "LPAR\0";
		case RPAR:
			return "RPAR\0";
		case LBRACKET:
			return "LBRACKET\0";
		case RBRACKET:
			return "RBRACKET\0";
		case LACC:
			return "LACC\0";
		case RACC:
			return "RACC\0";
		case BREAK:
			return "BREAK\0";
		case CHAR:
			return "CHAR\0";
		case DOUBLE:
			return "DOUBLE\0";
		case ELSE:
			return "ELSE\0";
		case FOR:
			return "FOR\0";
		case IF:
			return "IF\0";
		case INT:
			return "INT\0";
		case RETURN:
			return "RETURN\0";
		case STRUCT:
			return "STRUCT\0";
		case VOID:
			return "VOID\0";
		case WHILE:
			return "WHILE\0";
		
		default: return "Invalid code value!";
			//break;
	}
			
}

void displayTokens()
{
	Token *currToken = first_tokens;
	char *tokName = (char*)malloc(20);
	while(currToken != NULL)
	{
		strcpy(tokName, getTokenCode(currToken->code));
		printf("%s ", tokName);
		switch(currToken->code)
		{
			case CT_INT:
				printf(":%ld", currToken->i);
				break;
			case CT_CHAR:
				printf(":%c", currToken->i);
				break;
			case CT_STRING:
				printf(":%s", currToken->text);
				break;
			case CT_REAL:
				printf(":%g", currToken->r);
				break;
			case ID:
				printf(":%s", currToken->text);
				break;
			default:
				break;
		}
		printf("\n");
		currToken=currToken->next;
	}
}
void tkerr(const Token *tk,const char *fmt,...)
{	
	va_list va;
	va_start(va,fmt);
	fprintf(stderr,"error in line %d: ",tk->line);
	vfprintf(stderr,fmt,va);
	fputc('\n',stderr);
	va_end(va);
	exit(-1);
}

char* getRidOfSpecialSymbols(char *str)
{
	int len = 0;
	char *retString = (char*)malloc(len);
	retString[len] = 0;

	for(int i = 0; i < strlen(str); i++)
	{
		len++;
		retString = (char*)realloc(retString, len+1);
		if(str[i] == '\\')
		{
			switch(str[i+1])
			{
				case 'a':
					retString[len-1] = '\a';
					break;

				case 'b':
					retString[len-1] = '\b';
					break;
				case 'f':
					retString[len-1] = '\f';
					break;
				case 'n':
					retString[len-1] = '\n';
					break;
				case 'r':
					retString[len-1] = '\r';
					break;

				case 't':
					retString[len-1] = '\t';
					break;
				case 'v':
					retString[len-1] = '\v';
					break;
				case '\\':
					retString[len-1] = '\\';
					break;
				case '"':
					retString[len-1] = '\"';
					break;

				case '\'':
					retString[len-1] = '\'';
					break;
				case '?':
					retString[len-1] = '\?';
					break;
				case '0':
					retString[len-1] = '\0';
					break;
				default:printf("\nERROR:Wrong symbol!");
			}
			i++;
		}
		else retString[len-1] = str[i];

		retString[len] = 0; 
	}

	return retString;
}

char* createString(char *start, char *end, char *str)
{
	unsigned int len = end - start;
	char *retString = (char*)malloc(len + 1);
	retString[0] = 0;
	unsigned short index = 0;
	while(index < len)
	{
		retString[index] = start[index];
		index++;
		//printf("%d %c\n", retString[index-1], retString[index-1]);
	}
	retString[index] = 0;

	strcpy(retString, getRidOfSpecialSymbols(retString));

	return retString;
} 

void play(char *str)
{
	char *ptr = str;
	printf("%s\n", (ptr));
	ptr++;
	printf("%c\n",ptr[0]);
}

int main(int argc, char **argv)
{
	
	char *text = (char*)malloc(MAX);

	int fd = open(argv[1], O_RDONLY);
	//int fd = open("tests/test.c", O_RDONLY);

	if(fd == -1)
	{
		printf("\nNo %s file!\n", argv[1]);
		exit(-1);
	}

	if(read(fd, text, MAX-1) == -1)
	{
		perror("\nCould not read file!\n");
		exit(-2);
	}

	printf("%s\n\n", text);
	pchrCurr = text;

	while(getNextToken()!=END);

	close(fd);


	displayTokens();


	return 0;
}



int getNextToken()
{
	char ch, *ptrStart, prevChar;
	int state = 0;
	int lenChar;
	int n;
	int isHexFlag = 0;
	Token *tk;
	line = currLine;
	for(;;)
	{
		ch = *pchrCurr;
		//printf("#state=%d ch=%c ch=(%d)\n",state, ch,ch);
		switch(state)
		{
			case 0:
				
					if(isalpha(ch)|| ch =='_')
					{
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 1;
					}
					else if(ch == '\'')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 13;
					}
					else if(ch == '\'')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 13;
					}
					else if(ch == '\"')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 16;
					}
					else if(ch == '[')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 28;
					}
					else if(ch == ']')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 29;
					}
					else if(ch >= '1' && ch <= '9')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 2;
					}
					else if(ch == '0')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 5;
					}
					else if(ch == '!')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 10;
					}
					else if(ch == '/')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 18;
					}
					else if(ch == ' ' || ch == '\r' || ch == '\t')
					{	
						pchrCurr++;
						//state = 18;
					}
					else if(ch == '\n')
					{
						pchrCurr++;
						currLine++;
					}
					else if(ch == '}')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 41;
					}
					else if(ch == '{')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 42;
					}
					else if(ch == '<')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 22;
					}
					else if(ch == '>')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 23;
					}
					else if(ch == '=')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 24;
					}
					else if(ch == '+')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 45;
					}
					else if(ch == '-')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 46;
					}
					else if(ch == '*')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 47;
					}
					else if(ch == ',')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 35;
					}
					else if(ch == ';')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 36;
					}
					else if(ch == '.')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 48;
					}
					else if(ch == '&')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 25;
					}
					else if(ch == '|')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 26;
					}
					else if(ch == '(')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 51;
					}
					else if(ch == ')')
					{	
						ptrStart = pchrCurr;
						pchrCurr++;
						state = 52;
					}
					else if(ch == '\0')
					return END;
					else {
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
						}
					break;
				
			case 1:
					if(ch =='_' || isalnum(ch))
					{
						pchrCurr++;
					}
					else state = 27;
					break;
			case 2:
					if(ch >= '0' && ch <= '9')
					{
						pchrCurr++;
					}
					else if(ch == '.')
					{
						pchrCurr++;
						state = 3;
					}
					else if(ch == 'e' || ch == 'E')
					{
						pchrCurr++;
						state = 6;
					}
					else state = 30;
					break;
			case 3:
					if(ch >= '0' && ch <= '9')
					{
						pchrCurr++;
						state = 4;
					}
					else 
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 4:
					if(ch >= '0' && ch <= '9')
					{
						pchrCurr++;
					}
					else if(ch == 'e' || ch == 'E')
					{
						pchrCurr++;
						state = 6;
					}
					else state = 33;
					break;
			case 5:
					if(ch >= '0' && ch <= '7')
					{
						pchrCurr++;
						state = 7;
					}
					else if(ch == '.')
					{
						pchrCurr++;
						state = 3;
					}
					else if(ch == 'e' || ch == 'E')
					{
						pchrCurr++;
						state = 6;
					}
					else if(ch == 'x')
					{
						isHexFlag = 1;
						pchrCurr++;
						state = 11;
					}
					else if(ch == '8' || ch == '9')
					{
						pchrCurr++;
						state = 55;
					}
					else state = 30;
					break;
			case 6:
					if(ch >= '0' && ch <= '9')
					{
						pchrCurr++;
						state = 8;
					}
					else if(ch == '-' || ch == '+')
					{
						pchrCurr++;
						state = 9;
					}
					else 
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 7:
					if(ch >= '0' && ch <= '7')
					{
						pchrCurr++;
					}
					else if(ch == '8' || ch == '9')
					{
						pchrCurr++;
						state = 55;
					}
					else
					{
						pchrCurr++;
						state = 30;
					}
					break;
			case 8:
					if(ch >= '0' && ch <= '9')
					{
						pchrCurr++;
					}
					else
					{
						//pchrCurr++;
						state = 33;
					}
					break;
			case 9:
					if(ch >= '0' && ch <= '9')
					{
						pchrCurr++;
						state = 8;
					}
					else 
					{
						perror("\nINVALID CHARACTER AFTER +/- !\n");
						exit(-1);
					}
					break;
			case 10:
					if(ch == '=')
					{
						pchrCurr++;
						state = 32;
					}
					else 
					{
						pchrCurr++;
						state = 31;
					}
					break;
			case 11:
					if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) 
					{
						pchrCurr++;
						state = 12;
					}
					else 
					{
						perror("\nINVALID CHARACTER IN BASE 16! CH BETWEEN 0-F!\n");
						exit(-1);
					}
					break;
			case 12:
					if((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) 
					{
						pchrCurr++;
					}
					else 
					{
						//pchrCurr++;
						state = 30;
					}
					break;
			case 13:
					if(ch == '\\')
					{
						pchrCurr++;
						state = 14;
					}
					else if(ch == '\'')
					{
					 	perror("\nYOU CAN'T HAVE AN EMPTY CHARACTER\n");
						exit(-1);
					}
					else 
					{
						prevChar = pchrCurr[0];
						pchrCurr++;
						state = 15;
					}
					break;
			case 14:
					// if(ch == 'a' || ch == 'n' ||ch == 'f' ||ch == 'r' ||ch == 't' ||ch == 'v' ||ch == '\'' ||ch == '\"' ||ch == '?' ||ch == '\\' ||ch == '0')
					// {

					// 	pchrCurr++;
					// 	state = 15;
					// }
					// else perror("\nINVALID SPECIAL CHARACTER!\n");
					switch(ch)
					{
						case 'a':
					prevChar = '\a';
					break;

					case 'b':
						prevChar = '\b';
						break;
					case 'f':
						prevChar = '\f';
						break;
					case 'n':
						prevChar = '\n';
						break;
					case 'r':
						prevChar = '\r';
						break;

					case 't':
						prevChar = '\t';
						break;
					case 'v':
						prevChar = '\v';
						break;
					case '\\':
						prevChar = '\\';
						break;
					case '"':
						prevChar = '\"';
						break;

					case '\'':
						prevChar = '\'';
						break;
					case '?':
						prevChar = '\?';
						break;
					case '0':
						prevChar = '\0';
						break;
					default:prevChar = ch;
					}
					pchrCurr++;
					state = 15;
					break;
			case 15:
					if(ch == '\'')
					{
						pchrCurr++;
						state = 34;
					}
					else 
					{
						perror("\nTo many characters!\n");
						exit(-1);
					}
					break;
			case 16:
					if(ch == '\\')
					{
						pchrCurr++;
						state = 17;
					}
					else if(ch == '\"')
					{
						//pchrCurr++;
						state = 37;
					}
					else
					{
						pchrCurr++;
					}
					break;
			case 17:
					if(ch == 'a' || ch == 'n' ||ch == 'f' ||ch == 'r' ||ch == 't' ||ch == 'v' ||ch == '\'' ||ch == '\"' ||ch == '?' ||ch == '\\' ||ch == '0')
					{
						pchrCurr++;
						state = 16;
					}
					else 
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 18:
					if(ch == '*')
					{
						pchrCurr++;
						state = 20;
					}
					else if(ch == '/')
					{
						pchrCurr++;
						state = 19;	
					}
					else 
					{
						//pchrCurr++;
						state = 38;
					}
					break;
			case 19:
					if(ch != '\n' && ch != '\r' && ch != '\0')
					{
						pchrCurr++;
					}
					else
					{
						pchrCurr++;
						state = 0;
					}
					break;
			case 20:
					if(ch != '*')
					{
						pchrCurr++;
					}
					else if(ch == '*')
					{
						pchrCurr++;
						state = 21;
					}
					else
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					
					break;
			case 21:
					if(ch == '*')
					{
						pchrCurr++;
					}
					else if(ch != '/')
					{
						pchrCurr++;
						state = 20;
					}
					else if(ch == '/')
					{
						pchrCurr++;
						state = 0;
					}
					else
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 22:
					if(ch == '=')
					{
						pchrCurr++;
						state = 40;
					}
					else
					{
						//pchrCurr++;
						state = 39;
					}
					break;
			case 23:
					if(ch == '=')
					{
						pchrCurr++;
						state = 43;
					}
					else
					{
						//pchrCurr++;
						state = 54;
					}
					break;
			case 24:
					if(ch == '=')
					{
						pchrCurr++;
						state = 44;
					}
					else
					{
						//pchrCurr++;
						state = 53;
					}
					break;
			case 25:
					if(ch == '&')
					{
						pchrCurr++;
						state = 49;
					}
					else
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 26:
					if(ch == '|')
					{
						pchrCurr++;
						state = 50;
					}
					else
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 27:
					n = pchrCurr - ptrStart;
					if(n == 5 && (!memcmp(ptrStart,"break",5)))
					{
						tk = addTk(BREAK);
					}
					else if (n == 5 && (!memcmp(ptrStart,"while",5))) {
						tk = addTk(WHILE);
					}	
					else if (n == 4 && (!memcmp(ptrStart,"char",4))) {
						tk = addTk(CHAR);
					}
					else if (n == 6 && (!memcmp(ptrStart,"double",n))) {
						tk = addTk(DOUBLE);
					}
					else if (n == 4 && (!memcmp(ptrStart,"else",n))) {
						tk = addTk(ELSE);
					}
					else if (n == 3 && (!memcmp(ptrStart,"for",n))) {
						tk = addTk(FOR);
					}
					else if (n == 2 && (!memcmp(ptrStart,"if",n))) {
						tk = addTk(IF);
					}
					else if (n == 3 && (!memcmp(ptrStart,"int",n))) {
						tk = addTk(INT);
					}
					else if (n == 6 && (!memcmp(ptrStart,"return",n))) {
						tk = addTk(RETURN);
					}
					else if (n == 6 && (!memcmp(ptrStart,"struct",n))) {
						tk = addTk(STRUCT);
					}
					else if (n == 4 && (!memcmp(ptrStart,"void",n))) {
						tk = addTk(VOID);
					}
					else if (n == 5 && (!memcmp(ptrStart,"while",n))) {
						tk = addTk(WHILE);
					}
					else
					{
						tk = addTk(ID);
						tk->text = (char*)malloc(pchrCurr - ptrStart + 1);
						strcpy(tk->text,createString(ptrStart, pchrCurr, pchrCurr)); 
					}					
					return ID;
					break;
			case 28:
					tk = addTk(LBRACKET);

					return LBRACKET;
					break;
			case 29:
					addTk(RBRACKET);
					return RBRACKET;
					break;
			case 30:
					tk = addTk(CT_INT);
					if(isHexFlag == 0) tk->i = atoi(createString(ptrStart, pchrCurr, pchrCurr));
					else	tk->i=(int)strtol(createString(ptrStart+2, pchrCurr, pchrCurr), NULL, 16);
					return CT_INT;
					break;
			case 31:
					addTk(NOT);
					return NOT;
					break;
			case 32:
					addTk(NOTEQ);
					return NOTEQ;
					break;
			case 33:
					//pchrCurr--;
					tk = addTk(CT_REAL);
					tk->r = atof(createString(ptrStart, pchrCurr, pchrCurr));
					return CT_REAL;
					break;
			case 34:
					tk = addTk(CT_CHAR);
					tk->i = prevChar;
					return CT_CHAR;
					break;
			case 35:
					addTk(COMMA);
					return COMMA;
					break;
			case 36:
					addTk(SEMICOLON);
					return SEMICOLON;
					break;
			case 37:
					tk = addTk(CT_STRING);
					tk->text = (char*)malloc(pchrCurr - ptrStart + 1);
					strcpy(tk->text,createString(ptrStart+1, pchrCurr, pchrCurr));
					pchrCurr++;
					return CT_STRING;
					break;
					
			case 38:
					addTk(DIV);
					return DIV;
					break;
			case 39:
					addTk(LESS);
					return LESS;
					break;
			case 40:
					addTk(LESSEQ);
					return LESSEQ;
					break;
			case 41:
					addTk(RACC);
					return RACC;
					break;
			case 42:
					addTk(LACC);
					return LACC;
					break;	
			case 43:
					addTk(GREATEREQ);
					return GREATEREQ;
					break;
			case 44:
					addTk(EQUAL);
					return EQUAL;
					break;
			case 45:
					addTk(ADD);
					return ADD;
					break;
			case 46:
					addTk(SUB);
					return SUB;
					break;
			case 47:
					addTk(MUL);
					return MUL;
					break;
			case 48:
					addTk(DOT);
					return DOT;
					break;	
			case 49:
					addTk(AND);
					return AND;
					break;
			case 50:
					addTk(OR);
					return OR;
					break;
			case 51:
					addTk(LPAR);
					return LPAR;
					break;
			case 52:
					addTk(RPAR);
					return RPAR;
					break;
			case 53:
					addTk(ASSIGN);
					return ASSIGN;
					break;	
			case 54:
					addTk(GREATER);
					return GREATER;
					break;
			case 55:
					if(ch == '.')
					{
						pchrCurr++;
						state = 3;
					}
					else if (ch == 'e' || ch == 'E') 
					{
						pchrCurr++;
						state = 6;
					}
					else if (ch >= '0' && ch <= '9') 
					{
						pchrCurr++;
					}
					else 
					{
						perror("\nINVALID CHARACTER!\n");
						exit(-1);
					}
					break;
			case 56:
					addTk(END);
					return END;
					break;	
		}
	}
}
