#include "3DDemo.h"

#ifdef _3DDEMO_6
//mipmap
#include "3DLib1.h"
#include "3DLib2.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject2.h"
#include "3DRenderlist2.h"
#include "3DShader.h"
#include "3DTexture.h"
#include "3DTexture2.h"
#include "3DTexture3.h"
#include "3DLight.h"
#include "3DZbuffer.h"
#include "3DMipmap.h"
#include "3DLog.h"

#define NUM_OBJECTS 16
#define OBJECT_SPACING  125
#define CAMERA_DISTANCE 1750


static LIGHTV1 lights[MAX_LIGHTS]; //��Դ

static OBJECT4DV2 obj2; //����2
static RENDERLIST4DV2 renderlist; //��Ⱦ�б�
static CAM4DV1 cam;//���
static POINT4D cam_pos = { -600, 1500, 0, 1 };
static VECTOR4D cam_dir = { 2.88, 6.26, 0, 1 };
static ZBUFFER zbuffer;

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
		10, 12000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);

	//��ʼ������
	InitObject(&obj2);
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

void test_bitmap(OBJECT4DV2_PTR obj, UCHAR* destbuffer, int mempitch)
{
	int lpitch = mempitch >> 2;
	BITMAP_IMG_PTR* mipmap = (BITMAP_IMG_PTR*)obj->texture;

	for (int i = 0; i < 8; i++)
	{
		BITMAP_IMG_PTR bitmap = mipmap[i];


		for (int xi = 0; xi < bitmap->width; xi++)
		{
			for (int yi = 0; yi < bitmap->height; yi++)
			{
				*((UINT*)destbuffer + (yi + 50 * (i + 1)) * lpitch + xi + 50 * (i + 1)) = *((UINT*)bitmap->buffer + yi*bitmap->width + xi);
			}
		}
	}
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
	UpdateObjectPosAndDir(&obj2);
	//��ʼ���任����
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//������Ⱦ�б�
	ResetRenderlist(&renderlist);
	//������Ⱦ�б�
	for (int x = -NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; x++)
	{
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; z++)
		{
			//��������
			ResetObjState(&obj2);

			obj2.world_pos.x = x * OBJECT_SPACING + OBJECT_SPACING / 2;
			obj2.world_pos.y = 0;
			obj2.world_pos.z = z * OBJECT_SPACING + OBJECT_SPACING / 2;

			if (!(CullObj(&obj2, &cam, CULL_OBJECT_XYZ_PLANES)))
			{
				ModelToWorldObj(&obj2);
				InsertObjToRenderlist(&renderlist, &obj2, 0);
			}
		}
	}
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
	for (int poly = 0; poly < renderlist.num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = renderlist.poly_ptrs[poly];

		if (cur_poly->state & POLY4DV2_STATE_BACKFACE ||
			cur_poly->state & POLY4DV2_STATE_CLIPPED)
			continue;
		OperWithMipmap(cur_poly);
		//ShaderFlat(cur_poly, back_buffer, back_lpitch);
		//ShaderGouraud(cur_poly, back_buffer, back_lpitch);
		//DrawTextureGouraud(cur_poly, &bitmap, back_buffer, back_lpitch);
		//DrawTextureConstantWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureGouraudWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureFlatWithPerInvz(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstantWithPerInvzAlpha(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstantWithPerInvzBinfilter(cur_poly, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
	}
	test_bitmap(&obj2, back_buffer, back_lpitch);
	DDraw_Unlock_Back_Surface();

	DDraw_Flip();
	//Wait_Clock(30); //֡������
	DEBUG_LOG("FPS:%f", GetFPS());
	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}

	return 0;
}

#endif