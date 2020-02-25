#include<stdio.h>
#include<stdlib.h>
extern void Parser(char *SrcFilePtr);//测试主程序
void main(int argc,char *argv[])
{
	Parser(argv[1]);//调用parser进行语法分析，其中被测试的语句在test.txt中
	system("pause");

}
//通过更改的Parser()函数参数来改变要读取的文件（在yufa>parsertest目录下可看到0-5 6个txt文件）