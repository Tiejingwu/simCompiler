#include<windows.h>
#include<wingdi.h>
#include"parser.h"

extern HDC hDC;

#define red RGB(255,0,0)
#define black RGB(0,0,0)
//�ⲿ��������
extern void DrawPixel(unsigned long x, unsigned long y);//����
extern double GetExprValue(struct ExprNode * root);//��ñ��ʽ��ֵ
extern void DrawLoop(double Start,
	double End,
	double Step,
	struct ExprNode *HorPtr,
	struct ExprNode *VerPtr);//ͼ�λ���
extern void DelExprTree(struct ExprNode *root);//ɾ��һ����