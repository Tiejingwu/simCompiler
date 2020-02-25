#include "scanner.h"
#define TOKEN_LEN 100
#pragma warning(disable: 4996)

unsigned int LineNo;//行号
static FILE *InFile;//输入文件流
static char TokenBuffer[TOKEN_LEN];//记号字符缓冲区
//初始化词法分析器
extern int InitScanner(const char *FileName)
{
	LineNo = 1;//行号
	InFile = fopen(FileName, "r");
	if (InFile != NULL) return 1;
	else return 0;
}

//关闭词法分析器
extern void CloseScanner(void)
{
	if (InFile != NULL)
		fclose(InFile);
}

//从源程序读字符
static char GetChar(void)
{
	char Char = getc(InFile);//逐个字符读
	return toupper(Char);
}

//将预读字符退回
static void BackChar(char Char)
{
	if (Char != EOF)
		ungetc(Char, InFile);
}

//加入字符到缓冲区
static void AddCharTokenString(char Char)
{
	int TokenLength = strlen(TokenBuffer);
	if (TokenLength + 1 >= sizeof(TokenBuffer)) return;
	TokenBuffer[TokenLength] = Char;
	TokenBuffer[TokenLength + 1] = '\0';
}

//清空缓冲区
static void EmptyTokenString()
{
	memset(TokenBuffer, 0, TOKEN_LEN);
}

//判断记号是否在记号表中
static Token JudgeKeyToken(const char *IDString)
{
	int loop;
	for (loop = 0; loop < sizeof(TokenTab) / sizeof(TokenTab[0]); loop++)//遍历符号表
	{	
		if (strcmp(TokenTab[loop].lexeme, IDString) == 0)
			return TokenTab[loop];
	}
	Token errortoken;
	memset(&errortoken, 0, sizeof(Token));//置空errortoken
	errortoken.type = ERRTOKEN;//填入出错记号
	return errortoken;
}

//获得一个记号
extern Token GetToken(void)
{
	Token token;
	char Char;
	memset(&token, 0, sizeof(Token));
	EmptyTokenString();
	token.lexeme = TokenBuffer;
	for (;;)
	{
		Char = GetChar();
		if (Char == EOF)//读出错
		{
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n')     //过滤空格、TAB、回车等分隔符，文件结束返回空记号
			LineNo++;
		if (!isspace(Char))
			break;
	}
	AddCharTokenString(Char);
	if (isalpha(Char))//英文字母
	{
		for (;;)
		{
			Char = GetChar();
			if (isalnum(Char))
				AddCharTokenString(Char);
			else break;
		}
		BackChar(Char);
		token = JudgeKeyToken(TokenBuffer);
		token.lexeme = TokenBuffer;
		return token;
	}
	else if (isdigit(Char))//数字
	{
		for (;;)
		{
			Char = GetChar();
			if (isdigit(Char))
				AddCharTokenString(Char);
			else break;
		}
		if (Char == '.')
		{
			AddCharTokenString(Char);
			for (;;)
			{
				Char = GetChar();
				if (isdigit(Char))
					AddCharTokenString(Char);
				else break;
			}
		}
		BackChar(Char);
		token.type = CONST_ID;
		token.value = atof(TokenBuffer);
		return token;
	}
	else//其他符号
	{
		switch (Char)
		{
		case ';': token.type = SEMICO; break;
		case '(': token.type = L_BRACKET; break;
		case ')': token.type = R_BRACKET; break;
		case ',': token.type = COMMA; break;
		case '+': token.type = PLUS; break;
		case '-':
			Char = GetChar();
			if (Char == '-')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else
			{
				BackChar(Char);
				token.type = MINUS;
				break;
			}
		case '/':
			Char = GetChar();
			if (Char == '/')
			{
				while (Char != '\n' && Char != EOF) Char = GetChar();
				BackChar(Char);
				return GetToken();
			}
			else
			{
				BackChar(Char);
				token.type = DIV;
				break;
			}
		case '*':
			Char = GetChar();
			if (Char == '*')
			{
				token.type = POWER;
				break;
			}
			else
			{
				BackChar(Char);
				token.type = MUL;
				break;
			}
		default:token.type = ERRTOKEN; break;
		}
	}
	return token;
}