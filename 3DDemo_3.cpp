#include "3DDemo.h"

#ifdef _3DDEMO_3
//纹理着色
#include "3DLib1.h"
#include "3DLib2.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject2.h"
#include "3DShader.h"
#include "3DTexture.h"
#include "3DTexture2.h"
#include "3DTexture3.h"
#include "3DZbuffer.h"
#include "3DLight.h"
#include "3DLog.h"

static LIGHTV1 lights[MAX_LIGHTS]; //光源
static OBJECT4DV2 obj2; //物体
static CAM4DV1 cam;
static POINT4D cam_pos = { -80, 0, 0, 1 };
static VECTOR4D cam_dir = { 1.626647, 6.209884, 0, 1 };
static BITMAP_FILE bitmap;
static ZBUFFER zbuffer;
static int binfilter = 1;

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
		10, 8000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);
	//初始化物体
	InitObject(&obj2);
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

void test_bitmap(BITMAP_FILE_PTR bitmap, UCHAR* destbuffer, int mempitch)
{
	int color;
#ifdef WINDDOW_BPP32
	int lpitch = mempitch >> 2;
#else
	int lpitch = mempitch >> 1;
#endif

	for (int xi = 0; xi < bitmap->bitmapinfoheader.biWidth; xi++)
	{
		for (int yi = 0; yi < bitmap->bitmapinfoheader.biHeight; yi++)
		{
#ifdef WINDDOW_BPP32
			*((UINT*)destbuffer + (yi + 50) * lpitch + xi) = *((UINT*)bitmap->buffer + yi*bitmap->bitmapinfoheader.biWidth + xi);
#else

			*((USHORT*)destbuffer + (yi + 50 )* lpitch + xi) = *((USHORT*)bitmap->buffer + yi*bitmap->bitmapinfoheader.biWidth + xi);
#endif
		}
	}
}

//渲染列表绘制
int GameMain()
{
	//开始时钟
	Start_Clock();
	//重置Zbuffer
	UpdateZbuffer(&zbuffer, 0);
	//初始化后备缓冲区
	DDraw_Fill_Surface(lpddsback, 0);
	//读取键盘和鼠标数据
	DInput_Read_Keyboard();
	DInput_Read_Mouse();

	if (keyboard_state[DIK_Z])
	{
		binfilter = -binfilter;
	}

	char textbuf[128];
	sprintf(textbuf, "campos:%f, %f, %f", cam.pos.x, cam.pos.y, cam.pos.z);
	Draw_Text_GDI(textbuf, 10, 50, RGB(255, 255, 255), lpddsback);
	char buffer[1024];
	sprintf(buffer, "campos:%f,%f,%f, camdir:%f,%f,%f", cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.x, cam.dir.y, cam.dir.z);
	Draw_Text_GDI(buffer, 10, 30, RGB(255, 255, 255), lpddsback);
	DEBUG_LOG("%s", buffer);
	//更新相机位置与朝向
	UpdateCameraPosAndDir(&cam);
	//UpdateObjectPosAndDir(&obj2);
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

	POLYF4DV2 shade_face;
	//描绘
	DDraw_Lock_Back_Surface();
	for (int poly = 0; poly < obj2.num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &obj2.plist[poly];

		if (cur_poly->state & POLY4DV1_STATE_BACKFACE)
			continue;

		shade_face.color = cur_poly->color;
		shade_face.texture = cur_poly->texture;
		for (int i = 0; i < 3; i++)
		{
			VECTOR4D_COPY(&shade_face.tvlist[i].v, &cur_poly->vlist[cur_poly->vert[i]].v);
			shade_face.tvlist[i].u0 = cur_poly->tlist[cur_poly->text[i]].x;
			shade_face.tvlist[i].v0 = cur_poly->tlist[cur_poly->text[i]].y;
			shade_face.lit_color[i] = cur_poly->lit_color[i];
		}

		//DrawTextureConstantWithPerInvz(&shade_face,  back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureFlatWithPerInvz(&shade_face,  back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstantWithPerInvzAlpha(&shade_face,  back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstant(&shade_face, back_buffer, back_lpitch);
		DrawTextureGouraud(&shade_face,  back_buffer, back_lpitch);
		//DrawTextureFlat(&shade_face, back_buffer, back_lpitch);
		//DrawTextureConstantWithPerInvzBinfilter(&shade_face, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
		//DrawTextureConstantWithPerInvzBinfilter(&shade_face, back_buffer, back_lpitch, zbuffer.zbuffer, zbuffer.width);
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