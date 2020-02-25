/*#include "scanner.h"

void main(int argc, char *argv[])
{
	Token token;
	if (argc < 2)
	{
		printf("please input Source File !\n"); return ;
	}
	if (!InitScanner(argv[1]))
	{
		printf("Open Source File Error!\n"); return ;
	}
	printf("记号类型    字符串     常数值     函数指针\n");
	printf("------------------------------------------\n");
	while (1)
	{
		token = GetToken();
		if (token.type != NONTOKEN)
			printf("%4d,%12s,%12f,%12x\n", token.type, token.lexeme, token.value, token.FuncPtr);
		else break;
	}
	printf("------------------------------------------\n");
	CloseScanner();
}*/
