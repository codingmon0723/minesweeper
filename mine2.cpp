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
MAXMIN RowCol = { 10, 90 };
MAXMIN Undo = { 0, 10 };
MAXMIN Mine = { 1, 0 };

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

OffSets Move[8] = { { -1, -1 }, { 0. - 1 }, { 1, -1 }, { -1, 0 }, { 1, 0 }, { -1, 1 }, { 0.1 }, { 1, 1 } };

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
	Mine_Total = GetPrivateProfileInt("Option", "Mine_Total", 10, "Mine.ini");
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
	for (int i = 0; i < Main_ROW; i++)
	{
		Block[i] = new BLOCK[Main_COL];
		for (int j = 0; j < Main_COL; j++)
		{
			Block[i][j].State = EMPTY;
			Block[i][j].IsOpen = CLOSE;
		}
	}

	for (int i = 0; i < Mine_Total; i++)
	{
		int Temp_Row = rand() % Main_ROW;
		int Temp_Col = rand() % Main_COL;

		if (Block[Temp_Row][Temp_Col].State == EMPTY)
		{
			Block[Temp_Row][Temp_Col].State = BOMB;
		}
		else
		{
			i--;
		}

	}

	for (int i = 0; i < Main_ROW; i++)
	{
		for (int j = 0; j < Main_COL; j++)
		{
			if (Block[i][j].State == EMPTY)
			{
				Count = 0;
				for (int k = i - 1; k <= i + 1; k++)
				{
					for (int l = j - 1; l <= j + 1; l++)
					{
						if (k >= 0 && k < Main_ROW && l >= 0 && l < Main_COL)
						{
							if (Block[k][l].State == BOMB)
								Count++;
						}
					}
				}
				switch (Count)
				{
				case 0: Block[i][j].State = EMPTY; break;
				case 1: Block[i][j].State = ONE; break;
				case 2: Block[i][j].State = TWO; break;
				case 3: Block[i][j].State = THREE; break;
				case 4: Block[i][j].State = FOUR; break;
				case 5: Block[i][j].State = FIVE; break;
				case 6: Block[i][j].State = SIX; break;
				case 7: Block[i][j].State = SEVEN; break;
				case 8: Block[i][j].State = EIGHT; break;

				}
			}
		}
	}
	Create_Window(hwnd);
}

void Main_OnDestroy(HWND hwnd)
{
	DeleteObject(MineNumber);
	DeleteObject(RedNumber);
	DeleteObject(Smile);

	SaveOption();
	DeleteBlock();

	KillTimer(hwnd, 1);
	PostQuitMessage(0);
}

void Main_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc, memdc, bufdc;
	char Array[4];
	Array[3] = '\0';

	hdc = BeginPaint(hwnd, &ps);

	memdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(0);

	GetClientRect(hwnd, &Window_Size);
	Back = CreateCompatibleBitmap(hdc, Window_Size.right, Window_Size.bottom);
	Old = (HBITMAP)SelectObject(memdc, Back);

	OnPaintDefault(memdc, bufdc);
	OnPaintMine(memdc, bufdc);
	IntToArray(Remain_Mine, Array);
	OnPaintCount(memdc, bufdc, Array, TRUE);
	IntToArray(TimeCount, Array);
	OnPaintCount(memdc, bufdc, Array, FALSE);
	OnPaintSmile(memdc, bufdc);
	if (Game_End == TRUE)
	{
		OnPaintEnd(memdc, bufdc);
	}

	BitBlt(hdc, 0, 0, Window_Size.right, Window_Size.bottom, memdc, 0, 0, SRCCOPY);

	SelectObject(memdc, Old);
	DeleteObject(Back);
	DeleteDC(memdc);
	DeleteDC(bufdc);

	EndPaint(hwnd, &ps);
}

void Draw3dRect(HDC hdc, int x, int y, int xx, int yy, BOOL down, int width)
{
	COLORREF clrMain = RGB(192, 192, 192), clrLight = RGB(255, 255, 255), clrDark = RGB(128, 128, 128);
	HPEN hPen1, hPen2, hOldPen;
	HBRUSH hBrush, hOldBrush;

	if (down)
	{
		hPen2 = CreatePen(PS_SOLID, 1, clrLight);
		hPen1 = CreatePen(PS_SOLID, 1, clrDark);
	}
	else
	{
		hPen1 = CreatePen(PS_SOLID, 1, clrLight);
		hPen2 = CreatePen(PS_SOLID, 1, clrDark);
	}

	hBrush = CreateSolidBrush(clrMain);
	hOldPen = (HPEN)SelectObject(hdc, hPen1);
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Rectangle(hdc, x, y, xx + 1, yy + 1);

	for (int i = 0; i < width; i++)
	{
		SelectObject(hdc, hPen1);

		MoveToEx(hdc, xx - 1, y, 0);
		LineTo(hdc, x, y);
		LineTo(hdc, x, yy - 1);

		SelectObject(hdc, hPen2);

		MoveToEx(hdc, x, yy, 0);
		LineTo(hdc, xx, yy);
		LineTo(hdc, xx, y);

		x++; y++; xx--; yy--;
	}
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);

	DeleteObject(hPen1);
	DeleteObject(hPen2);
	DeleteObject(hBrush);
}

