#pragma comment(linker, "/subsystem:windows")
#pragma commet(lib, "msimg32.lib")

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "resource.h"

void Main_OnRbuttondown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags){
	Right_Button_State = TRUE;
	int Row, Col;
	int cnt = 0;
	Right_Button_Down = FALSE;


}