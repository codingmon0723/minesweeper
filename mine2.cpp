#pragma comment(linker , "/subsystem:windows")

#pragma comment(lib ,"msimg32.lib")

#include <windows.h>
#include <windowsX.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InfoDloProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RankingDloProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ResultDloProc(HWND, UINT, WPARAM, LPARAM);

void Main_OnDestroy(HWND);
BOOL Main_OnCreate(HWND, LPCREATESTRUCT);
void Create_Window(HWND);
void InitializeBorder(void);
void Init(HWND);
void Load_Bitmap(void);

void Main_OnPaint(HWND);
void OnPaintCount(HDC, HDC, char*, BOOL);
void OnPaintMine(HDC, HDC);
void OnPaintDefault(HDC, HDC);
void OnPaintSmile(HDC, HDC);
void OnPaintEnd(HDC, HDC);

void Main_OnLbuttonup(HWND, int, int, UINT);
void Main_OnLbuttondown(HWND, BOOL, int, int, UINT);
void Main_MouseMove(HWND, int, int, UINT);
void Main_OnRbuttonup(HWND, int, int, UINT);
void Main_OnRbuttondown(HWND, BOOL, int, int, UINT);

void SaveOption();
void GameSave();
BOOL GameLoad();

void Main_OnCommand(HWND, int, HWND, UINT);
void MenuCheck(HWND, int);
void Empty_Fine(int, int);
void IntToArray(long, char*);

void GameFailed(HWND hwnd);
void Game_Win(HWND);

void DeleteBlock();
void Block_Open(HWND, int, int);
void CopyBlock();
void UndoBlock();

void InsertBlock();

void StringCopy(char*);
void Draw3dRect(HDC, int, int, int, int, BOOL, int);
void Main_Timmer(HWND, UINT);
int MaxMinCheck(int, int);

enum STATE { EMPTY = 0, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, BOMB };
enum ISOPEN { CLOSE = 0, OPEN, QUESTION, FLAG };
enum RbuttonSTATION { REMPTY, ROPEN, RCLOSE };
enum MAXMINKIND { MINE, ROWCOL, UNDO };
enum LEVEL { PRIMARY = 1, INTERMEDIATE, ADVANCED, USER };

struct BLOCK {
	int State;
	int IsOpen;
};

struct OffSets {
	short int horiz;
	short int vert;
};

struct MAXMIN {
	int MIN;
	int MAX;
};

struct tagUNDO {
	BLOCK **Block;
};

char Highest_Primary_Name[20];
char Highest_Intermediate_Name[20];
char Highest_Advanced_Name[20];
char User_Name[20];

int Highest_Primary_Time;
int Highest_Intermediate_Time;
int Highest_Advanced_Time;

HWND hDlgMain;
MAXMIN RowCol = { 10,90 };
MAXMIN Undo = { 0,10 };
MAXMIN Mine = { 1,0 };

BLOCK **Block;
tagUNDO *Mask;

HINSTANCE g_hInstance;
LPCWSTR lpszClass = (LPCWSTR)"지뢰찾기";

RECT Window_Size;
RECT Game_Size;
RECT Smile_Rect;

int Main_ROW, Main_COL;
int Mine_Total, Remain_Mine;
int Undo_Total, Remain_Undo;

int TimeCount;
int Game_Level;
int Right_Button_State;
int Menu_Checked;
int Menu_Checked1;

BOOL Left_Button_State;
BOOL Game_End = FALSE;
BOOL Game_Box = FALSE;
BOOL Smile_Button_Down = FALSE;
BOOL Right_Button_Down = FALSE;
BOOL Game_Sucessce = FALSE;
BOOL InsertFlag = FALSE;
BOOL Game_Flag;
POINTS DownSelect;
HANDLE hTimer;

OffSets Move[8] = { {-1,-1} , {0. - 1} , {1,-1} , {-1,0} , {1,0} , {-1,1} ,{0.1},{1,1} };

