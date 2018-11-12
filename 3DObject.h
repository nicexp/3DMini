#ifndef _3DOBJECT_H
#define _3DOBJECT_H

#include "3DLib1.h"
#include "3DCamera.h"

#define BORDER_LENGTH 100

#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

class Object3D //基于顶点的列表和多边形列表的物体
{
public:
	Object3D();
	~Object3D();

public:

//field memeber
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
//mehtod:
	//初始化物体
	void InitObject();
	//局部坐标转换为世界坐标
	void ModelToWorldObj();
	//物体背面消除
	void RemoveObjBackface(POINT4D_PTR cam_pos);
	//世界坐标转换相机坐标
	void WorldToCameraObj(MATRIX4X4_PTR mcam);
	//相机坐标转换为透视坐标
	void CameraToPerspectObj(MATRIX4X4_PTR mper);
	//透视坐标转换成屏幕坐标
	void PerspectToScreenObj(MATRIX4X4_PTR mscr);
	//物体齐次坐标转换
	void ConvertObjByW();
	//物体进行矩阵变换运算
	void Transform_Object(MATRIX4X4_PTR mt, int coord_select, int bTransformDir);
	//物体渲染3D流水线
	void Object3DLine(Camera* cam);
	//剔除物体
	int CullObj(Camera* cam, int cull_flags);
	//重置物体状态
	void ResetObjState();
};

#endif