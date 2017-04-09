
#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>
#include "resource.h"

#pragma comment(lib,"DbgHelp.lib")

//---------------------------用户界面部分-------------------------------------------------

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL OnHook();
BOOL OffHook();
BOOL EnablePrivilege();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hWnd;
	MSG msg;

	// 创建窗体
	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DialogProc);
	ShowWindow(hWnd, SW_SHOW);

	// 消息循环
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

// 对话框消息出来程序
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: // 初始化对话框:
		EnablePrivilege();
		break;
	case WM_COMMAND: // 点击按钮
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ON: // 点击挂钩按钮
			OnHook();
			break;
		case IDC_BUTTON_OFF: // 点击脱钩按钮
			OffHook();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE: // 关闭窗口
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY: // 窗口销毁
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

//---------------------------------------------------钩子部分------------------------------------------------------------

typedef LRESULT(CALLBACK *pCallWndProc)(int, WPARAM, LPARAM);
HHOOK hHook;

BOOL OnHook() // 挂钩
{
	// 加载钩子dll中的假Windows消息循环
	HMODULE hDll = LoadLibrary(TEXT("hookdll.dll"));
	pCallWndProc CallWndProc = (pCallWndProc)GetProcAddress(hDll, "CallWndProc");

	//// 创建淄博热力进程
	//TCHAR szFileName[MAX_PATH];
	//ZeroMemory(&szFileName, MAX_PATH * sizeof(TCHAR));
	//GetCurrentDirectory(MAX_PATH, szFileName);
	//_tcscat_s(szFileName, TEXT("\\App\\app.exe"));
	//STARTUPINFO si;
	//ZeroMemory(&si, sizeof(si));
	//si.cb = sizeof(si);
	//PROCESS_INFORMATION pi;
	//ZeroMemory(&pi, sizeof(pi));
	//CreateProcess(szFileName, TEXT(" --enable-npapi --enable-eloamPlugin"), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	////WaitForSingleObject(pi.hProcess, INFINITE);

	// 挂钩
	hHook = SetWindowsHookExW(WH_KEYBOARD_LL, CallWndProc, GetModuleHandle(TEXT("hookdll.dll")), 0);
	int error = GetLastError();
	char message[100] = "";
	char buf[50] = "";
	_itoa_s(error, buf, 10);
	strcat_s(message, "挂钩！错误码：");
	strcat_s(message, buf);
	MessageBoxA(NULL, message, "钩子", MB_OK);
	return TRUE;
}

BOOL OffHook() // 脱钩
{
	UnhookWindowsHookEx(hHook);
	MessageBox(NULL, TEXT("脱钩成功！"), TEXT("钩子"), MB_OK);
	return TRUE;
}

// -----------------------------------------提升权限-------------------------------------------------------------------
BOOL EnablePrivilege()
{
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tkp;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);//修改进程权限
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL);//通知系统修改进程权限
		return((GetLastError() == ERROR_SUCCESS));
	}
	return TRUE;
}