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


static LIGHTV1 lights[MAX_LIGHTS]; //光源

static OBJECT4DV2 obj2; //物体2
static RENDERLIST4DV2 renderlist; //渲染列表
static CAM4DV1 cam;
static POINT4D cam_pos = { -123, 180, 134, 1 };
static VECTOR4D cam_dir = { 2.84, 5.62, 0, 1 };
static BITMAP_FILE bitmap;

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
	//初始鼠标
	DInput_Init_Mouse();
	if (!WINDOWED_APP)
		ShowCursor(FALSE);

	//初始化相机
	InitCamera(&cam,
		CAMERA_UVN,
		&cam_pos,
		&cam_dir,
		NULL,
		100, 1000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);

	//初始化物体
	InitObject(&obj2);
	//初始化光源
	InitAllLight(lights);

	Load_Bitmap_File(&bitmap, "test.bmp");

	return 0;
}

int GameShutdown()
{
	//释放鼠标
	DInput_Release_Mouse();
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

//渲染列表绘制
int GameMain()
{
	//开始时钟
	Start_Clock();


	DDraw_Fill_Surface(lpddsback, 0);
	DInput_Read_Keyboard();
	DInput_Read_Mouse();
	//监听相机位置
	UpdateCameraPosAndDir(&cam);
	//UpdateObjectPosAndDir(&obj2);
	//初始化变换矩阵
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//重置渲染列表
	ResetRenderlist(&renderlist);
	//重置物体
	ResetObjState(&obj2);
	//加入渲染列表
	if ((CullObj(&obj2, &cam, CULL_OBJECT_XYZ_PLANES)))
	{
		DDraw_Flip();
		Wait_Clock(30); //帧率限制
		if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
		{
			PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		}
		return 0;
	}
	//世界坐标
	ModelToWorldObj(&obj2);
	//插入到渲染列表
	InsertObjToRenderlist(&renderlist, &obj2, 0);
	//背面消除
	RemoveRendlistBackface(&renderlist, &cam.pos);
	//相机坐标
	WorldToCameraRenderlist(&renderlist, &cam.mcam);
	//3D裁剪
	ClipPolysRenderlist(&renderlist, &cam, CULL_OBJECT_XYZ_PLANES);
	//光源转换为相机坐标空间
	TransformLights(lights, LIGHT_COUNT, &cam.mcam, LIGHT_TRANSFORM_LOCAL_TO_TRANS);
	//光照处理
	LightRenderlistFlat(&renderlist, lights, LIGHT_COUNT);//恒定着色
	//LightRenderlistGouraud(&renderlist, lights, LIGHT_COUNT);//gouraud着色
	//透视坐标
	CameraToPerspectRenderlist(&renderlist, &cam.mper);
	//屏幕坐标
	PerspectToScreenRenderlist(&renderlist, &cam.mscr);

	DDraw_Lock_Back_Surface();
	
	for (int poly = 0; poly < renderlist.num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = &renderlist.poly_data[poly];

		if (cur_poly->state & POLY4DV2_STATE_BACKFACE ||
			cur_poly->state & POLY4DV2_STATE_CLIPPED)
			continue;
		//恒定着色
		ShaderFlat(cur_poly, back_buffer, back_lpitch);
		//高洛德着色
		//ShaderGouraud(cur_poly, back_buffer, back_lpitch);

		//DrawTextureConstant(cur_poly, &bitmap, back_buffer, back_lpitch);
		//DrawTextureGouraud(cur_poly, &bitmap, back_buffer, back_lpitch);
		//DrawTextureFlat(cur_poly, &bitmap, back_buffer, back_lpitch);
	}

	DDraw_Unlock_Back_Surface();

	DDraw_Flip();

	Wait_Clock(30); //帧率限制

	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	{
		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	}

	return 0;
}

#endif