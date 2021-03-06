#ifndef _3DLIB1_H
#define _3DLIB1_H

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "3DLog.h"

#define WINDOW_WIDTH      800
#define WINDOW_HEIGHT     600
#define WINDOWED_APP      1 //是否窗口化(0:全屏，1:窗口)

#define WINDDOW_BPP32 //是否32位位深 

#ifdef WINDDOW_BPP32
#define WINDOW_BPP        32 //位深
#else
#define WINDOW_BPP        16 //位深
#endif

#define SET_BIT(state, mask) ((state) = ((state) | (mask)))
#define RESET_BIT(state, mask) ((state) = ((state) & (~mask)))

#define PI 3.141592653598

#define FIXP16_SHIFT 16
#define FIXP16_ROUND_UP 0x00008000

#define ZCORRECT_SHIFT 20
#define ZBUFFER_SHIFT 28

#define BMP_SIZE 128

#define CULL_OBJECT_X_PLANE           0x0001
#define CULL_OBJECT_Y_PLANE           0x0002
#define CULL_OBJECT_Z_PLANE           0x0004
#define CULL_OBJECT_XYZ_PLANES        (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

#define POLY4DV1_STATE_ACTIVE             0x0001
#define POLY4DV1_STATE_CLIPPED            0x0002
#define POLY4DV1_STATE_BACKFACE           0x0004

#define POLY4DV2_STATE_ACTIVE             0x0001
#define POLY4DV2_STATE_CLIPPED            0x0002
#define POLY4DV2_STATE_BACKFACE           0x0004
#define POLY4DV2_STATE_LIT                0x0008

#define POLY4DV2_ATTR_SHAD_MODE_FLAT	0x0010
#define POLY4DV2_ATTR_SHAD_MODE_GOURAUD	0x0020
#define POLY4DV2_ATTR_SHAD_MODE_TEXTURE	0x0040

#define EPSILON_E3 (float)(1E-3) 
#define EPSILON_E4 (float)(1E-4) 
#define EPSILON_E5 (float)(1E-5)
#define EPSILON_E6 (float)(1E-6)

#define SWAP(a,b,t) {t=a; a=b; b=t;}
#define FCMP(a, b) (fabs((a)-(b)) < EPSILON_E3 ? 1 : 0)

#define _RGB565FROM16BIT(RGB, r, g, b) {*r = ((((RGB)>>11) & 0x1f) << 3);*g = ((((RGB)>>5) & 0x3f) << 2);*b = (((RGB) & 0x1f)<<3);}
#define _RGB888FROM32BIT(RGB, r, g, b) {*r = ((((RGB)>>16) & 0xff));*g = ((((RGB)>>8) & 0xff));*b = ((((RGB)) & 0xff));}
#define _RGBAFROM32BIT(RGBA, r, g, b, a) {*a = ((((RGB)>>24) & 0xff));*r = ((((RGB)>>16) & 0xff));*g = ((((RGB)>>8) & 0xff));*b = ((((RGB)) & 0xff));}
#define _RGB16BIT565(r, g, b) (((b >> 3)&0x1f) + (((g >> 2)&0x3f)<<5) + (((r >> 3)&0x1f)<<11))
#define _RGB32BIT888(r, g, b) (((r & 0xff) << 16) +((g & 0xff) << 8) + ((b & 0xff)) + 0xff000000)

#ifdef WINDDOW_BPP32
#define _RGBFROMINT(RGB, r, g, b) _RGB888FROM32BIT(RGB, r, g, b)
#define _RGBTOINT(r, g, b) _RGB32BIT888(r, g, b)
#else
#define _RGBFROMINT(RGB, r, g, b) _RGB565FROM16BIT(RGB, r, g, b)
#define _RGBTOINT(r, g, b) _RGB16BIT565(r, g, b)
#endif