void OnPaintCount(HDC memdc, HDC bufdc, char* Array, BOOL MINEorTIMMER)
{
	SelectObject(bufdc, RedNumber);

	for (int i = 0; i < 3; i++)
	{
		if (MINEorTIMMER)
		{
			switch (Array[i])
			{
			case'_': BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 0, SRCCOPY); break;
			case 9: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 46, SRCCOPY); break;
			case 8: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 69, SRCCOPY); break;
			case 7: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 92, SRCCOPY); break;
			case 6: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 115, SRCCOPY); break;
			case 5: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 138, SRCCOPY); break;
			case 4: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 161, SRCCOPY); break;
			case 3: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 184, SRCCOPY); break;
			case 2: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 207, SRCCOPY); break;
			case 1: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 230, SRCCOPY); break;
			case 0: BitBlt(memdc, 16 + (i * 13), 16, 13, 23, bufdc, 0, 253, SRCCOPY); break;

			}
		}
		else
		{
			int x = Window_Size.right - 58;
			switch (Array[i])
			{
			case 9: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 46, SRCCOPY); break;
			case 8: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 69, SRCCOPY); break;
			case 7: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 92, SRCCOPY); break;
			case 6: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 115, SRCCOPY); break;
			case 5: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 138, SRCCOPY); break;
			case 4: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 161, SRCCOPY); break;
			case 3: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 184, SRCCOPY); break;
			case 2: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 207, SRCCOPY); break;
			case 1: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 230, SRCCOPY); break;
			case 0: BitBlt(memdc, x + (i * 13), 16, 13, 23, bufdc, 0, 253, SRCCOPY); break;
			}
		}
	}
}


void OnPaintMine(HDC memdc, HDC bufdc)
{
	SelectObject(bufdc, MineNumber);
	for (int x = 12; x < Main_ROW * 16; x += 16)
	{
		for (int y = 55; y < Main_COL * 16 + 50; y += 16)
		{
			if (Block[(x - 12) / 16][(y - 55) / 16].IsOpen == OPEN)
			{
				switch (Block[(x - 12) / 16][(y - 55) / 16].State)
				{
				case EMPTY: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 240, SRCCOPY);
				case ONE: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 224, SRCCOPY);
				case TWO: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 208, SRCCOPY);
				case THREE: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 192, SRCCOPY);
				case FOUR: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 176, SRCCOPY);
				case FIVE: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 160, SRCCOPY);
				case SIX: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 144, SRCCOPY);
				case SEVEN: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 128, SRCCOPY);
				case EIGHT: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 112, SRCCOPY);
				case BOMB: BitBlt(memdc, x, y, 16, 16, bufdc, 0, 80, SRCCOPY);

				}
			}
			else if (Block[(x - 12) / 16][(y - 55) / 16].IsOpen == FLAG)
			{
				BitBlt(memdc, x, y, 16, 16, bufdc, 0, 16, SRCCOPY);
			}
			else if (Block[(x - 12) / 16][(y - 55) / 16].IsOpen == QUESTION)
			{
				BitBlt(memdc, x, y, 16, 16, bufdc, 0, 32, SRCCOPY);
			}
			else
			{
				BitBlt(memdc, x, y, 16, 16, bufdc, 0, 0, SRCCOPY);
			}
		}
	}
	int Row = (DownSelect.x - 12) / 16;
	int Col = (DownSelect.y - 55) / 16;

	if (Left_Button_State == TRUE && Game_Box == TRUE && Right_Button_State == RCLOSE)
	{
		int cnt = 0;

		if (Block[Row][Col].IsOpen == QUESTION)
		{
			BitBlt(memdc, DownSelect.x, DownSelect.y, 16, 16, bufdc, 0, 96, SRCCOPY);
		}
		if (Block[Row][Col].IsOpen == CLOSE)
		{
			BitBlt(memdc, DownSelect.x, DownSelect.y, 16, 16, bufdc, 0, 240, SRCCOPY);
		}
		while (cnt < 8)
		{
			if (((Row + Move[cnt].horiz) >= 0) && ((Row + Move[cnt].horiz) < Main_ROW) && ((Col + Move[cnt].vert) >= 0) && ((Col + Move[cnt].vert) < Main_COL))
			{
				if (Block[Row + Move[cnt].horiz][Col + Move[cnt].vert].IsOpen == CLOSE)
				{
					BitBlt(memdc, (Row + Move[cnt].horiz * 16 + 12), (Col + Move[cnt].vert) * 16 + 55, 16, 16, bufdc, 0, 240, SRCCOPY);
				}
				else if (Block[Row + Move[cnt].horiz][Col + Move[cnt].vert].IsOpen == QUESTION)
				{
					BitBlt(memdc, (Row + Move[cnt].horiz * 16 + 12), (Col + Move[cnt].vert) * 16 + 55, 16, 16, bufdc, 0, 96, SRCCOPY);
				}
			}
			cnt++;
		}
	}
	else if (Left_Button_State == TRUE && Game_Box == TRUE)
	{
		if (Block[Row][Col].IsOpen == QUESTION)
		{
			BitBlt(memdc, DownSelect.x, DownSelect.y, 16, 16, bufdc, 0, 96, SRCCOPY);
		}
		if (Block[Row][Col].IsOpen == CLOSE)
		{
			BitBlt(memdc, DownSelect.x, DownSelect.y, 16, 16, bufdc, 0, 240, SRCCOPY);
		}
	}
}

