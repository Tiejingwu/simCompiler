#include "scanner.h"
#define TOKEN_LEN 100
#pragma warning(disable: 4996)

unsigned int LineNo;//�к�
static FILE *InFile;//�����ļ���
static char TokenBuffer[TOKEN_LEN];//�Ǻ��ַ�������
//��ʼ���ʷ�������
extern int InitScanner(const char *FileName)
{
	LineNo = 1;//�к�
	InFile = fopen(FileName, "r");
	if (InFile != NULL) return 1;
	else return 0;
}

//�رմʷ�������
extern void CloseScanner(void)
{
	if (InFile != NULL)
		fclose(InFile);
}

//��Դ������ַ�
static char GetChar(void)
{
	char Char = getc(InFile);//����ַ���
	return toupper(Char);
}

//��Ԥ���ַ��˻�
static void BackChar(char Char)
{
	if (Char != EOF)
		ungetc(Char, InFile);
}

//�����ַ���������
static void AddCharTokenString(char Char)
{
	int TokenLength = strlen(TokenBuffer);
	if (TokenLength + 1 >= sizeof(TokenBuffer)) return;
	TokenBuffer[TokenLength] = Char;
	TokenBuffer[TokenLength + 1] = '\0';
}

//��ջ�����
static void EmptyTokenString()
{
	memset(TokenBuffer, 0, TOKEN_LEN);
}

//�жϼǺ��Ƿ��ڼǺű���
static Token JudgeKeyToken(const char *IDString)
{
	int loop;
	for (loop = 0; loop < sizeof(TokenTab) / sizeof(TokenTab[0]); loop++)//�������ű�
	{	
		if (strcmp(TokenTab[loop].lexeme, IDString) == 0)
			return TokenTab[loop];
	}
	Token errortoken;
	memset(&errortoken, 0, sizeof(Token));//�ÿ�errortoken
	errortoken.type = ERRTOKEN;//�������Ǻ�
	return errortoken;
}

//���һ���Ǻ�
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
		if (Char == EOF)//������
		{
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n')     //���˿ո�TAB���س��ȷָ������ļ��������ؿռǺ�
			LineNo++;
		if (!isspace(Char))
			break;
	}
	AddCharTokenString(Char);
	if (isalpha(Char))//Ӣ����ĸ
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
	else if (isdigit(Char))//����
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
	else//��������
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