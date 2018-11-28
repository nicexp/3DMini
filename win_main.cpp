#include <iostream>
using namespace std;

#include "3DLib2.h"
#include "3DDemo.h"

#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name
#define WINDOW_TITLE      "3DMini"

extern HWND main_window_handle; // save the window handle
extern HINSTANCE main_instance; // save the instance

LRESULT CALLBACK WndProc(HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			break;
		}
		case WM_PAINT:
		{
						 HDC hdc;
						 PAINTSTRUCT ps;
						 hdc = BeginPaint(hwnd, &ps);
						 EndPaint(hwnd, &ps);
						 break;
		}
		case WM_QUIT:
		{
						DEBUG_LOG("quit....");
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE PreInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wc;
	wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = WINDOW_CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindow(WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		(WINDOWED_APP ? (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION) : (WS_POPUP | WS_VISIBLE)),
		0,0,WINDOW_WIDTH,WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	main_window_handle = hwnd;
	main_instance = hInstance;

	if (WINDOWED_APP)
	{
		RECT window_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

		MoveWindow(hwnd,
			0, // x position
			0, // y position
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		ShowWindow(hwnd, SW_SHOW);
	}

	GameInit();

	SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, NULL, 0);

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		GameMain();
	}

	GameShutdown();

	SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	return msg.wParam;
}