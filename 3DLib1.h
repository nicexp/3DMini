#ifndef _3DLIB1_H
#define _3DLIB1_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#define SET_BIT(state, mask) ((state) = ((state) | (mask)))
#define RESET_BIT(state, mask) ((state) = ((state) & (~mask)))

#define PI 3.141592653598

#define CULL_OBJECT_X_PLANE           0x0001
#define CULL_OBJECT_Y_PLANE           0x0002
#define CULL_OBJECT_Z_PLANE           0x0004
#define CULL_OBJECT_XYZ_PLANES        (CULL_OBJECT_X_PLANE | CULL_OBJECT_Y_PLANE | CULL_OBJECT_Z_PLANE)

#define POLY4DV1_STATE_ACTIVE             0x0001
#define POLY4DV1_STATE_CLIPPED            0x0002
#define POLY4DV1_STATE_BACKFACE           0x0004

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
};

//函数
void VECTOR4D_INITXYZ(VECTOR4D_PTR vt, float x, float y, float z);
void VECTOR4D_ADD(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc);
void VECTOR4D_SUB(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc);
float VECTOR4D_DOT(VECTOR4D_PTR va, VECTOR4D_PTR vb);
void VECTOR4D_CROSS(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc);
void VECTOR4D_Normalize(VECTOR4D_PTR va);
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
#endif