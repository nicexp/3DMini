#include "3DDemo.h"

#ifdef _3DDEMO_2

#include "3DLib1.h"
#include "3DLib2.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject.h"
#include "3DObject2.h"
#include "3DRenderlist.h"
#include "3DShader.h"
#include "3DLight.h"
#include "3DLog.h"

#define CAMERA_DISTANCE 500

static LIGHTV1 lights[MAX_LIGHTS]; //��Դ
static OBJECT4DV2 obj2; //����
static CAM4DV1 cam;
static POINT4D cam_pos = { -600, 0, 0, 1 };
static VECTOR4D cam_dir = { PI/2, 0, 0, 1 };

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
		20, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//��ʼ������
	InitObject(&obj2);
	//��ʼ����Դ
	InitAllLight(lights);

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
	//��ʼ���󱸻�����
	DDraw_Fill_Surface(lpddsback, 0);
	//��ȡ���̺��������
	DInput_Read_Keyboard();
	DInput_Read_Mouse();

	//�������λ���볯��
	UpdateCameraPosAndDir(&cam);
	//��ʼ���任����
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//�޳�����
	if (CullObj(&obj2, &cam, CULL_OBJECT_XYZ_PLANES))
	{
		DDraw_Flip();
		Wait_Clock(30); //֡������
		if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
		{
			PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		}
		return 0;
	}
	//��������״̬
	ResetObjState(&obj2);
	//����3D��ˮ��
	ModelToWorldObj(&obj2);//��������
	RemoveObjBackface(&obj2, &cam.pos);//��������
	//ComputeObject2PolyNormals(&obj2);//�����淨��
	//LightObject2ByFlat(&obj2, &cam, lights, 4);//�㶨��ɫ���մ���
	ComputeObject2VertexNormals(&obj2);//���㶥�㷨��
	LightObject2ByGouraud(&obj2, &cam, lights, 4); //gouraud��ɫ���մ���
	WorldToCameraObj(&obj2, &cam.mcam);//�������
	CameraToPerspectObj(&obj2, &cam.mper);//͸������
	PerspectToScreenObj(&obj2, &cam.mscr);//��Ļ����

	//���
	DDraw_Lock_Back_Surface();
	for (int poly = 0; poly < obj2.num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &obj2.plist[poly];

		if (cur_poly->state & POLY4DV1_STATE_BACKFACE)
			continue;

		//�㶨��ɫ
		//ShaderFlat(cur_poly,back_buffer, back_lpitch);
		//�������ɫ
		ShaderGouraud(cur_poly, back_buffer, back_lpitch);
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