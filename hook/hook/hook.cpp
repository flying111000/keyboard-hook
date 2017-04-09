
#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>
#include "resource.h"

#pragma comment(lib,"DbgHelp.lib")

//---------------------------�û����沿��-------------------------------------------------

BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL OnHook();
BOOL OffHook();
BOOL EnablePrivilege();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hWnd;
	MSG msg;

	// ��������
	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, DialogProc);
	ShowWindow(hWnd, SW_SHOW);

	// ��Ϣѭ��
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

// �Ի�����Ϣ��������
BOOL CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: // ��ʼ���Ի���:
		EnablePrivilege();
		break;
	case WM_COMMAND: // �����ť
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ON: // ����ҹ���ť
			OnHook();
			break;
		case IDC_BUTTON_OFF: // ����ѹ���ť
			OffHook();
			break;
		default:
			break;
		}
		break;
	case WM_CLOSE: // �رմ���
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY: // ��������
		PostQuitMessage(0);
		break;
	}
	return FALSE;
}

//---------------------------------------------------���Ӳ���------------------------------------------------------------

typedef LRESULT(CALLBACK *pCallWndProc)(int, WPARAM, LPARAM);
HHOOK hHook;

BOOL OnHook() // �ҹ�
{
	// ���ع���dll�еļ�Windows��Ϣѭ��
	HMODULE hDll = LoadLibrary(TEXT("hookdll.dll"));
	pCallWndProc CallWndProc = (pCallWndProc)GetProcAddress(hDll, "CallWndProc");

	//// �����Ͳ���������
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

	// �ҹ�
	hHook = SetWindowsHookExW(WH_KEYBOARD_LL, CallWndProc, GetModuleHandle(TEXT("hookdll.dll")), 0);
	int error = GetLastError();
	char message[100] = "";
	char buf[50] = "";
	_itoa_s(error, buf, 10);
	strcat_s(message, "�ҹ��������룺");
	strcat_s(message, buf);
	MessageBoxA(NULL, message, "����", MB_OK);
	return TRUE;
}

BOOL OffHook() // �ѹ�
{
	UnhookWindowsHookEx(hHook);
	MessageBox(NULL, TEXT("�ѹ��ɹ���"), TEXT("����"), MB_OK);
	return TRUE;
}

// -----------------------------------------����Ȩ��-------------------------------------------------------------------
BOOL EnablePrivilege()
{
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tkp;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);//�޸Ľ���Ȩ��
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL);//֪ͨϵͳ�޸Ľ���Ȩ��
		return((GetLastError() == ERROR_SUCCESS));
	}
	return TRUE;
}