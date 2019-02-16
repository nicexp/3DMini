#include "3DDemo.h"

#ifdef _3DDEMO_7
//assimp模型加载,md2模型加载
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


static LIGHTV1 lights[MAX_LIGHTS]; //光源

static RENDERLIST4DV2 renderlist; //渲染列表
static CAM4DV1 cam;//相机
static POINT4D cam_pos = { 370, 269, -27, 1 };
static VECTOR4D cam_dir = { 2 ,3.15, 0, 1 };
static ZBUFFER zbuffer;
static MODEL model;
static TERRAIN terrain;

bool linemode = false;

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
		1, 10000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//初始化物体
	//Loadmodel(&model);
	LoadMd2Model(&model);
	//加载地形
	GenerateTerrain(&terrain, TERRAIN_WIDTH, TERRAIN_LENGTH, TERRAIN_MAXHEIGHT, "\\Terrain\\heightmap.bmp", "\\Terrain\\terrainmap.bmp");
	//初始化光源
	InitAllLight(lights);
	//创建1/z缓存
	CreateZbuffer(&zbuffer, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	//创建alpha缓存
	CreateAlphaBuffer();
	return 0;
}

int GameShutdown()
{
	//释放alphaebuffer
	DeleteAlphabuffer();
	//释放zbuffer
	DeleteZbuffer(&zbuffer);
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
	//清理面
	DDraw_Fill_Surface(lpddsback, 0);
	//读取键盘缓存数据
	DInput_Read_Keyboard();
	//读取鼠标缓存数据
	DInput_Read_Mouse();
	//重置Zbuffer
	UpdateZbuffer(&zbuffer, 0);
	//监听相机位置
	UpdateCameraPosAndDir(&cam);
	//test
	char buffer[1024];
	sprintf(buffer, "campos:%f,%f,%f, camdir:%f,%f,%f", cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.x, cam.dir.y, cam.dir.z);
	Draw_Text_GDI(buffer, 10, 30, RGB(255, 255, 255), lpddsback);
	//初始化变换矩阵
	BuildMatrixCamUVN(&cam, UVN_SPHERICAL);
	BuildCameraToPerspectMatrix(&cam);
	BuildPerspectToScreenMatrix(&cam);
	//重置渲染列表
	ResetRenderlist(&renderlist);
	//加入渲染列表
	for (int i = 0; i < model.numMesh; i++)
	{
		MESH_PTR mesh = &model.meshes[i];
		MeshToWorld(mesh, &model.worldpos, &model.worldrot);
		InsertMeshToRenderlist(&renderlist, mesh);
	}
	//插入地形
	ResetMeshState(&terrain.mesh);
	MeshToWorld(&terrain.mesh, NULL, NULL);
	InsertMeshToRenderlist(&renderlist, &terrain.mesh);
	//InsertModelToRenderlist(&renderlist, &model_weapon);
	//背面消除
	RemoveRendlistBackface(&renderlist, &cam.pos);
	//相机坐标
	WorldToCameraRenderlist(&renderlist, &cam.mcam);
	//3D裁剪
	ClipPolysRenderlist(&renderlist, &cam, CULL_OBJECT_XYZ_PLANES);
	//光源转换为相机坐标空间
	TransformLights(lights, LIGHT_COUNT, &cam.mcam, LIGHT_TRANSFORM_LOCAL_TO_TRANS);
	//光照处理
	//LightRenderlistFlat(&renderlist, lights, LIGHT_COUNT);//恒定着色
	LightRenderlistGouraud(&renderlist, lights, LIGHT_COUNT);//gouraud着色
															 
	RenderlistSortByZ(&renderlist);//z排序
	//透视坐标
	CameraToPerspectRenderlist(&renderlist, &cam.mper);
	//屏幕坐标
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
	//Wait_Clock(30); //帧率限制
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