HBITMAP Old, MineNumber, RedNumber, Smile, Back;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInstance = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(NULL));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName = lpszClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW &WS_MAXIMIZEBOX &WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT
		, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
		HANDLE_MSG(hWnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hWnd, WM_PAINT, Main_OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, Main_OnDestroy);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, Main_MouseMove);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, Main_OnLbuttonup);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, Main_OnLbuttondown);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, Main_OnRbuttonup);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, Main_OnRbuttondown);
		HANDLE_MSG(hWnd, WM_TIMER, Main_Timmer);
		HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}

BOOL Main_CnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	Load_Bitmap();
	InitializeBorder();
	Init(hwnd);
	return TRUE;
}

void Load_Bitmap(void)
{
	MineNumber = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
	RedNumber = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
	Smile = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP4));
}

void Create_Window(HWND hwnd)
{
	Window_Size.left = 0;
	Window_Size.top = 0;
	Window_Size.right = (Main_ROW + 1) * 16 + 8;
	Window_Size.bottom = (Main_COL + 1) * 16 + 51;

	Smile_Rect.left = (Window_Size.right - Window_Size.left) / 2 - 14;
	Smile_Rect.right = Smile_Rect.left + 24;
	Smile_Rect.top = 15;
	Smile_Rect.bottom = Smile_Rect.top + 24;

	Game_Size.top = 55;
	Game_Size.left = 12;
	Game_Size.right = Main_ROW * 16 + 12;
	Game_Size.bottom = Main_COL * 16 + 55;

	AdjustWindowRect(&Window_Size, GetWindowLong(hwnd, GWL_STYLE), TRUE);

	int width = Window_Size.right - Window_Size.left;
	int height = Window_Size.bottom - Window_Size.top;
	int x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	HMENU hMenu = GetMenu(hwnd);

	CheckMenuItem(hMenu, Menu_Checked, MF_CHECKED);

	MoveWindow(hwnd, x, y, width, height, TRUE);
}

void InitializeBorder(void)
{
	Main_COL = GetPrivateProfileInt("Option", "Main_COL", 10, "Mine.ini");
	Main_ROW = GetPrivateProfileInt("Option", "Main_ROW", 10, "Mine.ini");
	Mine_Total = GetPrivateProfileInt("Option", "Main_Total", 10, "Mine.ini");
	Undo_Total = GetPrivateProfileInt("Option", "Undo_Total", 10, "Mine.ini");
	Game_Flag = GetPrivateProfileInt("SAVELOAD", "Game_Flag", 0, "Mine.ini");
	Game_Level = GetPrivateProfileInt("Option", "Game_Level", PRIMARY, "Mine.ini");
	Menu_Checked = GetPrivateProfileInt("Option", "Menu_Checked", NULL, "Mine.ini");

	Highest_Primary_Time = GetPrivateProfileInt("Ranking", "Highest_Primary_Time", 999, "Mine.ini");
	GetPrivateProfileString("Ranking", "Highest_Primary_Name", "익명", Highest_Primary_Name, 50, "Mine.ini");

	Highest_Intermediate_Time = GetPrivateProfileInt("Ranking", "Highest_Intermediate_Time", 999, "Mine.ini");
	GetPrivateProfileString("Ranking", "Highest_Intermediate_Name", "익명", Highest_Intermediate_Name, 50, "Mine.ini");

	Highest_Advanced_Time = GetPrivateProfileInt("Ranking", "Highest_Advanced_Time", 999, "Mine.ini");
	GetPrivateProfileString("Ranking", "Highest_Advanced_Name", "익명", Highest_Advanced_Name, 50, "Mine.ini");
}

void Init(HWND hwnd)
{
	int Count;

	Game_End = FALSE;
	Mine.MAX = (Main_ROW * Main_COL) / 100 * 81;

	Remain_Mine = Mine_Total;
	Remain_Undo = -1;

	srand((unsigned)time(NULL));

	Block = new BLOCK*[Main_ROW];
	Mask = new tagUNDO[Undo_Total];
	for (int i = 0; i < Undo_Total; i++)
	{
		Mask[i].Block = new BLOCK*[Main_ROW];
		for (int j = 0; j < Main_ROW; j++)
		{
			Mask[i].Block[j] = new BLOCK[Main_COL];
		}
	}
}

