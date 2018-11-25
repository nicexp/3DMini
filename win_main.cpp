#include <iostream>
using namespace std;

#include "3DView.h"

#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name
#define WINDOW_TITLE      "3DMini"

extern HWND main_window_handle; // save the window handle
extern HINSTANCE main_instance; // save the instance

int GameInit();
int GameShutdown();

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
						 HPEN hPenSolid;
						 HPEN hPenOld;
						 InvalidateRect(hwnd, NULL, TRUE);
						 hdc = BeginPaint(hwnd, &ps);
						 hPenSolid = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
						 hPenOld = (HPEN)SelectObject(hdc, hPenSolid);
						 //物体1(线框)
						 //ObjectView(&cam_pos, &cam_dir, &cam, &obj);
						 //物体2(填充线)
						 //ObjectView2(&cam_pos, &cam_dir, &cam, &obj2, lights, &hdc);
						 //渲染列表
						 //RenderlistView(&cam_pos, &cam_dir, &cam, &obj, &renderlist, &hdc);

						 SelectObject(hdc, hPenOld);
						 DeleteObject(hPenSolid);
						 EndPaint(hwnd, &ps);
						 break;
		}
		case WM_KEYDOWN:
		{
						   switch (wParam)
						   {
						   case VK_RIGHT:
							   if ((view_angle_y -= 1) <= 0)
								   view_angle_y = 360;
							   cam_dir.y = view_angle_y / 180 * PI;

							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case VK_LEFT:
							   if ((view_angle_y += 1) >= 360)
								   view_angle_y = 0;
							   cam_dir.y = view_angle_y / 180 * PI;

							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case VK_UP:
							   if ((view_angle_x -= 1) <= 10)
								   view_angle_x = 10;
							   cam_dir.x = view_angle_x / 180 * PI;

							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case VK_DOWN:
							   if ((view_angle_x += 1) >= 170)
								   view_angle_x = 170;
							   cam_dir.x = view_angle_x / 180 * PI;

							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case 0x57:
							   SetCameraPos(&cam_pos, &cam, DIRECT_FRONT, CAMERA_SPEED);
							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case 0x41:
							   SetCameraPos(&cam_pos, &cam, DIRECT_LEFT, CAMERA_SPEED);
							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case 0x53:
							   SetCameraPos(&cam_pos, &cam, DIRECT_AFTER, CAMERA_SPEED);
							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case 0x44:
							   SetCameraPos(&cam_pos, &cam, DIRECT_RIGHT, CAMERA_SPEED);
							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case 0x45:
							   cam_pos.y += CAMERA_SPEED;
							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case 0x51:
							   cam_pos.y -= CAMERA_SPEED;
							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   default:
							   break;
						   }
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
		//RenderlistView(&cam_pos, &cam_dir, &cam, &obj, &renderlist);
		ObjectView2(&cam_pos, &cam_dir, &cam, &obj2, lights);
		//ObjectView(&cam_pos, &cam_dir, &cam, &obj);
	}

	GameShutdown();

	SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, NULL, 0);

	return msg.wParam;
}

int GameInit()
{
	//初始日志
	LOG_INIT();
	//初始ddraw
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);
	//初始输入
	DInput_Init();
	//初始键盘
	DInput_Init_Keyboard();

	if (!WINDOWED_APP)
		ShowCursor(FALSE);

	//初始化相机
	InitCamera(&cam,
		CAMERA_UVN,
		&cam_pos,
		&cam_dir,
		NULL,
		20, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);

	//初始化物体
	InitObject(&obj2);

	return 0;
}

int GameShutdown()
{
	//释放键盘
	DInput_Release_Keyboard();
	//释放输入
	DInput_Shutdown();
	//释放ddraw
	DDraw_Shutdown();
	DEBUG_LOG("game exit...");
	//关闭日志
	LOG_CLOSE();

	return 0;
}