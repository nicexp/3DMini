#ifndef _3DLIB1_H
#define _3DLIB1_H

#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "3DLog.h"

#define WINDOW_WIDTH      800
#define WINDOW_HEIGHT     600
#define WINDOWED_APP      1 //�Ƿ񴰿ڻ�(0:ȫ����1:����)

#define WINDDOW_BPP32 //�Ƿ�32λλ�� 

#ifdef WINDDOW_BPP32
#define WINDOW_BPP        32 //λ��
#else
#define WINDOW_BPP        16 //λ��
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

//���������ά
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

//���������ά
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

//���������ά
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

//�������ݽṹ
typedef struct VERTEX4DTV1_TYP
{
	union
	{
		float M[12];//���鷽ʽ�洢
		struct
		{
			float x, y, z, w;//����
			float nx, ny, nz, nw; //����
			float u0, v0;//��������
			float i;//�����⴦���Ķ�����ɫ
			int attr;//����
		};
		struct
		{
			POINT4D v;//����
			VECTOR4D n;//����
			POINT2D t;//��������
			float i;//�⴦�����ɫ
			int attr;//����
		};
	};
} VERTEX4DTV1, *VERTEX4DTV1_PTR;

//4x4����
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

//����νṹ
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

//����νṹ2
typedef struct POLY4DV2_TYP
{
	int state;//״̬
	int attr;//����
	int color;//��ɫ

	unsigned int lit_color[3];//�洢�⴦������ɫ
	int mati;//-1��ʾû�в���

	VERTEX4DTV1_PTR vlist;//�����б�
	POINT2D_PTR tlist;//���������б�
	int vert[3];//��������
	int text[3];//������������
	float nlength;//���߳���
}POLY4DV2, *POLY4DV2_PTR;

//�԰�������νṹ2
typedef struct POLYF4DV2_TYP
{
	int state;//״̬
	int attr;//����
	int color;//��ɫ
	int lit_color[3];//���մ�������ɫ

	int mati;//-1��ʾû�в���
	float nlength;//���߳���
	VECTOR4D normal;//����η���
	float avg_z;//ƽ��zֵ���ڼ�����
	VERTEX4DTV1 vlist[3];//����
	VERTEX4DTV1 tvlist[3];//�任��Ķ���

	POLYF4DV2_TYP* next;//��һ�������ָ��
	POLYF4DV2_TYP* pre;//ǰһ�������ָ��
}POLYF4DV2, *POLYF4DV2_PTR;

//RGBAֵ
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

//BITMAP�ṹ
typedef struct BITMAP_FILE_TYP
{
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	PALETTEENTRY palette[256];
	UCHAR*	buffer;
}BITMAP_FILE, *BITMAP_FILE_PTR;

//�������ת������
enum
{
	TRANSFORM_LOCAL_ONLY = 0,
	TRANSFORM_TRANS_ONLY,
	TRANSFORM_LOCAL_TO_TRANS,
};

//uvn���ģʽ
enum
{
	UVN_SIMPLE = 0,
	UVN_SPHERICAL,
};

//���ģ��
enum
{
	CAMERA_EULER = 0,
	CAMERA_UVN,
};

//����ƶ�����
enum
{
	DIRECT_FRONT = 0,
	DIRECT_AFTER,
	DIRECT_LEFT,
	DIRECT_RIGHT,
	DIRECT_UP,
	DIRECT_DOWN,
};

//����
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

//��������
inline void VERTEX4DTV1_COPY(VERTEX4DTV1_PTR vdst, VERTEX4DTV1_PTR vsrc)
{
	*vdst = *vsrc;
}

inline void VERTEX4DTV1_INIT(VERTEX4DTV1_PTR vdst, VERTEX4DTV1_PTR vsrc)
{
	*vdst = *vsrc;
}
#endif