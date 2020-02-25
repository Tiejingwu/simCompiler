#ifndef PARSER_H
#define PARSER_H

#include"scanner.h"
typedef double(*FuncPtr)(double);
struct ExprNode                                             //语法树节点类型
{
	enum Token_Type OpCode;                                
	union
	{
		struct { ExprNode *Left, *Right; }CaseOperater;     //二元运算
		struct { ExprNode *Child; FuncPtr MathFuncPtr; }CaseFunc;//函数调用
		double CaseConst;                                   //常数
		double *CaseParmPtr;                                //参数T
	}Content;
};

extern void Parser(char *SrcFilePtr);                       //对外接口

#endif 
