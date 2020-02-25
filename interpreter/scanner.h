#ifndef SCANNER_H
#define SCANNER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

enum Token_Type//记号类别
{
	ORIGIN, SCALE, ROT, IS, TO, STEP, DRAW, FOR, FROM,//保留字
	T,//参数
	SEMICO, L_BRACKET, R_BRACKET, COMMA,//分层符号
	PLUS, MINUS, MUL, DIV, POWER,//运算符
	FUNC,//函数
	CONST_ID,//常数
	NONTOKEN,//空记号，标记源程序文件的结束
	ERRTOKEN//出错记号，标记非法输入
};

typedef double(*MathFuncPtr)(double);//MathFuncPtr代表一个指向返回double值并带有一个double形参的函数的指针的类型

struct Token
{
	Token_Type type;//记号类别
	char *lexeme;//属性，字符串
	double value;//属性，常数的值
	double(*FuncPtr)(double);//属性，函数指针
};

static Token TokenTab[] = //符号表
{
	{ CONST_ID,	"PI", 3.1415926,	NULL },
	{ CONST_ID,	"E",	2.71828,	NULL },
	{ T,		"T",		0.0,	NULL },
	{ FUNC,		"SIN",		0.0,	sin },
	{ FUNC,		"COS",		0.0,	cos },
	{ FUNC,		"TAN",		0.0,	tan },
	{ FUNC,		"LN",		0.0,	log },
	{ FUNC,		"EXP",		0.0,	exp },
	{ FUNC,		"SQRT",		0.0,	sqrt },
	{ ORIGIN,	"ORIGIN",	0.0,	NULL },
	{ SCALE,	"SCALE",	0.0,	NULL },
	{ ROT,		"ROT",		0.0,	NULL },
	{ IS,		"IS",		0.0,	NULL },
	{ FOR,		"FOR",		0.0,	NULL },
	{ FROM,		"FROM",		0.0,	NULL },
	{ TO,		"TO",		0.0,	NULL },
	{ STEP,		"STEP",		0.0,	NULL },
	{ DRAW,		"DRAW",		0.0,	NULL },
};

extern unsigned int LineNo;//源文件行号
extern int InitScanner(const char *);
extern Token GetToken(void);//获取记号函数
extern void CloseScanner(void);

#endif