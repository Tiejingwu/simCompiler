#include"parser.h"
#include"semantic.h"
#pragma warning(disable: 4996)
#pragma warning(disable: 4703)
double Parameter = 0,
Origin_x = 0, Origin_y = 0,
Scale_x = 1, Scale_y = 1,
Rot_angle = 0;

static Token token;

//辅助函数声明
static void FetchToken();
static void MatchToken(enum Token_Type The_Token);
static void SyntaxError(int case_of);//理处语法错误的子程序，SyntaxError（1）词法错   SyntaxError（2）语法错
static void ErrMsg(unsigned LineNo, char *descrip, char *string);
static void PrintSyntaxTree(struct ExprNode *root, int indent);//先序遍历打印语法树
static void Program();//程序
static void Statement();//语句
static void OriginStatement();
static void RotStatement();
static void ScaleStatement();
static void ForStatement();//返回值为指向语法树节点的指针的函数。
static struct ExprNode *Expression();//表达式、二元加减
static struct ExprNode *Term();//乘除
static struct ExprNode *Factor();//一元加减
static struct ExprNode *Component();
static struct ExprNode *Atom();

							  
extern void Parser(char *SrcFilePtr);

//语法树构造
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

//获得记号
static void FetchToken()
{
	token = GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1); //语法错误
}

//匹配当前记号
static void MatchToken(enum Token_Type The_Token)
{
	if (token.type != The_Token)
	{
		SyntaxError(2);
	}
	FetchToken();
}

//语法错误处理
static void SyntaxError(int case_of)
{
	switch (case_of)
	{
	case 1: ErrMsg(LineNo, "错误记号 ", token.lexeme);
		break;
	case 2: ErrMsg(LineNo, "不是预期记号", token.lexeme);
		break;
	}
}

//打印错误信息
void ErrMsg(unsigned LineNo, char *descrip, char *string)
{
	char msg[256];
	memset(msg, 0, 256);
	sprintf(msg, "Line No %5d:%s %s !", LineNo, descrip, string);

	MessageBox(NULL, msg, "error!", MB_OK);

	CloseScanner();
	exit(1);
}

//先序遍历打印语法树
void PrintSyntaxTree(struct ExprNode *root, int indent)
{
	int temp;
	for (temp = 1; temp <= indent; temp++) printf("\t");
	switch (root->OpCode)
	{
	case PLUS:		printf("%s\n", "+");								break;
	case MINUS:		printf("%s\n", "-");								break;
	case MUL:		printf("%s\n", "*");								break;
	case DIV:		printf("%s\n", "/");								break;
	case POWER:		printf("%s\n", "**");								break;
	case FUNC:		printf("%x\n", root->Content.CaseFunc.MathFuncPtr);	break;
	case CONST_ID:	printf("%f\n", root->Content.CaseConst);			break;
	case T:			printf("%s\n", "T");								break;
	default:		printf("Error Tree Node !\n");						exit(0);
	}
	if (root->OpCode == CONST_ID || root->OpCode == T) //叶子节点
		return;//常数：右值；参数：左值地址
	if (root->OpCode == FUNC)//递归打印一个孩子节点
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);//函数有孩子节点和指向函数名的指针
	else
	{
		PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
		PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
	}
}

//绘图语言解释器
void Parser(char *SrcFilePtr)
{
	if (!InitScanner(SrcFilePtr))
	{
		printf("Open Source File Error !\n"); return;
	}
	FetchToken();//获得记号
	Program();//递归下降分析
	CloseScanner();
	return;
}

//程序Program的递归子程序
static void Program()
{
	while (token.type != NONTOKEN)
	{
		Statement();
		MatchToken(SEMICO);
	}
}

//Statment的递归子程序
static void Statement()
{
	switch (token.type)
	{
	case ORIGIN: OriginStatement(); break;
	case SCALE: ScaleStatement(); break;
	case ROT:  RotStatement(); break;
	case FOR: ForStatement(); break;
	default: SyntaxError(2); 
	}
}

//OriginStatement的递归子程序
static void OriginStatement(void)
{
	struct ExprNode *tmp;//语法树节点的类型
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();

	Origin_x = GetExprValue(tmp);//获取横坐标点平移距离
	DelExprTree(tmp);//删除一棵树

	MatchToken(COMMA);

	tmp = Expression();

	Origin_y = GetExprValue(tmp);//获取纵坐标点平移距离
	DelExprTree(tmp);//删除一棵树

	MatchToken(R_BRACKET);
}

