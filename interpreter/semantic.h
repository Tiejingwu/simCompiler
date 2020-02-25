#include<windows.h>
#include<wingdi.h>
#include"parser.h"

extern HDC hDC;

#define red RGB(255,0,0)
#define black RGB(0,0,0)
//外部函数声明
extern void DrawPixel(unsigned long x, unsigned long y);//画点
extern double GetExprValue(struct ExprNode * root);//获得表达式的值
extern void DrawLoop(double Start,
	double End,
	double Step,
	struct ExprNode *HorPtr,
	struct ExprNode *VerPtr);//图形绘制
extern void DelExprTree(struct ExprNode *root);//删除一棵树