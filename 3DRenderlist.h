#ifndef _3DRENDERLIST_H
#define _3DRENDERLIST_H

#include "3DLib1.h"
#include "3DObject.h"
#include "3DCamera.h"

#define RENDERLIST_MAX_POLYS 32768

class Renderlist
{
public:
	Renderlist();
	~Renderlist();

public:

//field member:
	int state;
	int attr;

	POLYF4DV1_PTR poly_ptrs[RENDERLIST_MAX_POLYS];
	POLYF4DV1 poly_data[RENDERLIST_MAX_POLYS];

	int num_polys;
//method:
	//重置渲染列表
	void ResetRenderlist();
	//将多边形面插入渲染列表
	void InsertPolyToRenderlist(POLY4DV1_PTR poly);
	//物体插入渲染列表
	void InsertObjToRenderlist(Object3D* obj, int islocal);
	//渲染列表背面消除
	void RemoveRendlistBackface(POINT4D_PTR cam_pos);
	//渲染列表世界坐标转换为相机坐标
	void WorldToCameraRenderlist(MATRIX4X4_PTR mcam);
	//渲染列表相机坐标转换为透视坐标
	void Renderlist::CameraToPerspectRenderlist(MATRIX4X4_PTR mper);
	//渲染列表透视坐标转换成屏幕坐标
	void PerspectToScreenRenderlist(MATRIX4X4_PTR mscr);
	//渲染列表3D流水线
	void Renderlist::Renderlist3DLine(Camera* cam);
	//渲染列表进行矩阵运算变换
	void Transform_Renderlist(MATRIX4X4_PTR mt, int coord_select);
	//渲染列表齐次坐标转换
	void ConvertRenderlistByW();
};

#endif