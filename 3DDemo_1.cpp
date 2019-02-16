#include "3DDemo.h"

#ifdef _3DDEMO_1
//渲框列表
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


#define NUM_OBJECTS 16
#define OBJECT_SPACING  250
#define CAMERA_DISTANCE 1750


static LIGHTV1 lights[MAX_LIGHTS]; //光源

static OBJECT4DV1 obj; //物体1
static OBJECT4DV2 obj2; //物体2
static RENDERLIST4DV1 renderlist; //渲染列表
static CAM4DV1 cam;
static POINT4D cam_pos = { 0, 0, 0, 1 };
static VECTOR4D cam_dir = { 0, 0, 0, 1 };
static int view_angle = 0;

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
	InitObject(&obj);

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

//渲染列表绘制
int GameMain()
{
	//开始时钟
	Start_Clock();


	DDraw_Fill_Surface(lpddsback, 0);
	DInput_Read_Keyboard();

	BuildCameraPosAndDir(&cam, CAMERA_DISTANCE);

	//初始化变换矩阵
	BuildMatrixCamUVN(&cam, UVN_SIMPLE);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//重置渲染列表
	ResetRenderlist(&renderlist);
	//加入渲染列表
	for (int x = -NUM_OBJECTS / 2; x < NUM_OBJECTS / 2; x++)
	{
		for (int z = -NUM_OBJECTS / 2; z < NUM_OBJECTS / 2; z++)
		{
			ResetObjState(&obj);

			obj.world_pos.x = x * OBJECT_SPACING + OBJECT_SPACING / 2;
			obj.world_pos.y = 0;
			obj.world_pos.z = z * OBJECT_SPACING + OBJECT_SPACING / 2;

			if (!(CullObj(&obj, &cam, CULL_OBJECT_XYZ_PLANES)))
			{
				ModelToWorldObj(&obj);
				InsertObjToRenderlist(&renderlist, &obj, 0);
			}
		}
	}
	//构建3d流水线
	Renderlist3DLine(&renderlist, &cam);

	DDraw_Lock_Back_Surface();
	//描绘
	for (int poly = 0; poly < renderlist.num_polys; poly++)
	{
		POLYF4DV1_PTR cur_poly = renderlist.poly_ptrs[poly];

		if ((cur_poly->state & POLY4DV1_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV1_STATE_CLIPPED))
			continue;

		Draw_Clip_Line(cur_poly->tvlist[0].x, cur_poly->tvlist[0].y,
			cur_poly->tvlist[1].x, cur_poly->tvlist[1].y, cur_poly->color,
			back_buffer, back_lpitch);

		Draw_Clip_Line(cur_poly->tvlist[1].x, cur_poly->tvlist[1].y,
			cur_poly->tvlist[2].x, cur_poly->tvlist[2].y, cur_poly->color,
			back_buffer, back_lpitch);

		Draw_Clip_Line(cur_poly->tvlist[2].x, cur_poly->tvlist[2].y,
			cur_poly->tvlist[0].x, cur_poly->tvlist[0].y, cur_poly->color,
			back_buffer, back_lpitch);
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