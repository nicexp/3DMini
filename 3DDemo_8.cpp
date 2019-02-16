#include "3DDemo.h"

#ifdef _3DDEMO_8
//md2���������Σ�����
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
#include "3DUIPanel.h"
#include "3DLog.h"

#define NUM_OBJECTS 16
#define OBJECT_SPACING  500
#define CAMERA_DISTANCE 1750


static LIGHTV1 lights[MAX_LIGHTS]; //��Դ

static RENDERLIST4DV2 renderlist; //��Ⱦ�б�
static CAM4DV1 cam;//���
static POINT4D cam_pos = { -600, 0, 0, 1 };
static VECTOR4D cam_dir = { PI / 2, 0, 0, 1 };
static ZBUFFER zbuffer;
static MODEL_MANAGER modelman;
static TERRAIN terrain;
static UI_PANEL loadingpanel;
static UI_PANEL settingpanel;
static UI_PANEL backpanel;

bool linemode = false;
bool showsetting = false;
bool showdebug = false;
bool openlight = false;

char debugBuffer[1024];

int polynum = 0;

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
	LoadLoadingPanel(&loadingpanel);
	LoadSettingPanel(&settingpanel);
	LoadBackPanel(&backpanel);
	//loading
	ShowLoading(&loadingpanel);
	//��ʼ�����
	InitCamera(&cam,
		CAMERA_UVN,
		&cam_pos,
		&cam_dir,
		NULL,
		10, 100000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//��������ģ��
	LoadAllModels(&modelman, "models.cfg");
	//���ص���
	GenerateTerrain(&terrain, TERRAIN_WIDTH, TERRAIN_LENGTH, TERRAIN_MAXHEIGHT, "\\Terrain\\heightmap.bmp", "\\Terrain\\terrainmap.bmp");
	//��ʼ����Դ
	InitAllLight(lights);
	//����1/z����
	CreateZbuffer(&zbuffer, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	//����alpha����
	CreateAlphaBuffer();
	//anim�����
	InitAnimTable();

	DEBUG_LOG("init finish!");

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
	//����
	UpdateTarget(&modelman);
	MODEL_PTR host = GetHostModel(&modelman);
	SetCameraTargetPos(&cam, &host->worldpos);
	//�������λ��
	UpdateCameraPosAndDirSimple(&cam);
	//UpdateCameraPosAndDir(&cam);
	//��ʼ���任����
	BuildMatrixCamUVN(&cam, UVN_SIMPLE);
	//BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//������Ⱦ�б�
	ResetRenderlist(&renderlist);
	//������Ⱦ�б�
	for (int i = 0; i < modelman.num_model; i++)
	{
		MODEL_PTR model = &modelman.models[i];
		SetModelWorldTransform(model, &model->worldpos, &model->worldrot);

		for (int mindex = 0; mindex < model->numMesh; mindex++)
		{
			MESH_PTR mesh = &model->meshes[mindex];
			ResetMeshState(mesh);
			if (!CullMesh(mesh, &cam, CULL_OBJECT_XYZ_PLANES))
			{
				MeshToWorld(mesh, &model->worldpos, &model->worldrot);
				InsertMeshToRenderlist(&renderlist, mesh);
			}
			if (i == modelman.hostid)
				MeshStateByInput(mesh);
			else
				SetMeshAnimStateAndMode(mesh, ANIM_STATE_STANDING_IDLE, ANIM_MODE_LOOP);
			
			PlayMeshAnim(mesh);
		}
	}
	//�������
	ResetMeshState(&terrain.mesh);
	MeshToWorld(&terrain.mesh, NULL, NULL);
	InsertMeshToRenderlist(&renderlist, &terrain.mesh);
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
	//z����
	RenderlistSortByZ(&renderlist);
	//͸������
	CameraToPerspectRenderlist(&renderlist, &cam.mper);
	//��Ļ����
	PerspectToScreenRenderlist(&renderlist, &cam.mscr);

	DDraw_Lock_Back_Surface();
	//��汳��
	ScrollBitmap(backpanel.texture, -1);
	DrawUIPanel(&backpanel, back_buffer, back_lpitch);
	//�����Ⱦ�б�
	polynum = 0;
	for (int poly = 0; poly < renderlist.num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = renderlist.poly_ptrs[poly];

		if (cur_poly->state & POLY4DV2_STATE_BACKFACE ||
			cur_poly->state & POLY4DV2_STATE_CLIPPED)
			continue;
		
		polynum++;

		//DrawRenderlistLine(cur_poly, back_buffer, back_lpitch);
		//ShaderFlat(cur_poly, back_buffer, back_lpitch);
		//ShaderGouraud(cur_poly, back_buffer, back_lpitch);
		//DrawTextureGouraud(cur_poly, back_buffer, back_lpitch);
		//DrawTextureConstant(cur_poly, back_buffer, back_lpitch);
		//DrawTextureGouraudWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstantWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);

		if (linemode)
			DrawRenderlistLine(cur_poly, back_buffer, back_lpitch);
		else
		{
			DrawTextureGouraudWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		}
	}
	//������ý���
	if (showsetting)
		DrawUIPanel(&settingpanel, back_buffer, back_lpitch);
	//debug info
	if (showdebug)
	{
		int py = WINDOW_HEIGHT - 26;
		sprintf(debugBuffer, "Hostpos:%f,%f,%f", host->worldpos.x, host->worldpos.y, host->worldpos.z);
		Draw_Text_GDI(debugBuffer, 10, py, RGB(0, 255, 0), lpddsback);
		sprintf(debugBuffer, "Hostdir:%f,%f,%f", host->worldrot.x, host->worldrot.y, host->worldrot.z);
		Draw_Text_GDI(debugBuffer, 10, py -= 12, RGB(0, 255, 0), lpddsback);
		sprintf(debugBuffer, "Campos:%f,%f,%f", cam.pos.x, cam.pos.y, cam.pos.z);
		Draw_Text_GDI(debugBuffer, 10, py -= 12, RGB(0, 255, 0), lpddsback);
		sprintf(debugBuffer, "Current Fps:%f", GetFPS());
		Draw_Text_GDI(debugBuffer, 10, py -= 12, RGB(0, 255, 0), lpddsback);
		sprintf(debugBuffer, "Total Polys Num:%d", renderlist.num_polys);
		Draw_Text_GDI(debugBuffer, 10, py -= 12, RGB(0, 255, 0), lpddsback);
		sprintf(debugBuffer, "Current Polys Num:%d", polynum);
		Draw_Text_GDI(debugBuffer, 10, py -= 12, RGB(0, 255, 0), lpddsback);
	}

	Draw_Text_GDI("S����/�ر�����", 10, 32, RGB(0, 255, 0), lpddsback);

	DDraw_Unlock_Back_Surface();
	DDraw_Flip();
	
	//Wait_Clock(30); //֡������

	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}

	if (keyboard_state[DIK_V])
	{
		linemode = !linemode;
		Wait_Clock(100);
	}
	if (keyboard_state[DIK_I])
	{
		showdebug = !showdebug;
		Wait_Clock(100);
	}
	if (keyboard_state[DIK_S])
	{
		showsetting = !showsetting;
		Wait_Clock(100);
	}
	if (keyboard_state[DIK_L])
	{
		lights[LIGHT_INFINITE].state = !lights[LIGHT_INFINITE].state;
		Wait_Clock(100);
	}
	return 0;
}

#endif