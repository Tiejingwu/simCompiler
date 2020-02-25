#include"parser.h"
#include"semantic.h"
#pragma warning(disable: 4996)
#pragma warning(disable: 4703)
double Parameter = 0,
Origin_x = 0, Origin_y = 0,
Scale_x = 1, Scale_y = 1,
Rot_angle = 0;

static Token token;

//������������
static void FetchToken();
static void MatchToken(enum Token_Type The_Token);
static void SyntaxError(int case_of);//���﷨������ӳ���SyntaxError��1���ʷ���   SyntaxError��2���﷨��
static void ErrMsg(unsigned LineNo, char *descrip, char *string);
static void PrintSyntaxTree(struct ExprNode *root, int indent);//���������ӡ�﷨��
static void Program();//����
static void Statement();//���
static void OriginStatement();
static void RotStatement();
static void ScaleStatement();
static void ForStatement();//����ֵΪָ���﷨���ڵ��ָ��ĺ�����
static struct ExprNode *Expression();//���ʽ����Ԫ�Ӽ�
static struct ExprNode *Term();//�˳�
static struct ExprNode *Factor();//һԪ�Ӽ�
static struct ExprNode *Component();
static struct ExprNode *Atom();

							  
extern void Parser(char *SrcFilePtr);

//�﷨������
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

//��üǺ�
static void FetchToken()
{
	token = GetToken();
	if (token.type == ERRTOKEN) SyntaxError(1); //�﷨����
}

//ƥ�䵱ǰ�Ǻ�
static void MatchToken(enum Token_Type The_Token)
{
	if (token.type != The_Token)
	{
		SyntaxError(2);
	}
	FetchToken();
}

//�﷨������
static void SyntaxError(int case_of)
{
	switch (case_of)
	{
	case 1: ErrMsg(LineNo, "����Ǻ� ", token.lexeme);
		break;
	case 2: ErrMsg(LineNo, "����Ԥ�ڼǺ�", token.lexeme);
		break;
	}
}

//��ӡ������Ϣ
void ErrMsg(unsigned LineNo, char *descrip, char *string)
{
	char msg[256];
	memset(msg, 0, 256);
	sprintf(msg, "Line No %5d:%s %s !", LineNo, descrip, string);

	MessageBox(NULL, msg, "error!", MB_OK);

	CloseScanner();
	exit(1);
}

//���������ӡ�﷨��
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
	if (root->OpCode == CONST_ID || root->OpCode == T) //Ҷ�ӽڵ�
		return;//��������ֵ����������ֵ��ַ
	if (root->OpCode == FUNC)//�ݹ��ӡһ�����ӽڵ�
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent + 1);//�����к��ӽڵ��ָ��������ָ��
	else
	{
		PrintSyntaxTree(root->Content.CaseOperater.Left, indent + 1);
		PrintSyntaxTree(root->Content.CaseOperater.Right, indent + 1);
	}
}

//��ͼ���Խ�����
void Parser(char *SrcFilePtr)
{
	if (!InitScanner(SrcFilePtr))
	{
		printf("Open Source File Error !\n"); return;
	}
	FetchToken();//��üǺ�
	Program();//�ݹ��½�����
	CloseScanner();
	return;
}

//����Program�ĵݹ��ӳ���
static void Program()
{
	while (token.type != NONTOKEN)
	{
		Statement();
		MatchToken(SEMICO);
	}
}

//Statment�ĵݹ��ӳ���
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

//OriginStatement�ĵݹ��ӳ���
static void OriginStatement(void)
{
	struct ExprNode *tmp;//�﷨���ڵ������
	MatchToken(ORIGIN);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();

	Origin_x = GetExprValue(tmp);//��ȡ�������ƽ�ƾ���
	DelExprTree(tmp);//ɾ��һ����

	MatchToken(COMMA);

	tmp = Expression();

	Origin_y = GetExprValue(tmp);//��ȡ�������ƽ�ƾ���
	DelExprTree(tmp);//ɾ��һ����

	MatchToken(R_BRACKET);
}

//ScaleStatement�ĵݹ��ӳ���
static void ScaleStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(SCALE);
	MatchToken(IS);
	MatchToken(L_BRACKET);

	tmp = Expression();

	Scale_x = GetExprValue(tmp);//��ȡ������ı�������
	DelExprTree(tmp);

	MatchToken(COMMA);

	tmp = Expression();

	Scale_y = GetExprValue(tmp);//��ȡ������ı�������
	DelExprTree(tmp);

	MatchToken(R_BRACKET);
}

//RotStatement�ĵݹ��ӳ���
static void RotStatement(void)
{
	struct ExprNode *tmp;
	MatchToken(ROT);
	MatchToken(IS);

	tmp = Expression();

	Rot_angle = GetExprValue(tmp);//��ת�Ƕ�
	DelExprTree(tmp);
}

//ForStatement�ĵݹ��ӳ���
static void ForStatement(void)
{
	double Start, End, Step;//��㡢�յ㡢����
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;

	MatchToken(FOR);
	MatchToken(T);
	MatchToken(FROM);

	start_ptr = Expression(); //��������ʽ���﷨��

	Start = GetExprValue(start_ptr);//��������ֵ
	DelExprTree(start_ptr);//�ͷ�����﷨���ռ�

	MatchToken(TO);

	end_ptr = Expression();//�����յ��﷨��

	End = GetExprValue(end_ptr);//�����յ�
	DelExprTree(end_ptr);

	MatchToken(STEP);

	step_ptr = Expression();//���첽���﷨��

	Step = GetExprValue(step_ptr);//���㲽��
	DelExprTree(step_ptr);

	MatchToken(DRAW);
	MatchToken(L_BRACKET);

	x_ptr = Expression();

	MatchToken(COMMA);

	y_ptr = Expression();

	MatchToken(R_BRACKET);

	DrawLoop(Start, End, Step, x_ptr, y_ptr); //����ͼ��
	DelExprTree(x_ptr);
	DelExprTree(y_ptr);
}

//Expression�ĵݹ��ӳ���
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
		left = MakeExprNode(token_tmp, left, right);//��������������
	}
	return left;//���ص�ǰ�ǺŽڵ�
}

//Term�ĵݹ��ӳ���
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

//Factor�ĵݹ��ӳ���
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

//Component�ĵݹ��ӳ���
static struct ExprNode *Component()
{
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER)
	{
		MatchToken(POWER);
		right = Component();//�ݹ����Component��ʵ��POWER���ҽ��
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//Atom�ĵݹ��ӳ���
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

//����һ���ڵ㣬���տɱ�����б�
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...)
{
	struct ExprNode *ExprPtr = new(struct ExprNode);
	ExprPtr->OpCode = opcode;
	va_list ArgPtr;
	va_start(ArgPtr, opcode);//��ʼ��va_list����
	switch (opcode)//����ڵ�
	{
	case CONST_ID://����
		ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
		break;
	case T://����T
		ExprPtr->Content.CaseParmPtr = &Parameter;
		break;
	case FUNC://��������
		ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
		ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	default://��Ԫ����
		ExprPtr->Content.CaseOperater.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		ExprPtr->Content.CaseOperater.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
		break;
	}
	va_end(ArgPtr);

	return ExprPtr;
}