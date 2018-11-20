#include <iostream>
using namespace std;

#include <windows.h>
#include "3DLib1.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject.h"
#include "3DRenderlist.h"
#include "3DLog.h"

#define WINDOW_CLASS_NAME "WIN3DCLASS"  // class name
#define WINDOW_TITLE      "3DMini"

#define CAMERA_DISTANCE 1750
#define NUM_OBJECTS 16
#define OBJECT_SPACING  250
#define CAMERA_SPEED 10

static CAM4DV1 cam;
static OBJECT4DV1 obj;
static RENDERLIST4DV1 renderlist;

static POINT4D cam_pos = { 0, 0, 0, 1 };
static VECTOR4D cam_dir = { PI / 2, PI, 0, 1 };

static float view_angle_x = 90;
static float view_angle_y = 180;

void RenderlistView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, RENDERLIST4DV1_PTR renderlist, HDC* hdc);
void ObjectView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, HDC* hdc);
void SetCameraPos(POINT4D_PTR cam_pos, CAM4DV1_PTR cam, int direct, int speed);

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
						 sprintf_s(buff, "cam_pos:��%f|%f|%f) cam_dir:��%f|%f|%f) (%f)\0", cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.x, cam.dir.y, cam.dir.z, view_angle_y/180*PI);
						 TextOutA(hdc, 0, 0, buff, strlen(buff));
						 DEBUG_LOG("cam_pos:��%f|%f|%f) cam_dir:��%f|%f|%f) (%f)\0", cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.x, cam.dir.y, cam.dir.z, view_angle_y / 180 * PI);
						 //����
						 //ObjectView(&cam_pos, &cam_dir, &cam, &obj, &hdc);
						 //��Ⱦ�б�
						 RenderlistView(&cam_pos, &cam_dir, &cam, &obj, &renderlist, &hdc);

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
							   if ((view_angle_y += 1) >= 360)
								   view_angle_y = 0;
							   cam_dir.y = view_angle_y / 180 * PI;

							   SendMessage(hwnd, WM_PAINT, 0, 0);
							   break;
						   case VK_LEFT:
							   if ((view_angle_y -= 1) <= 0)
								   view_angle_y = 360;
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
//��Ⱦ�б����
void RenderlistView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam,OBJECT4DV1_PTR obj, RENDERLIST4DV1_PTR renderlist, HDC* hdc)
{
	//��ʼ�����
	InitCamera(cam,
		CAMERA_UVN,
		cam_pos,
		cam_dir,
		NULL,
		20, 8000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);
	//��ʼ���任����
	InitTransMatrix(cam);
	//������Ⱦ�б�
	ResetRenderlist(renderlist);
	//��ʼ������
	InitObject(obj);
	//������Ⱦ�б�
	for (int x = -NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; x++)
	{
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; z++)
		{
			ResetObjState(obj);

			obj->world_pos.x = x * OBJECT_SPACING + OBJECT_SPACING / 2;
			obj->world_pos.y = 0;
			obj->world_pos.z = z * OBJECT_SPACING + OBJECT_SPACING / 2;


			if (!(CullObj(obj,cam, CULL_OBJECT_XYZ_PLANES)))
			{
				ModelToWorldObj(obj);
				InsertObjToRenderlist(renderlist, obj, 0);
			}
		}
	}
	//����3d��ˮ��
	Renderlist3DLine(renderlist, cam);
	//���
	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		POLYF4DV1_PTR cur_poly = renderlist->poly_ptrs[poly];

		if ((cur_poly->state & POLY4DV1_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV1_STATE_CLIPPED))
			continue;
		DEBUG_LOG("%f,%f", cur_poly->tvlist[0].x, cur_poly->tvlist[0].y);
		MoveToEx(*hdc, cur_poly->tvlist[0].x, cur_poly->tvlist[0].y, NULL);
		LineTo(*hdc, cur_poly->tvlist[1].x, cur_poly->tvlist[1].y);
		LineTo(*hdc, cur_poly->tvlist[2].x, cur_poly->tvlist[2].y);
		LineTo(*hdc, cur_poly->tvlist[0].x, cur_poly->tvlist[0].y);
	}
}

//�������
void ObjectView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, HDC* hdc)
{
	//��ʼ�����
	InitCamera(cam,
		CAMERA_UVN,
		cam_pos,
		cam_dir,
		NULL,
		20, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//��ʼ���任����
	InitTransMatrix(cam);
	//��ʼ������
	InitObject(obj);
	ResetObjState(obj);
	//����3D��ˮ��
	Object3DLine(obj, cam);

	//���
	int v0, v1, v2 = 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR cur_poly = &obj->plist[poly];

		if (obj->plist[poly].state & POLY4DV1_STATE_BACKFACE)
			continue;

		MoveToEx(*hdc, cur_poly->vlist[cur_poly->vert[0]].x, cur_poly->vlist[cur_poly->vert[0]].y, NULL);
		LineTo(*hdc, cur_poly->vlist[cur_poly->vert[1]].x, cur_poly->vlist[cur_poly->vert[1]].y);
		LineTo(*hdc, cur_poly->vlist[cur_poly->vert[2]].x, cur_poly->vlist[cur_poly->vert[2]].y);
		LineTo(*hdc, cur_poly->vlist[cur_poly->vert[0]].x, cur_poly->vlist[cur_poly->vert[0]].y);
	}
}
//�������λ��
void SetCameraPos(POINT4D_PTR cam_pos, CAM4DV1_PTR cam, int direct, int speed)
{
	if (direct == DIRECT_FRONT)
	{
		cam_pos->x += cam->n.x * speed;
		cam_pos->z += cam->n.z * speed;
	}
	else if(direct == DIRECT_AFTER)
	{
		cam_pos->x += cam->n.x * -speed;
		cam_pos->z += cam->n.z * -speed;
	}
	else if (direct == DIRECT_LEFT)
	{
		cam_pos->x += cam->u.x * -speed;
		cam_pos->z += cam->u.z * -speed;
	}
	else if (direct == DIRECT_RIGHT)
	{
		cam_pos->x += cam->u.x * speed;
		cam_pos->z += cam->u.z * speed;
	}
}