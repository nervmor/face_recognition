#include <windows.h>
#include "resource.h"

HWND g_hDlg = NULL;
INT_PTR nDialogRet = NULL;
UINT nEventId = 2012;
UINT_PTR nTimeId = 0;

typedef struct _escape_item
{
	CHAR szWindowName[100];
	CHAR szClassName[100];
	HWND hWnd;
}escape_item, *p_escape_item;

escape_item g_escape_items[] =
{
	{"OpenCV - Microsoft Visual Studio", NULL, NULL},
	{"OpenCV(正在运行) - Microsoft Visual Studio", NULL, NULL },
	{"face_recognition - Microsoft Visual Studio", NULL, NULL},
	{"face_recognition(正在运行) - Microsoft Visual Studio", NULL, NULL },
	{"demo - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"demo (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"demo (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"shadow - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"shadow (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"shadow (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"feature - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"feature (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"feature (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"frame - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"frame (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"frame (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"dissect - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"dissect (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"dissect (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"client - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"client (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"client (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"server - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"server (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"server (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"protect - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"protect (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"protect (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"forger - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"forger (正在运行) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"forger (正在调试) - Microsoft Visual Studio", "wndclass_desked_gsk", NULL},
	{"Kernel 'com:port=\\\\.\\pipe\\com_1,baud=115200,pipe,reconnect' - WinDbg:6.12.0002.633 X86 ", "WinDbgFrameClass", NULL}
};

VOID EnableEscape(BOOL bEnable)
{
	for (int i = 0; i != ARRAYSIZE(g_escape_items); i++)
	{
		if (g_escape_items[i].hWnd == NULL)
		{
			g_escape_items[i].hWnd = FindWindowA(g_escape_items[i].szClassName[0] != '\0' ? g_escape_items[i].szClassName : NULL, g_escape_items[i].szWindowName);
		}
		if (g_escape_items[i].hWnd)
		{
			if (!ShowWindow(g_escape_items[i].hWnd, bEnable ? SW_HIDE : SW_SHOWMAXIMIZED))
			{
				g_escape_items[i].hWnd = FindWindowA(g_escape_items[i].szClassName[0] != '\0' ? g_escape_items[i].szClassName : NULL, g_escape_items[i].szWindowName);
				ShowWindow(g_escape_items[i].hWnd, bEnable ? SW_HIDE : SW_SHOWMAXIMIZED);
			}
		}
	}
}


VOID MonEvent()
{
	if (GetAsyncKeyState(VK_OEM_3) && !GetAsyncKeyState(VK_MENU))
	{
		EnableEscape(TRUE);
	}
	if (GetAsyncKeyState(VK_OEM_3) && GetAsyncKeyState(VK_MENU))
	{
		EnableEscape(FALSE);
	}
	if (GetAsyncKeyState(VK_F6) && GetAsyncKeyState(VK_F5) && GetAsyncKeyState(VK_MENU))
	{
		ShowWindow(g_hDlg, SW_SHOWNORMAL);
	}
}




INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
	case WM_CLOSE:
		{
			EndDialog(hwndDlg, nDialogRet);
			break;
		}
	case WM_TIMER:
		if (nEventId == wParam)
		{	
			MonEvent();
		}
		break;
	case WM_INITDIALOG:
		g_hDlg = hwndDlg;
		nTimeId = SetTimer(hwndDlg, nEventId, 100, NULL);
		break;
	case WM_COMMAND:
		if (wParam == ID_HIDE)
		{
			ShowWindow(hwndDlg, SW_HIDE);
		}
		break;
	default:
		return FALSE;
	}
	return DefWindowProc( hwndDlg, uMsg, wParam, lParam );
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	HANDLE hMutex = CreateMutexA(NULL, FALSE, "Escape_");
	if (hMutex != NULL && ERROR_ALREADY_EXISTS == GetLastError())
	{
		return 0;
	}
	nDialogRet = DialogBox( hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);
	return 0;
}