#define ANIM_STATE_NONE			    -1 //无动画
#define ANIM_STATE_STANDING_IDLE	0 //站立
#define ANIM_STATE_RUN				1 //奔跑
#define ANIM_STATE_ATTACK			2 //攻击
#define ANIM_STATE_PAIN_1			3 //被攻击
#define ANIM_STATE_PAIN_2			4 //被攻击2
#define ANIM_STATE_PAIN_3			5 //被攻击3
#define ANIM_STATE_JUMP				6 //跳跃
#define ANIM_STATE_FLIP				7 //做手势
#define ANIM_STATE_SALUTE			8 //敬礼
#define ANIM_STATE_TAUNT			9 //嘲笑
#define ANIM_STATE_WAVE				10 //挥手致意
#define ANIM_STATE_POINT			11 //指向别人
#define ANIM_STATE_CROUCH_STAND		12 //不动时蹲下
#define ANIM_STATE_CROUCH_WALK		13 //行走时蹲下
#define ANIM_STATE_CROUCH_ATTACK	14 //蹲下时攻击
#define ANIM_STATE_CROUCH_PAIN		15 //蹲下时被攻击
#define ANIM_STATE_CROUCH_DEATH		16 //蹲下时死亡
#define ANIM_STATE_DEATH_BACK		18 //后倒死亡
#define ANIM_STATE_DEATH_FORWARD	19 //前倒死亡
#define ANIM_STATE_DEATH_SLOW		20 //缓慢死亡
#define ANIM_STATE_NUM				21 //动画数量

#define ANIM_MODE_SINGLE	0 //单次播放动画
#define ANIM_MODE_LOOP		1 //循环播放动画

#define ANIM_MAX_FRAME		198 //动画最大帧

#define MOUSE_TO_EULER_UNIT 10 //鼠标转换角度单位

#define HOST_PLAYER_SPEED 10 //主角移动速度单位


#define TERRAIN_WIDTH         16000
#define TERRAIN_LENGTH        16000
#define TERRAIN_MAXHEIGHT     1500

#define MODEL_POS_UP		  240

//灯光索引
#define LIGHT_AMBIENT	0
#define LIGHT_INFINITE	1
#define LIGHT_POINT		2
#define LIGHT_SPOT		3

//assimp支持
//#define ASSIMP_SUPPORT

//点和向量二维
typedef struct VECTOR2D_TYP
{
	union
	{
		float M[2];
		struct
		{
			float x, y;
		};
	};
} VECTOR2D, POINT2D, *VECTOR2D_PTR, *POINT2D_PTR;

//点和向量三维
typedef struct VECTOR3D_TYP
{
	union
	{
		float M[3];
		struct
		{
			float x, y, z;
		};
	};
} VECTOR3D, POINT3D, *VECTOR3D_PTR, *POINT3D_PTR;

//点和向量四维
typedef struct VECTOR4D_TYP
{
	union
	{
		float M[4];
		struct
		{
			float x, y, z, w;
		};
	};
} VECTOR4D, POINT4D, *VECTOR4D_PTR, *POINT4D_PTR;

//顶点数据结构
typedef struct VERTEX4DTV1_TYP
{
	union
	{
		float M[12];//数组方式存储
		struct
		{
			float x, y, z, w;//顶点
			float nx, ny, nz, nw; //法线
			float u0, v0;//纹理坐标
			float i;//经过光处理后的顶点颜色
			int attr;//属性
		};
		struct
		{
			POINT4D v;//顶点
			VECTOR4D n;//法线
			POINT2D t;//纹理坐标
			float i;//光处理后颜色
			int attr;//属性
		};
	};
} VERTEX4DTV1, *VERTEX4DTV1_PTR;

//4x4矩阵
typedef struct MATRIX4X4_TYP
{
	union
	{
		float M[4][4];
		struct
		{
			float M00, M01, M02, M03;
			float M10, M11, M12, M13;
			float M20, M21, M22, M23;
			float M30, M31, M32, M33;
		};
	};
} MATRIX4X4, *MATRIX4X4_PTR;

//BITMAP结构
typedef struct BITMAP_FILE_TYP
{
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	PALETTEENTRY palette[256];
	UCHAR*	buffer;
}BITMAP_FILE, *BITMAP_FILE_PTR;

//纹理位图数据
typedef struct BITMAP_IMG_TYP
{
	int attr;
	int state;

	int width;
	int height;

	int num_bytes;

	UCHAR* buffer;
}BITMAP_IMG, *BITMAP_IMG_PTR;

