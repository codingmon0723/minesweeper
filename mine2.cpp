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
LPCSTR lpszClass = (LPCSTR)"지뢰찾기";

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
	MineNumber = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
	RedNumber = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
	Smile = LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP3));
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

// 경계선

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
				KillTimer(hwnd, 1);
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
		if (DialogBox(g_hInstance, MAKEINTRESOURCE(OPTION_DIALOG), hwnd, (DLGPROC)InfoDloProc) == IDOK){
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
			Mine_Total = MaxMinCheck(GetDlgItemInt(hDlg, IDC_MAIN_MINE, NULL, FALSE), MINE);
			Undo_Total = MaxMinCheck(GetDlgItemInt(hDlg, IDC_MAIN_UNDO, NULL, FALSE), UNDO);
		}
		EndDialog(hDlg, IDOK);
		break;

		case IDCANCEL:
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

		SetDlgItemText(hDlg, IDC_EDIT1, "익명");

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
			GetDlgItemText(hDlgMain, IDC_EDIT1, User_Name, 20);
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
	switch(Kind){
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
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)RankingDloProc);
				StringCopy(Highest_Primary_Name);
			}
			break;
		case INTERMEDIATE:
			if (Highest_Intermediate_Time >= TimeCount){
				Highest_Intermediate_Time = TimeCount;
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)RankingDloProc);
				StringCopy(Highest_Intermediate_Name);
			}
			break;
		case ADVANCED:
			if (Highest_Advanced_Time >= TimeCount){
				Highest_Advanced_Time = TimeCount;
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)RankingDloProc);
				StringCopy(Highest_Advanced_Name);
			}
			break;
		}
		DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hwnd, (DLGPROC)ResultDloProc);
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