#include <iostream>
using namespace std;

#include "3DView.h"

#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name
#define WINDOW_TITLE      "3DMini"

#define CAMERA_DISTANCE 1750
#define CAMERA_SPEED 20

static CAM4DV1 cam;
static OBJECT4DV1 obj;
static OBJECT4DV2 obj2;
static RENDERLIST4DV1 renderlist;
static LIGHTV1 lights[MAX_LIGHTS];

static POINT4D cam_pos = { -600, 0, 0, 1 };
static VECTOR4D cam_dir = { PI / 2, 0, 0, 1 };
static VECTOR4D world_pos = { -125, 0, -125, 1 };
 
static float view_angle_x = 90;
static float view_angle_y = 0;

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
						 char buff[255] = {0};
						 sprintf_s(buff, "cam_pos:（%f|%f|%f) cam_dir:（%f|%f|%f) (%f)\0", cam_pos.x, cam_pos.y, cam_pos.z, cam_dir.x, cam_dir.y, cam_dir.z, view_angle_y / 180 * PI);
						 TextOutA(hdc, 0, 0, buff, strlen(buff));
						 //物体1(线框)
						 //ObjectView(&cam_pos, &cam_dir, &cam, &obj, &hdc);
						 //物体2(填充线)
						 ObjectView2(&cam_pos, &cam_dir, &cam, &obj2, lights, &hdc);
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
		case WM_DESTROY:
		{
			LOG_CLOSE();
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
	wc.style = 0;
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
		WS_OVERLAPPEDWINDOW,
		0,0,WINDOW_WIDTH,WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	RECT window_rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

	MoveWindow(hwnd,
		0, // x position
		0, // y position
		window_rect.right - window_rect.left, // width
		window_rect.bottom - window_rect.top, // height
		FALSE);
	 
	LOG_INIT();

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);


	}

	return msg.wParam;
}