//多边形结构
typedef struct POLY4DV1_TYP
{
	int state;
	int attr;
	int color;

	POINT4D_PTR vlist;
	int vert[3];
} POLY4DV1, *POLY4DV1_PTR;

typedef struct POLYF4DV1_TYP
{
	int state;
	int attr;
	int color;

	POINT4D vlist[3];
	POINT4D tvlist[3];
	POLYF4DV1_TYP *next;
	POLYF4DV1_TYP *pre;
} POLYF4DV1, *POLYF4DV1_PTR;

//多边形结构2
typedef struct POLY4DV2_TYP
{
	int state;//状态
	int attr;//属性
	int color;//颜色

	unsigned int lit_color[3];//存储光处理后的颜色
	int mati;//-1表示没有材质

	VERTEX4DTV1_PTR vlist;//顶点列表
	POINT2D_PTR tlist;//纹理坐标列表
	BITMAP_IMG_PTR texture;//纹理
	int vert[3];//顶点索引
	int text[3];//纹理坐标索引
	float nlength;//法线长度
}POLY4DV2, *POLY4DV2_PTR;

//自包含多边形结构2
typedef struct POLYF4DV2_TYP
{
	int state;//状态
	int attr;//属性
	int color;//颜色
	int lit_color[3];//光照处理后的颜色

	int mati;//-1表示没有材质
	float nlength;//法线长度
	VECTOR4D normal;//多边形法线
	float avg_z;//平均z值用于简单排序
	VERTEX4DTV1 vlist[3];//顶点
	VERTEX4DTV1 tvlist[3];//变换后的顶点
	BITMAP_IMG_PTR texture;//纹理
	POLYF4DV2_TYP* next;//下一个多边形指针
	POLYF4DV2_TYP* pre;//前一个多边形指针
}POLYF4DV2, *POLYF4DV2_PTR;

//RGBA值
typedef struct RGBAV1_TYP
{
	union
	{
		unsigned char rgba_M[4];
		struct
		{
			unsigned char r, g, b, a;
		};
	};
}RGBAV1, *RGBAV1_PTR;

typedef struct ANIMATION_TYP
{
	int startframe; //开始帧
	int endframe; //结束帧
	int interpolation; //
}ANIMATION, *ANIMATION_PTR;

//物体矩阵转换类型
enum
{
	TRANSFORM_LOCAL_ONLY = 0,
	TRANSFORM_TRANS_ONLY,
	TRANSFORM_LOCAL_TO_TRANS,
};

//uvn相机模式
enum
{
	UVN_SIMPLE = 0,
	UVN_SPHERICAL,
};

//相机模型
enum
{
	CAMERA_EULER = 0,
	CAMERA_UVN,
};

//相机移动方向
enum
{
	DIRECT_FRONT = 0,
	DIRECT_AFTER,
	DIRECT_LEFT,
	DIRECT_RIGHT,
	DIRECT_UP,
	DIRECT_DOWN,
};

//函数
void VECTOR4D_INITXYZ(VECTOR4D_PTR vt, float x, float y, float z);
void VECTOR4D_ADD(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc);
void VECTOR4D_SUB(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc);
float VECTOR4D_DOT(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void VECTOR4D_CROSS(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc);
void VECTOR4D_Normalize(VECTOR4D_PTR va);
float VECTOR4D_Length(VECTOR4D_PTR va);
void VECTOR4D_COPY(VECTOR4D_PTR res, VECTOR4D_PTR src);
void VECTOR4D_DIV_BY_W(VECTOR4D_PTR va);
void Mat_IDENTITY_4X4(MATRIX4X4_PTR ma);
void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vprod);
void Mat_Mul_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mc);
void Mat_Init_4X4(MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33);

//辅助函数
inline void VERTEX4DTV1_COPY(VERTEX4DTV1_PTR vdst, VERTEX4DTV1_PTR vsrc)
{
	*vdst = *vsrc;
}

inline void VERTEX4DTV1_INIT(VERTEX4DTV1_PTR vdst, VERTEX4DTV1_PTR vsrc)
{
	*vdst = *vsrc;
}

void ComputePolyNormals(POLY4DV2_PTR face);
#endif