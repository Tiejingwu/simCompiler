#ifndef PARSER_H
#define PARSER_H

#include"scanner.h"
typedef double(*FuncPtr)(double);
struct ExprNode                                             //�﷨���ڵ�����
{
	enum Token_Type OpCode;                                
	union
	{
		struct { ExprNode *Left, *Right; }CaseOperater;     //��Ԫ����
		struct { ExprNode *Child; FuncPtr MathFuncPtr; }CaseFunc;//��������
		double CaseConst;                                   //����
		double *CaseParmPtr;                                //����T
	}Content;
};

extern void Parser(char *SrcFilePtr);                       //����ӿ�

#endif 