//ScaleStatement的递归子程序
static void ScaleStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();

	Scale_x = GetExprValue(tmp);//获取横坐标的比例因子
	DelExprTree(tmp);

	MatchToken(COMMA);

	tmp = Expression();

	Scale_y = GetExprValue(tmp);//获取纵坐标的比例因子
	DelExprTree(tmp);

	MatchToken(R_BRACKET);
}

//RotStatement的递归子程序
static void RotStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(ROT);
	MatchToken(IS);

	tmp = Expression();

	Rot_angle = GetExprValue(tmp);//旋转角度
	DelExprTree(tmp);
}

//ForStatement的递归子程序
static void ForStatement(void)
{
	double Start, End, Step;//起点、终点、步长
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;

	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);

	start_ptr = Expression(); //获得起点表达式的语法树

	Start = GetExprValue(start_ptr);//计算起点的值
	DelExprTree(start_ptr);//释放起点语法树空间

	MatchToken(TO);

	end_ptr = Expression();//构造终点语法树

	End = GetExprValue(end_ptr);//计算终点
	DelExprTree(end_ptr);

	MatchToken(STEP);

	step_ptr = Expression();//构造步长语法树

	Step = GetExprValue(step_ptr);//计算步长
	DelExprTree(step_ptr);

	MatchToken(DRAW);
	MatchToken(L_BRACKET);

	x_ptr = Expression();

	MatchToken(COMMA);

	y_ptr = Expression();

	MatchToken(R_BRACKET);

	DrawLoop(Start, End, Step, x_ptr, y_ptr); //绘制图形
	DelExprTree(x_ptr);
	DelExprTree(y_ptr);
}

//Expression的递归子程序
static struct ExprNode *Expression()
{
	struct ExprNode *left, *right;
	Token_Type token_tmp;

	left = Term();
	while (token.type == PLUS || token.type == MINUS)
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Term();
		left = MakeExprNode(token_tmp, left, right);//构造运算左子树
	}
	return left;//返回当前记号节点
}

//Term的递归子程序
static struct ExprNode *Term()
{
	struct ExprNode *left, *right;
	Token_Type token_tmp;
	left = Factor();
	while (token.type == MUL || token.type == DIV)
	{
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Factor();
		left = MakeExprNode(token_tmp, left, right);
	}
	return left;
}

//Factor的递归子程序
static struct ExprNode *Factor()
{
	struct ExprNode *left, *right;
	if (token.type == PLUS)
	{
		MatchToken(PLUS);
		right = Factor();
	}
	else if (token.type == MINUS)
	{
		MatchToken(MINUS);
		right = Factor();
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = Component();
	return right;
}

//Component的递归子程序
static struct ExprNode *Component()
{
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER)
	{
		MatchToken(POWER);
		right = Component();//递归调用Component以实现POWER的右结合
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//Atom的递归子程序
static struct ExprNode *Atom()
{
	struct Token t = token;
	struct ExprNode *address, *tmp;
	switch (token.type)
	{
	case CONST_ID:
		MatchToken(CONST_ID);
		address = MakeExprNode(CONST_ID, t.value);
		break;
	case T:
		MatchToken(T);
		address = MakeExprNode(T);
		break;
	case FUNC:
		MatchToken(FUNC);
		MatchToken(L_BRACKET);
		tmp = Expression();
		address = MakeExprNode(FUNC, t.FuncPtr, tmp);
		MatchToken(R_BRACKET);
		break;
	case L_BRACKET:
		MatchToken(L_BRACKET);
		address = Expression();
		MatchToken(R_BRACKET);
		break;
	default:
		SyntaxError(2);
	}
	return address;
}

//生成一个节点，接收可变参数列表
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...)
{
	struct ExprNode *ExprPtr = new(struct ExprNode);
	ExprPtr->OpCode = opcode;
	va_list ArgPtr;
	va_start(ArgPtr, opcode);//初始化va_list变量
	switch (opcode)//构造节点
	{
	case CONST_ID://常数
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
		break;
	case T://参数T
		ExprPtr->Content.CaseParmPtr = &Parameter;
		break;
	case FUNC://函数调用
		ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
		ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	default://二元运算
		ExprPtr->Content.CaseOperater.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		ExprPtr->Content.CaseOperater.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	}
	va_end(ArgPtr);

	return ExprPtr;
}