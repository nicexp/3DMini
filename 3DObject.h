#ifndef _3DOBJECT_H
#define _3DOBJECT_H

#include "3DLib1.h"
#include "3DCamera.h"
#include "3DLight.h"

#define BORDER_LENGTH 100

#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

#define OBJECT4DV2_STATE_ACTIVE           0x0001
#define OBJECT4DV2_STATE_VISIBLE          0x0002 
#define OBJECT4DV2_STATE_CULLED           0x0004

#define OBJECT4DV2_MAX_VERTICES 4096
#define OBJECT4DV2_MAX_POLYS 8192

typedef struct OBJECT4DV1_TYP
{
	int id;
	char name[64];
	int state;
	int attr;
	float avg_radius;
	float max_radius;

	POINT4D world_pos;

	VECTOR4D dir; //物体在局部坐标下的旋转角度
	VECTOR4D ux, uy, uz; //记录物体朝向的坐标轴

	int num_vertices; //物体顶点数

	POINT4D vlist_local[64]; //顶点局部坐标
	POINT4D vlist_trans[64]; //顶点转换坐标

	int num_polys; //物体的多边形数
	POLY4DV1 plist[128];
}OBJECT4DV1, *OBJECT4DV1_PTR;

//物体版本2
typedef struct OBJECT4DV2_TYP
{
	int id;
	char name[64];
	int state;
	int attr;
	float avg_radius;
	float max_radius;

	POINT4D world_pos;

	VECTOR4D dir; //物体在局部坐标下的旋转角度
	VECTOR4D ux, uy, uz; //记录物体朝向的坐标轴

	int num_vertices; //物体顶点数
	VERTEX4DTV1 vlist_local[64];//顶点
	VERTEX4DTV1 vlist_trans[64];//转换后的顶点
	//指向顶点列表的开头
	VERTEX4DTV1_PTR head_vlist_local;
	VERTEX4DTV1_PTR head_vlist_trans;
	//纹理坐标
	POINT2D tlist[3 * 12];
	int num_polys;//多边形面数
	POLY4DV2 plist[128];//多边形
}OBJECT4DV2, *OBJECT4DV2_PTR;

void InitObject(OBJECT4DV1_PTR obj);
//局部坐标转换为世界坐标
void ModelToWorldObj(OBJECT4DV1_PTR obj);
//物体背面消除
void RemoveObjBackface(OBJECT4DV1_PTR obj, POINT4D_PTR cam_pos);
//世界坐标转换相机坐标
void WorldToCameraObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mcam);
//相机坐标转换为透视坐标
void CameraToPerspectObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mper);
//透视坐标转换成屏幕坐标
void PerspectToScreenObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mscr);
//物体齐次坐标转换
void ConvertObjByW(OBJECT4DV1_PTR obj);
//物体进行矩阵变换运算
void Transform_Object(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mt, int coord_select, int bTransformDir);
//物体渲染3D流水线
void Object3DLine(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
//剔除物体
int CullObj(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags);
//重置物体状态
void ResetObjState(OBJECT4DV1_PTR obj);
//version2
//计算物体顶点法线用于光照计算
void ComputeObject2VertexNormals(OBJECT4DV2_PTR obj);
//计算物体平面法线
void ComputeObject2PolyNormals(OBJECT4DV2_PTR obj);

#endif