void OnPaintDefault(HDC memdc, HDC bufdc)
{
	Draw3dRect(memdc, 0, 0, Window_Size.right, Window_Size.bottom, FALSE, 3);
	Draw3dRect(memdc, 9, 9, Window_Size.right - 9, Window_Size.bottom - 9, TRUE, 2);
	Draw3dRect(memdc, 15, 15, 55, 40, TRUE, 1);
	Draw3dRect(memdc, Window_Size.right - 59, 15, Window_Size.right - 19, 40, TRUE, 1);
	Draw3dRect(memdc, 9, 52, Window_Size.right - 9, Window_Size.bottom - 9, TRUE, 3);
}

void OnPaintSmile(HDC memdc, HDC bufdc)
{
	SelectObject(bufdc, Smile);

	if (Smile_Button_Down == TRUE && Left_Button_State == FALSE)
	{
		BitBlt(memdc, (Window_Size.right - Window_Size.left) / 2 - 14, 15, 24, 24, bufdc, 0, 0, SRCCOPY);
	}
	else if (Left_Button_State == TRUE)
	{
		BitBlt(memdc, (Window_Size.right - Window_Size.left) / 2 - 14, 15, 24, 24, bufdc, 0, 72, SRCCOPY);
	}
	else if (Game_End == TRUE && Game_Sucessce == TRUE)
	{
		BitBlt(memdc, (Window_Size.right - Window_Size.left) / 2 - 14, 15, 24, 24, bufdc, 0, 24, SRCCOPY);
	}
	else if (Game_End == TRUE && Game_Sucessce == FALSE)
	{
		BitBlt(memdc, (Window_Size.right - Window_Size.left) / 2 - 14, 15, 24, 24, bufdc, 0, 48, SRCCOPY);
	}
	else
	{
		BitBlt(memdc, (Window_Size.right - Window_Size.left) / 2 - 14, 15, 24, 24, bufdc, 0, 96, SRCCOPY);
	}
}

void OnPaintEnd(HDC memdc, HDC bufdc)
{
	SelectObject(bufdc, MineNumber);
	for (int x = 12; x <= Main_ROW * 16; x += 16)
	{
		for (int y = 55; y <= Main_COL * 16 + 50; y += 16)
		{
			if (Block[(x - 12) / 16][(y - 55) / 16].IsOpen == OPEN && Block[(x - 12) / 16][(y - 55) / 16].State == BOMB)
				BitBlt(memdc, x, y, 16, 16, bufdc, 0, 48, SRCCOPY);
			else if (Block[(x - 12) / 16][(y - 55) / 16].IsOpen == CLOSE && Block[(x - 12) / 16][(y - 55) / 16].State == BOMB)
				BitBlt(memdc, x, y, 16, 16, bufdc, 0, 80, SRCCOPY);
			else if (Block[(x - 12) / 16][(y - 55) / 16].IsOpen == FLAG && Block[(x - 12) / 16][(y - 55) / 16].State != BOMB)
				BitBlt(memdc, x, y, 16, 16, bufdc, 0, 64, SRCCOPY);
		}
	}
}

