#include<stdio.h>
#include<stdlib.h>
extern void Parser(char *SrcFilePtr);//����������
void main(int argc,char *argv[])
{
	Parser(argv[1]);//����parser�����﷨���������б����Ե������test.txt��
	system("pause");

}
//ͨ�����ĵ�Parser()�����������ı�Ҫ��ȡ���ļ�����yufa>parsertestĿ¼�¿ɿ���0-5 6��txt�ļ���