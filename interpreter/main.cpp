#include "semantic.h"
#define MAX_CHARS 200
#pragma warning(disable: 4996)

HDC hDC;//窗口句柄，全局变量
char SrcFilePath[MAX_CHARS];//存放源程序文件路径
static char Name[] = "Interpreter";
static bool PrepareWindow(HINSTANCE, HINSTANCE, int);

static bool CheckSrcFile(LPSTR);

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//主函数
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	strcpy(SrcFilePath, lpCmdLine);//保存原文件路径
	if (PrepareWindow(hInstance, hPrevInstance, nCmdShow) != true)
	{
		MessageBox(NULL, "窗口初始化失败", "错误", MB_OK);
		return 1;
	}

	if (!CheckSrcFile(lpCmdLine)) return 1;
	Parser(SrcFilePath);
	//消息循环
	MSG Msg;
	while (GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

//初始化窗口函数实现
bool PrepareWindow(HINSTANCE hInst,
	HINSTANCE hPrevInstance,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASS W;

	memset(&W, 0, sizeof(WNDCLASS));
	W.style = CS_HREDRAW | CS_VREDRAW;
	W.lpfnWndProc = WndProc;
	W.hInstance = hInst;
	W.hCursor = LoadCursor(NULL, IDC_ARROW);
	W.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	W.lpszClassName = Name;
	RegisterClass(&W);
	hWnd = CreateWindow(Name, Name,
		WS_OVERLAPPEDWINDOW, 10, 10, 740, 490, NULL, NULL, hInst, NULL);
	if (hWnd == NULL)return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetCursor(LoadCursor(hInst, IDC_ARROW));

	hDC = GetDC(hWnd);
	return true;
}

//检查源程序文件是否合法函数实现
bool CheckSrcFile(LPSTR lpszCmdParam)
{
	FILE * file = NULL;
	if (strlen(lpszCmdParam) == 0)
	{
		MessageBox(NULL, "未指定源程序文件 !", "错误", MB_OK);
		return false;
	}
	if ((file = fopen(lpszCmdParam, "r")) == NULL)
	{
		MessageBox(NULL, "打开源程序文件出错 !", "错误", MB_OK);
		MessageBox(NULL, lpszCmdParam, "文件名", MB_OK);
		return false;
	}
	else fclose(file);
	return true;
}

//窗口处理函数实现
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam,
	LPARAM lParam)
{
	switch (Message)
	{
	case WM_DESTROY:
		ReleaseDC(hWnd, hDC);
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		PAINTSTRUCT pt;
		BeginPaint(hWnd, &pt);
		Parser(SrcFilePath);
		EndPaint(hWnd, &pt);
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
}