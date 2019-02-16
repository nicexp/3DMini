#include "3DDemo.h"

#ifdef _3DDEMO_7
//assimpģ�ͼ���,md2ģ�ͼ���
#include "3DLib1.h"
#include "3DLib2.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DModel.h"
#include "3DTerrain.h"
#include "3DRenderlist2.h"
#include "3DShader.h"
#include "3DTexture.h"
#include "3DTexture2.h"
#include "3DTexture3.h"
#include "3DLight.h"
#include "3DZbuffer.h"
#include "3DLog.h"

#define NUM_OBJECTS 16
#define OBJECT_SPACING  100
#define CAMERA_DISTANCE 1750


static LIGHTV1 lights[MAX_LIGHTS]; //��Դ

static RENDERLIST4DV2 renderlist; //��Ⱦ�б�
static CAM4DV1 cam;//���
static POINT4D cam_pos = { 370, 269, -27, 1 };
static VECTOR4D cam_dir = { 2 ,3.15, 0, 1 };
static ZBUFFER zbuffer;
static MODEL model;
static TERRAIN terrain;

bool linemode = false;

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
		1, 10000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//��ʼ������
	//Loadmodel(&model);
	LoadMd2Model(&model);
	//���ص���
	GenerateTerrain(&terrain, TERRAIN_WIDTH, TERRAIN_LENGTH, TERRAIN_MAXHEIGHT, "\\Terrain\\heightmap.bmp", "\\Terrain\\terrainmap.bmp");
	//��ʼ����Դ
	InitAllLight(lights);
	//����1/z����
	CreateZbuffer(&zbuffer, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	//����alpha����
	CreateAlphaBuffer();
	return 0;
}

int GameShutdown()
{
	//�ͷ�alphaebuffer
	DeleteAlphabuffer();
	//�ͷ�zbuffer
	DeleteZbuffer(&zbuffer);
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
	//������
	DDraw_Fill_Surface(lpddsback, 0);
	//��ȡ���̻�������
	DInput_Read_Keyboard();
	//��ȡ��껺������
	DInput_Read_Mouse();
	//����Zbuffer
	UpdateZbuffer(&zbuffer, 0);
	//�������λ��
	UpdateCameraPosAndDir(&cam);
	//test
	char buffer[1024];
	sprintf(buffer, "campos:%f,%f,%f, camdir:%f,%f,%f", cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.x, cam.dir.y, cam.dir.z);
	Draw_Text_GDI(buffer, 10, 30, RGB(255, 255, 255), lpddsback);
	//��ʼ���任����
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//������Ⱦ�б�
	ResetRenderlist(&renderlist);
	//������Ⱦ�б�
	for (int i = 0; i < model.numMesh; i++)
	{
		MESH_PTR mesh = &model.meshes[i];
		MeshToWorld(mesh, &model.worldpos, &model.worldrot);
		InsertMeshToRenderlist(&renderlist, mesh);
	}
	//�������
	ResetMeshState(&terrain.mesh);
	MeshToWorld(&terrain.mesh, NULL, NULL);
	InsertMeshToRenderlist(&renderlist, &terrain.mesh);
	//InsertModelToRenderlist(&renderlist, &model_weapon);
	//��������
	RemoveRendlistBackface(&renderlist, &cam.pos);
	//�������
	WorldToCameraRenderlist(&renderlist, &cam.mcam);
	//3D�ü�
	ClipPolysRenderlist(&renderlist, &cam, CULL_OBJECT_XYZ_PLANES);
	//��Դת��Ϊ�������ռ�
	TransformLights(lights, LIGHT_COUNT, &cam.mcam, LIGHT_TRANSFORM_LOCAL_TO_TRANS);
	//���մ���
	//LightRenderlistFlat(&renderlist, lights, LIGHT_COUNT);//�㶨��ɫ
	LightRenderlistGouraud(&renderlist, lights, LIGHT_COUNT);//gouraud��ɫ
															 
	RenderlistSortByZ(&renderlist);//z����
	//͸������
	CameraToPerspectRenderlist(&renderlist, &cam.mper);
	//��Ļ����
	PerspectToScreenRenderlist(&renderlist, &cam.mscr);

	DDraw_Lock_Back_Surface();
	for (int poly = 0; poly < renderlist.num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = renderlist.poly_ptrs[poly];

		if (cur_poly->state & POLY4DV2_STATE_BACKFACE ||
			cur_poly->state & POLY4DV2_STATE_CLIPPED)
			continue;
		//DrawRenderlistLine(cur_poly, back_buffer, back_lpitch);
		//ShaderFlat(cur_poly, back_buffer, back_lpitch);
		//ShaderGouraud(cur_poly, back_buffer, back_lpitch);
		//DrawTextureGouraud(cur_poly, back_buffer, back_lpitch);
		//DrawTextureConstant(cur_poly, back_buffer, back_lpitch);
		//DrawTextureGouraudWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstantWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
	
		if(linemode)
			DrawRenderlistLine(cur_poly, back_buffer, back_lpitch);
		else
		{
			DrawTextureConstantWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		}
	}
	DDraw_Unlock_Back_Surface();

	DDraw_Flip();
	//Wait_Clock(30); //֡������
	DEBUG_LOG("FPS:%f", GetFPS());
	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}

	if (keyboard_state[DIK_V])
	{
		linemode = !linemode;
	}

	return 0;
}

#endif