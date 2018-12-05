#include "3DDemo.h"

#ifdef _3DDEMO_4

#include "3DLib1.h"
#include "3DLib2.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject2.h"
#include "3DRenderlist2.h"
#include "3DShader.h"
#include "3DTexture.h"
#include "3DLight.h"
#include "3DLog.h"


#define CAMERA_DISTANCE 1750


static LIGHTV1 lights[MAX_LIGHTS]; //��Դ

static OBJECT4DV2 obj2; //����2
static RENDERLIST4DV2 renderlist; //��Ⱦ�б�
static CAM4DV1 cam;
static POINT4D cam_pos = { -123, 180, 134, 1 };
static VECTOR4D cam_dir = { 2.84, 5.62, 0, 1 };
static BITMAP_FILE bitmap;

int GameInit()
{
	//��ʼ��־
	LOG_INIT();
	//��ʼddraw
	DDraw_Init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_BPP, WINDOWED_APP);
	//��ʼ����
	DInput_Init();
	//��ʼ����
	DInput_Init_Keyboard();
	//��ʼ���
	DInput_Init_Mouse();
	if (!WINDOWED_APP)
		ShowCursor(FALSE);

	//��ʼ�����
	InitCamera(&cam,
		CAMERA_UVN,
		&cam_pos,
		&cam_dir,
		NULL,
		100, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);

	//��ʼ������
	InitObject(&obj2);
	//��ʼ����Դ
	InitAllLight(lights);

	Load_Bitmap_File(&bitmap, "test.bmp");

	return 0;
}

int GameShutdown()
{
	//�ͷ����
	DInput_Release_Mouse();
	//�ͷż���
	DInput_Release_Keyboard();
	//�ͷ�����
	DInput_Shutdown();
	//�ͷ�ddraw
	DDraw_Shutdown();
	DEBUG_LOG("game exit...");
	//�ر���־
	LOG_CLOSE();

	return 0;
}

//��Ⱦ�б����
int GameMain()
{
	//��ʼʱ��
	Start_Clock();


	DDraw_Fill_Surface(lpddsback, 0);
	DInput_Read_Keyboard();
	DInput_Read_Mouse();

	UpdateCameraPosAndDir(&cam);
	//test
	char str_buffer[256] = "";
	sprintf(str_buffer, "(campos:%f|%f|%f), (camdir:%f|%f|%f)", cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.x, cam.dir.y, cam.dir.z);
	Draw_Text_GDI(str_buffer, 10, 100, RGB(255, 255, 255), lpddsback);
	//��ʼ���任����
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//������Ⱦ�б�
	ResetRenderlist(&renderlist);
	//��������
	ResetObjState(&obj2);
	//������Ⱦ�б�
	if ((CullObj(&obj2, &cam, CULL_OBJECT_XYZ_PLANES)))
	{
		DDraw_Flip();
		Wait_Clock(30); //֡������
		if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
		{
			PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		}
		return 0;
	}
	//��������
	ModelToWorldObj(&obj2);
	//ComputeObject2PolyNormals(&obj2);//�����淨��
	ComputeObject2VertexNormals(&obj2);//���㶥�㷨��
	//LightObject2ByGouraud(&obj2, lights, 4); //gouraud��ɫ���մ���
	//���뵽��Ⱦ�б�
	InsertObjToRenderlist(&renderlist, &obj2, 0);
	//��������
	RemoveRendlistBackface(&renderlist, &cam.pos);
	//�㶨��ɫ���մ���
	//LightRenderlistFlat(&renderlist, lights, 4);
	//gouraud��ɫ���մ���
	LightRenderlistGouraud(&renderlist, lights, 4);
	//�������
	WorldToCameraRenderlist(&renderlist, &cam.mcam);
	//3D�ü�
	ClipPolysRenderlist(&renderlist, &cam, CULL_OBJECT_XYZ_PLANES);
	//͸������
	CameraToPerspectRenderlist(&renderlist, &cam.mper);
	//��Ļ����
	PerspectToScreenRenderlist(&renderlist, &cam.mscr);

	DDraw_Lock_Back_Surface();
	
	for (int poly = 0; poly < renderlist.num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = &renderlist.poly_data[poly];

		if (cur_poly->state & POLY4DV2_STATE_BACKFACE ||
			cur_poly->state & POLY4DV2_STATE_CLIPPED)
			continue;

		//�㶨��ɫ
		//ShaderFlat(cur_poly, back_buffer, back_lpitch);
		//�������ɫ
		//ShaderGouraud(cur_poly, back_buffer, back_lpitch);

		DrawTextureConstant(cur_poly, &bitmap, back_buffer, back_lpitch);
		//DrawTextureGouraud(cur_poly, &bitmap, back_buffer, back_lpitch);
		//DrawTextureFlat(cur_poly, &bitmap, back_buffer, back_lpitch);
	}

	DDraw_Unlock_Back_Surface();

	DDraw_Flip();

	Wait_Clock(30); //֡������

	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}

	return 0;
}

#endif