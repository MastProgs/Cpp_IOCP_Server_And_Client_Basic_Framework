#include"stdafx.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	// ������ Ŭ���� ���
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"MyWndClass";
	if (false == RegisterClass(&wndclass)) { return -1; }

	// ������ ����
	HWND hWnd = CreateWindow(L"MyWndClass", L"WinApp", WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, hInstance, NULL);
	if (NULL == hWnd) { return -1; }
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// �������� ��� ���� - ����� ��尡 �ƴ϶�� ServerIP.txt ���Ͽ� ���� ip �ּҷ� �ٷ� ������ �ȴ�.
	g_client.Init(hWnd);

	// �޽��� ����
	MSG msg;
	/*while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/

	HACCEL hAccelTable = LoadAccelerators(hInstance, 0);

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			
		}
	}

	return msg.wParam;
}

// ������ ���ν���
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
		break;
	case WM_SIZE:
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
			break;
		default:	// ����� ��忡�� ���ܵǴ� Ű�� �Է��� ���, ������ �⺻ ���� �׽�Ʈ ����� �ϰ� �ȴ�.
#ifdef _DEBUG
			g_client.sendPacket_TEST();
#endif // _DEBUG
			return 0;
			break;
		}
		break;

	case WM_SOCKET:
		g_client.ProcessWinMessage(hWnd, uMsg, wParam, lParam);
		return 0;
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}