void GameSave()
{
	char Buffer1[10];
	char *Buffer2;
	Game_Flag = 1;

	_itoa(Main_COL, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Main_COL", Buffer1, "Mine.ini");

	_itoa(Main_ROW, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Main_ROW", Buffer1, "Mine.ini");

	_itoa(Mine_Total, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Mine_Total", Buffer1, "Mine.ini");

	_itoa(Remain_Mine, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Remain_Mine", Buffer1, "Mine.ini");

	_itoa(Undo_Total, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Undo_Total", Buffer1, "Mine.ini");

	_itoa(TimeCount, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "TimeCount", Buffer1, "Mine.ini");

	_itoa(Game_Flag, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Game_Flag", Buffer1, "Mine.ini");

	_itoa(Game_Level, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Game_Level", Buffer1, "Mine.ini");

	_itoa(Menu_Checked, Buffer1, 10);
	WritePrivateProfileString("SAVELOAD", "Menu_Checked", Buffer1, "Mine.ini");

	Buffer2 = new char[(Main_ROW * Main_COL * 2) + 1];
	int count = 0;

	for (int i = 0; i < Main_ROW; i++)
	{
		for (int j = 0; i < Main_COL; j++)
		{
			Buffer2[count++] = Block[i][j].State + 48;
			Buffer2[count++] = Block[i][j].IsOpen + 48;
		}
	}
	Buffer2[count] = '|0';
	WritePrivateProfileString("SAVELOAD", "SAVE", Buffer2, "Mine.ini");
	delete[]Buffer2;
}

BOOL GameLoad()
{
	char *Buffer1;
	int count = 0;

	if (Game_Flag > 0)
	{
		DeleteBlock();

		Main_COL = GetPrivateProfileInt("Option", "Main_COL", 10, "Mine.ini");
		Main_ROW = GetPrivateProfileInt("Option", "Main_ROW", 10, "Mine.ini");
		Mine_Total = GetPrivateProfileInt("Option", "Mine_Total", 10, "Mine.ini");
		Remain_Mine = GetPrivateProfileInt("Option", "Remain_Mine", 10, "Mine.ini");
		Undo_Total = GetPrivateProfileInt("SAVELOAD", "Undo_Total", 10, "Mine.ini");
		Game_Level = GetPrivateProfileInt("Option", "Game_Level", 1, "Mine.ini");
		TimeCount = GetPrivateProfileInt("Option", "TimeCount", 0, "Mine.ini");

		Menu_Checked1 = GetPrivateProfileInt("SAVELOAD", "Menu_Check", NULL, "Mine.ini");

		Buffer1 = new char[(Main_ROW * Main_COL * 2) + 1];

		GetPrivateProfileString("SAVELOAD", "SAVE", NULL, Buffer1, (Main_ROW * Main_COL * 2) + 1, "Mine.ini");

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

		for (int i = 0; i < Main_ROW; i++)
		{
			Block[i] = new BLOCK[Main_COL];
		}
		for (int i = 0; i < Main_ROW; i++)
		{
			for (int j = 0; j < Main_COL; j++)
			{
				Block[i][j].State = Buffer1[count++] - 48;
				Block[i][j].State = Buffer1[count++] - 48;
			}
		}
		delete[]Buffer1;
		if (Game_End == TRUE)
			Game_End = FALSE;
		return TRUE;
	}
	return FALSE;
}

void SaveOption()
{
	char Buffer[10];

	_itoa(Main_COL, Buffer, 10);
	WritePrivateProfileString("Option", "Main_COL", Buffer, "Mine.ini");

	_itoa(Main_ROW, Buffer, 10);
	WritePrivateProfileString("Option", "Main_ROW", Buffer, "Mine.ini");

	_itoa(Mine_Total, Buffer, 10);
	WritePrivateProfileString("Option", "Mine_Total", Buffer, "Mine.ini");

	_itoa(Undo_Total, Buffer, 10);
	WritePrivateProfileString("Option", "Undo_Total", Buffer, "Mine.ini");

	_itoa(Game_Flag, Buffer, 10);
	WritePrivateProfileString("Option", "Game_Flag", Buffer, "Mine.ini");

	_itoa(Game_Level, Buffer, 10);
	WritePrivateProfileString("Option", "Game_Level", Buffer, "Mine.ini");

	_itoa(Menu_Checked, Buffer, 10);
	WritePrivateProfileString("Option", "Menu_Checked", Buffer, "Mine.ini");
}

void Main_OnLbuttonup(HWND hwnd, int x, int y, UINT keyFlags)
{
	ReleaseCapture();
	BOOL UP = FALSE;
	int Row;
	int Col;
	int cnt = 0;

	int FlagCount = 0;
	Left_Button_State = FALSE;

	if (Game_End == FALSE)
	{
		if (!(Game_Size.left >= x || Game_Size.right <= x || Game_Size.top >= y || Game_Size.bottom <= y))
		{
			if (TimeCount == 0)
			{
				hTimer = (HANDLE)SetTimer(hwnd, 1, 1000, NULL);
				TimeCount++;
			}
			Row = (x - 12) / 16;
			Col = (y - 55) / 16;
			UP = TRUE;
			InsertBlock();
			Block_Open(hwnd, Row, Col);
		}
		if (UP == TRUE && Right_Button_State != 0)
		{
			while (cnt < 8)
			{
				if (((Row + Move[cnt].horiz) >= 0) && ((Row + Move[cnt].horiz) < Main_ROW) && ((Col + Move[cnt].vert) >= 0) && ((Col + Move[cnt].vert) < Main_COL))
				{
					if (Block[Row + Move[cnt].horiz][Col + Move[cnt].vert].IsOpen == FLAG)
						FlagCount++;
				}
				cnt++;
			}
			cnt = 0;
			if (Block[Row][Col].State == FlagCount)
			{
				InsertBlock();
				while (cnt < 8)
				{
					if (((Row + Move[cnt].horiz) >= 0) && ((Row + Move[cnt].horiz) < Main_ROW) && ((Col + Move[cnt].vert) >= 0) && ((Col + Move[cnt].vert) < Main_COL))
					{
						Block_Open(hwnd, Row + Move[cnt].horiz, Col + Move[cnt].vert);
					}
					cnt++;
				}
			}
		}
		Game_Win(hwnd);
	}

	if ((Smile_Rect.left <= x && Smile_Rect.right >= x && Smile_Rect.top <= y && Smile_Rect.bottom >= y) && Smile_Button_Down == TRUE)
	{
		DeleteBlock();
		InitializeBorder();
		Init(hwnd);
		InsertFlag = FALSE;
		KillTimer(hwnd, 1);
		TimeCount = 0;
	}
	Smile_Button_Down = FALSE;
	InvalidateRect(hwnd, NULL, FALSE);
}

void Main_OnLButtondown(HWND hwnd, BOOL fDoubleClock, int x, int y, UINT keyFlags)
{
	SetCapture(hwnd);
	if (Game_End == FALSE)
	{
		Left_Button_State = TRUE;
		int Row = (x - 12) / 16;
		int Col = (y - 55) / 16;

		if (Game_Size.left >= x || Game_Size.right <= x || Game_Size.top >= y || Game_Size.bottom <= y)
			Game_Box = FALSE;
		else
			Game_Box = TRUE;
	}
	if ((Smile_Rect.left <= x && Smile_Rect.right >= x && Smile_Rect.top <= y && Smile_Rect.bottom >= y))
	{
		Smile_Button_Down = TRUE;
		Left_Button_State = FALSE;
	}
	InvalidateRect(hwnd, NULL, FALSE);
}

void Main_MouseMove(HWND hwnd, int x, int y, UINT KeyFlags)
{
	if (Game_End == FALSE)
	{
		if (Game_Size.left >= x || Game_Size.right <= x || Game_Size.top >= y || Game_Size.bottom <= y)
			Game_Box = FALSE;
		else
		{
			for (int i = 12; i < x; i += 16)
			{
				for (int j = 55; j < y; j += 16)
				{
					DownSelect.x = i;
					DownSelect.y = j;
				}
			}
			Game_Box = TRUE;
		}
	}
	if (!(Smile_Rect.left <= x && Smile_Rect.right >= x && Smile_Rect.top <= y && Smile_Rect.bottom >= y))
	{
		Smile_Button_Down = FALSE;
	}
	InvalidateRect(hwnd, NULL, FALSE);
}

void Main_Timmer(HWND hwnd, UINT TimerNumber)
{
	if (TimeCount < 1000)
	{
		TimeCount++;
		InvalidateRect(hwnd, NULL, FALSE);
	}
}

void Empty_Fine(int x, int y)
{
	int cnt = 0;
	while (cnt < 8)
	{
		if (((x + Move[cnt].horiz) >= 0) && ((x + Move[cnt].horiz) < Main_ROW) && ((y + Move[cnt].vert) >= 0) && ((y + Move[cnt].vert) <= Main_COL))
		{
			if ((Block[x + Move[cnt].horiz][y + Move[cnt].vert].State == EMPTY) && (Block[x + Move[cnt].horiz][y + Move[cnt].vert].IsOpen == CLOSE))
			{
				Block[x + Move[cnt].horiz][y + Move[cnt].vert].IsOpen = OPEN;
				Empty_Fine(x + Move[cnt].horiz, y + Move[cnt].vert);
			}
			else if ((Block[x + Move[cnt].horiz][y + Move[cnt].vert].IsOpen == FLAG))
				;
			else
				Block[x + Move[cnt].horiz][y + Move[cnt].vert].IsOpen = OPEN;
		}
		cnt++;
	}
}

void Main_OnRbuttonup(HWND hwnd, int x, int y, UINT keyFlas)
{
	BOOL UP = FALSE;
	Right_Button_State = FALSE;
	int Row, Col;
	int FlagCount = 0;
	int cnt = 0;

	if (Game_End == FALSE)
	{
		if (!(Game_Size.left >= x || Game_Size.right <= x || Game_Size.top >= y || Game_Size.bottom <= y))
		{
			Row = (x - 12) / 16;
			Col = (y - 55) / 16;
			UP = TRUE;
		}
		if (UP == TRUE && Left_Button_State == TRUE)
		{
			while (cnt < 8)
			{
				if (((Row + Move[cnt].horiz) >= 0) && ((Row + Move[cnt].horiz) < Main_ROW) && ((Col + Move[cnt].vert) >= 0) && ((Col + Move[cnt].vert) <= Main_COL))
				{
					if (Block[Row + Move[cnt].horiz][Col + Move[cnt].vert].IsOpen = FLAG)
						FlagCount++;
				}
				cnt++;
			}
			cnt = 0;
			if (Block[Row][Col].State == FlagCount)
			{
				while (cnt < 8)
				{
					if (((Row + Move[cnt].horiz) >= 0) && ((Row + Move[cnt].horiz) < Main_ROW) && ((Col + Move[cnt].vert) >= 0) && ((Col + Move[cnt].vert) <= Main_COL))
						Block_Open(hwnd, Row + Move[cnt].horiz, Col + Move[cnt].vert);
					cnt++;
				}
			}
		}
		UP = FALSE;
		Right_Button_State = REMPTY;
		InvalidateRect(hwnd, NULL, FALSE);
	}

}
void Main_OnRbuttondown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags){
	Right_Button_State = TRUE;
	int Row, Col;
	int cnt = 0;
	Right_Button_Down = FALSE;

	if (Game_End == FALSE){
		if (!(Game_Size.left >= x || Game_Size.right <= x || Game_Size.top >= y || Game_Size.bottom <= y)){
			Row = (x - 12 / 16);
			Col = (y - 55) / 16;
			Right_Button_Down = TRUE;
		}

		if (Right_Button_Down == TRUE){
			if (Left_Button_State == FALSE){
				if (Block[Row][Col].IsOpen == CLOSE){
					Block[Row][Col].IsOpen = FLAG;
				}
				else if (Block[Row][Col].IsOpen == FLAG){
					Block[Row][Col].IsOpen = QUESTION;
				}
				else if (Block[Row][Col].IsOpen == QUESTION){
					Block[Row][Col].IsOpen = CLOSE;
				}
				Remain_Mine = Mine_Total;
				for (int i = 0; i < Main_ROW; i++){
					for (int j = 0; j < Main_COL; j++){
						if (Block[i][j].IsOpen == FLAG){
							Remain_Mine--;
						}
					}
				}
			}
			else{
				Right_Button_State = RCLOSE;
			}
		}
		Game_Win(hwnd);
	}
	InvalidateRect(hwnd, NULL, FALSE);
}

void Main_OnCommand(HWND hwnd, int ID, HWND hwndCtl, UINT codeNotify){
	char str[256];

	switch (ID){
	case ID_MENU_START:
		DeleteBlock();
		InsertFlag = FALSE;
		Init(hwnd);
		KillTimer(hwnd, 1);
		TimeCount = 0;
		break;
	case ID_MENU_SAVE:
		if (Game_End){
			MessageBox(hwnd, "게임이 끝났습니다. \n\n 저장 불가", "저장불가", MB_OK);
			if ((MessageBox(hwnd, "새로 게임을 하시겠습니까?", "새게임", MB_YESNO)) == IDYES){
				InsertFlag = FALSE;
				InitializeBorder();
				Init(hwnd);
				KillTiemr(hwnd, 1);
				TimeCount = 0;
			}
		}
		else
			GameSave();
		break;
	case ID_MENU_LOAD:
		if (GameLoad()){
			InsertFlag = FALSE;
			MenuCheck(hwnd, Menu_Checked1);
			Create_Window(hwnd);
			InvalidateRect(hwnd, NULL, FALSE);
			hTimer = (HANDLE)SetTimer(hwnd, 1, 1000, NULL);
		}
		break;
	case ID_MENU_PRIMARY:
		DeleteBlock();

		MenuCheck(hwnd, ID_MENU_PRIMARY);

		InsertFlag = FALSE;
		Main_COL = 10;
		Main_ROW = 10;
		Mine_Total = 10;
		Undo_Total = 10;
		Game_Flag = GetPrivateProfileInt("SAVELOAD", "Game_Flag", 0, "Mine.ini");
		Game_Level = PRIMARY;
		Init(hwnd);
		KillTimer(hwnd, 1);
		TimeCount = 0;
		SaveOption();

		break;

	case ID_MENU_INTERMEDIATE:
		DeleteBlock();

		MenuCheck(hwnd, ID_MENU_INTERMEDIATE);

		InsertFlag = FALSE;
		Main_COL = 16;
		Main_ROW = 16;
		Mine_Total = 40;
		Undo_Total = 5;
		Game_Flag = GetPrivateProfileInt("SAVELOAD", "Game_Flag", 0, "Mine.ini");
		Game_Level = INTERMEDIATE;
		Init(hwnd);
		KillTimer(hwnd, 1);
		TimeCount = 0;
		SaveOption();

		break;
	case ID_MENU_ADVANCED:
		DeleteBlock();

		MenuCheck(hwnd, ID_MENU_INTERMEDIATE);

		InsertFlag = FALSE;
		Main_COL = 16;
		Main_ROW = 30;
		Mine_Total = 99;
		Undo_Total = 1;
		Game_Flag = GetPrivateProfileInt("SAVELOAD", "Game_Flag", 0, "Mine.ini");
		Game_Level = ADVANCED;
		Init(hwnd);
		KillTimer(hwnd, 1);
		TimeCount = 0;
		SaveOption();

		break;
	case ID_MENU_USER:
		if (DialogBox(g_hInstance, MAKEINTERSOURCE(OPTION_DIALOG), hwnd, (DLGPROC)InfoDloProc) == IDOK){
			MenuCheck(hwnd, ID_MENU_USER);
			Game_Level = USER;
			SaveOption();
			Init(hwnd);
			KillTimer(hwnd, 1);
			TimeCount = 0;
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	case ID_MENU_EXIT:
		SendMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	case ID_MENU_UNDO:
		if (Remain_Undo >= 0 && InsertFlag){
			UndoBlock();

			if (Game_End == TRUE){
				Game_End = FALSE;
				hTimer = (HANDLE)SetTimer(hwnd, 1, 1000, NULL);
			}

			Remain_Mine = Mine_Total;
			for (int i = 0; i < Main_ROW; i++){
				for (int j = 0; j < Main_COL; j++){
					if (Block[i][j].IsOpen == FLAG){
						Remain_Mine--;
					}
				}
			}
			wsprintf(str, "되돌릴수 있는 공간이 \n %d개 있습니다.", Remain_Undo + 1);
			MessageBox(hwnd, str, "알림", MB_OK);
			if (Remain_Undo < 0){
				InsertFlag = FALSE;
			}
		}
		else if (!InsertFlag)
			MessageBox(hwnd, "저장 되어 있는 \n 되돌림 가능한 공간이 없습니다.", "경고", MB_OK);
		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case ID_MENU_RANKING:
		DialogBox(g_hInstance, MAKEINTERSOURCE(IDO_DIALOG2), hwnd, (DLGPROC)ResultDloProc);
		break;
	}
}

LRESULT CALLBACK InfoDloProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_MAIN_ROW, Main_ROW, 0);
		SetDlgItemInt(hDlg, IDC_MAIN_COL, Main_COL, 0);
		SetDlgItemInt(hDlg, IDC_MAIN_MINE, Mine_Total, 0);
		SetDlgItemInt(hDlg, IDC_MAIN_UNDO, Undo_Total, 0);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDOK:
		{
			DeleteBlock();
			Main_ROW = MaxMinCheck(GetDlgItemInt(hDlg, IDC_MAIN_ROW, NULL, FALSE), ROWCOL);
			Main_COL = MaxMinCheck(GetDlgItemInt(hDlg, IDC_MAIN_COL, NULL, FALSE), ROWCOL);
			Mine.MAX = Main_ROW * Main_COL / 100 * 81;
			Mine_Total = MaxMinCheck(GetDlgitemInt(hDlg, IDC_MAIN_MINE, NULL, FALSE), MINE);
			Undo_Total = MaxMinCheck(GetDlgitemInt(hDlg, IDC_MAIN_UNDO, NULL, FALSE), UNDO);
		}
		EndDialog(hDlg, IDOK);
		break;

		case ID_CANCEL:
			EndDialog(hDlg, IDCANCEL);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK RankingDloProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	char str[256];
	switch (msg){
	case WM_INITDIALOG:
		hDlgMain = hDlg;

		SetDlgItemInt(hDlg, IDC_EDIT1, "익명");

		switch (Game_Level){
		case PRIMARY:
			LoadString(g_hInstance, IDS_STRING1, str, 256);
			SetDlgItemText(hDlg, IDC_STATIC1, str);
			break;
		case INTERMEDIATE:
			LoadString(g_hInstance, IDS_STRING2, str, 256);
			SetDlgItemText(hDlg, IDC_STATIC1, str);
			break;
		case ADVANCED:
			LoadString(g_hInstance, IDS_STRING3, str, 256);
			SetDlgItemText(hDlg, IDC_STATIC1, str);
			break;
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_BUTTON1:{
			SetDlgItemText(hDlgMain, IDC_EDIT1, User_Name, 20);
			User_Name[19] = '\0';
			EndDialog(hDlg, IDOK);
			break;
		}
						 return TRUE;
		}
	}
	return FALSE;
}

LRESULT CALLBACK ResultDloProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam){
	char str[256];

	switch (msg){
	case WM_INITDIALOG:
		hDlgMain = hDlg;

		_itoa_s(Highest_Primary_Time, str, 10);
		strcat_s(str, "초");
		SetDlgItemText(hDlg, IDC_TIME1, str);
		strcat_s(str, "초");
		SetDlgItemText(hDlg, IDC_TIME2, str);
		strcat_s(str, "초");
		SetDlgItemText(hDlg, IDC_TIME3, str);

		SetDlgItemText(hDlg, IDC_NAME1, Highest_Primary_Name);
		SetDlgItemText(hDlg, IDC_NAME2, Highest_Intermediate_Name);
		SetDlgItemText(hDlg, IDC_NAME3, Highest_Advanced_Name);

		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)){
		case IDC_BUTTON1:
			strcpy(Highest_Primary_Name, "익명");
			strcpy(Highest_Intermediate_Name, "익명");
			strcpy(Highest_Advanced_Name, "익명");

			Highest_Primary_Time = 999;
			Highest_Intermediate_Time = 999;
			Highest_Advanced_Time = 999;

			_itoa(Highest_Primary_Time, str, 10);
			strcat(str, "초");
			SetDlgItemText(hDlg, IDC_TIME1, str);

			_itoa(Highest_Intermediate_Time, str, 10);
			strcat(str, "초");
			SetDlgItemText(hDlg, IDC_TIME2, str);

			_itoa(Highest_Advanced_Time, str, 10);
			strcat(str, "초");
			SetDlgItemText(hDlg, IDC_TIME3, str);

			SetDlgItemText(hDlg, IDC_NAME1, Highest_Primary_Name);
			SetDlgItemText(hDlg, IDC_NAME2, Highest_Intermediate_Name);
			SetDlgItemText(hDlg, IDC_NAME3, Highest_Advanced_Name);
			break;

		case IDOK:

			_itoa(Highest_Primary_Time, str, 10);
			WritePrivateProfileString("Ranking", "Highest_Primary_Time", str, "Mine.ini");

			_itoa(Highest_Intermediate_Time, str, 10);
			WritePrivateProfileString("Ranking", "Highest_Intermediate_Time", str, "Mine.ini");

			_itoa(Highest_Advanced_Time, str, 10);
			WritePrivateProfileString("Ranking", "Highest_Advanced_Time", str, "Mine.ini");

			WritePrivateProfileString("Ranking", "Highest_Primary_Name", Highest_Primary_Name, "Mine.ini");
			WritePrivateProfileString("Ranking", "Highest_Intermediate_Name", Highest_Intermediate_Name, "Mine.ini");
			WritePrivateProfileString("Ranking", "Highest_Advanced_Name", Highest_Advanced_Name, "Mine.ini");
			EndDialog(hDlg, IDOK);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

void CopyBlock(){
	for (int i = 0; i < Main_ROW; i++){
		for (int j = 0; j < Main_COL; j++){
			Block[i][j].IsOpen = Mask[Remain_Undo].Block[i][j].IsOpen;
			Block[i][j].State = Mask[Remain_Undo].Block[i][j].State;
		}
	}
}

void UndoBlock(){
	if (Remain_Undo >= 0){
		CopyBlock();
		Remain_Undo--;
	}
	else{
		InsertFlag = FALSE;
	}
}

void InsertBlock(){
	if (Undo_Total == 0){
		InsertFlag = FALSE;
	}
	else if (Remain_Undo == Undo_Total - 1){
		for (int i = 0; i <= Undo_Total - 2; i++){
			for (int j = 0; j < Main_ROW; j++){
				for (int k = 0; k < Main_COL; k++){
					Mask[i].Block[j][k].IsOpen = Mask[i + 1].Block[j][k].IsOpen;
					Mask[i].Block[j][k].State = Mask[i + 1].Block[j][k].State;
				}
			}
		}
		for (int j = 0; j < Main_ROW; j++){
			for (int k = 0; k < Main_COL; k++){
				Mask[Remain_Undo].Block[j][k].IsOpen = Block[j][k].IsOpen;
				Mask[Remain_Undo].Block[j][k].State = Block[j][k].State;
			}
		}
	}
	else{
		++Remain_Undo;
		for (int i = 0; i < Main_ROW; i++){
			for (int j = 0; j < Main_COL; j++){
				Mask[Remain_Undo].Block[i][j].IsOpen = Block[i][j].IsOpen;
				Mask[Remain_Undo].Block[i][j].State = Block[i][j].State;
			}
		}
		InsertFlag = TRUE;
	}
}

void StringCopy(char* Name){
	for (int i = 0; i < 19; i++){
		Name[i] = User_Name[i];
	}

	Name[19] = '\0';
}

void DeleteBlock(){
	for (int i = 0; i < Undo_Total; i++){
		for (int j = 0; j < Main_ROW; j++){
			delete[] Mask[i].Block[j];
		}
		delete[] Mask[i].Block;
	}
	for (int i = 0; i < Main_ROW; i++){
		delete[] Block[i];
	}

	delete[] Mask;
	delete[] Block;
}

int MaxMinCheck(int Number, int Kind){
	switch (Kind){
	case ROWCOL:
		if (Number < RowCol.MIN)
			Number = RowCol.MIN;
		else if (Number > RowCol.MAX)
			Number = RowCol.MAX;
		break;
	case MINE:
		if (Number < Mine.MIN)
			Number = Mine.MIN;
		else if (Number > Mine.MAX)
			Number = Mine.MAX;
		break;

	case UNDO:
		if (Number < Undo.MIN)
			Number = Undo.MIN;
		else if (Number > Undo.MAX)
			Number = Undo.MAX;
		break;
	}
	return Number;
}

void GameFailed(HWND hwnd){
	Game_End = TRUE;
	KillTimer(hwnd, 1);
}

void IntToArray(long Number, char* Array){
	int div = 100;
	int count = 0;

	if (Number >= 1000){
		Number = 999;
	}

	if (Number <= -1){
		Array[0] = '_';
		Number *= -1;
		++count;
		Number %= div;
		div = 10;

		while (count < 3){
			Array[count] = Number / div;
			Number -= div * Array[count];
			div /= 10;
			++count;
		}
	}
	else{
		while (count < 3){
			Array[count] = Number / div;
			Number -= div* Array[count];
			div /= 10;
			++count;
		}
	}
}

void Game_Win(HWND hwnd){
	int FlagCount = 0;
	Remain_Mine = Mine_Total;
	for (int i = 0; i < Main_ROW; i++){
		for (int j = 0; j < Main_COL; j++){
			if (Block[i][j].IsOpen == FLAG && Block[i][j].State == BOMB)
				FlagCount++;
			if (Block[i][j].IsOpen == FLAG)
				Remain_Mine--;
		}
	}
	if (FlagCount == Mine_Total && Remain_Mine == 0){
		Game_Sucessce = TRUE;

		for (int i = 0; i < Main_ROW; i++){
			for (int j = 0; j < Main_COL; j++){
				if (Block[i][j].IsOpen == CLOSE && Block[i][j].IsOpen != FLAG)
					Block[i][j].IsOpen = OPEN;
			}
		}
		GameFailed(hwnd);

		switch (Game_Level){
		case PRIMARY:
			if (Highest_Primary_Time >= TimeCount){
				Highest_Primary_Time = TimeCount;
				DialogBox(g_hInstance, MAKEINTERSOURCE(IDD_DIALOG1), hwnd, (DLGPROC)RankingDloProc);
				StringCopy(Highest_Primary_Name);
			}
			break;
		case INTERMEDIATE:
			if (Highest_Intermediate_Time >= TimeCount){
				Highest_Intermediate_Time = TimeCount;
				DialogBox(g_hInstance, MAKEINTERSOURCE(IDD_DIALOG1), hwnd, (DLGPROC)RankingDloProc);
				StringCopy(Highest_Intermediate_Name);
			}
			break;
		case ADVANCED:
			if (Highest_Advanced_Time >= TimeCount){
				Highest_Advanced_Time = TimeCount;
				DialogBox(g_hInstance, MAKEINTERSOURCE(IDD_DIALOG1), hwnd, (DLGPROC)RankingDloProc);
				StringCopy(Highest_Advanced_Name);
			}
			break;
		}
		DialogBox(g_hInstance, MAKEINTERSOURCE(IDD_DIALOG2), hwnd, (DLGPROC)ResultDloProc);
	}
}

void Block_Open(HWND hwnd, int Row, int Col){
	int FlagCount = 0;

	if (Block[Row][Col].IsOpen == QUESTION){
		if (Block[Row][Col].State == BOMB){
			Block[Row][Col].IsOpen = OPEN;
			Game_Sucessce = FALSE;
			GameFailed(hwnd);
			MessageBox(hwnd, "게임이 끝났습니다. \n 좀 더 열심히 해주세요.", NULL, MB_OK);
		}
		else{
			if (Block[Row][Col].State == EMPTY){
				Block[Row][Col].IsOpen = OPEN;
				Empty_Fine(Row, Col);
			}
			else
				Block[Row][Col].IsOpen = OPEN;
		}
	}
	else if (Block[Row][Col].IsOpen == CLOSE && Block[Row][Col].State == EMPTY){
		Block[Row][Col].IsOpen = OPEN;
		Empty_Fine(Row, Col);
	}
	else if (Block[Row][Col].IsOpen == CLOSE && Block[Row][Col].State >= ONE && Block[Row][Col].State <= EIGHT){
		Block[Row][Col].IsOpen = OPEN;
	}
	else if (Block[Row][Col].IsOpen == CLOSE && Block[Row][Col].State == BOMB){
		Block[Row][Col].IsOpen = OPEN;
		Game_Sucessce = FALSE;
		GameFailed(hwnd);
		MessageBox(hwnd, "게임이 끝났습니다. \n 좀 더 열심히 해주세요.", NULL, MB_OK);
	}
}

void MenuCheck(HWND hwnd, int ID){
	HMENU hMenu = GetMenu(hwnd);
	CheckMenuItem(hMenu, Menu_Checked, MF_UNCHECKED);

	Menu_Checked = ID;
	CheckMenuItem(hMenu, Menu_Checked, MF_UNCHECKED);
}