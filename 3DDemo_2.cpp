#include "3DDemo.h"

#ifdef _3DDEMO_2
//shader 
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

static LIGHTV1 lights[MAX_LIGHTS]; //光源
static OBJECT4DV2 obj2; //物体
static CAM4DV1 cam;
static POINT4D cam_pos = { -600, 0, 0, 1 };
static VECTOR4D cam_dir = { PI/2, 0, 0, 1 };

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
		20, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//初始化物体
	InitObject(&obj2);
	//初始化光源
	InitAllLight(lights);

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
	//初始化后备缓冲区
	DDraw_Fill_Surface(lpddsback, 0);
	//读取键盘和鼠标数据
	DInput_Read_Keyboard();
	DInput_Read_Mouse();

	//更新相机位置与朝向
	UpdateCameraPosAndDir(&cam);
	//初始化变换矩阵
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//剔除物体
	if (CullObj(&obj2, &cam, CULL_OBJECT_XYZ_PLANES))
	{
		DDraw_Flip();
		Wait_Clock(30); //帧率限制
		if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
		{
			PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		}
		return 0;
	}
	//重置物体状态
	ResetObjState(&obj2);
	//构建3D流水线
	ModelToWorldObj(&obj2);//世界坐标
	RemoveObjBackface(&obj2, &cam.pos);//消除背面
	//LightObject2ByFlat(&obj2, lights, 4);//恒定着色光照处理
	LightObject2ByGouraud(&obj2, lights, 4); //gouraud着色光照处理
	WorldToCameraObj(&obj2, &cam.mcam);//相机坐标
	CameraToPerspectObj(&obj2, &cam.mper);//透视坐标
	PerspectToScreenObj(&obj2, &cam.mscr);//屏幕坐标

	//描绘
	DDraw_Lock_Back_Surface();

	POLYF4DV2 shade_face;

	for (int poly = 0; poly < obj2.num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &obj2.plist[poly];

		if (cur_poly->state & POLY4DV1_STATE_BACKFACE)
			continue;

		shade_face.color = cur_poly->color;
		for (int i = 0; i < 3; i++)
		{
			VECTOR4D_COPY(&shade_face.tvlist[i].v, &cur_poly->vlist[cur_poly->vert[i]].v);
			shade_face.lit_color[i] = cur_poly->lit_color[i];
		}
		//恒定着色
		//ShaderFlat(&shade_face, back_buffer, back_lpitch);
		//高洛德着色
		ShaderGouraud(&shade_face, back_buffer, back_lpitch);
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