#ifndef _3DRENDERLIST2_H
#define _3DRENDERLIST2_H

#include "3DLib1.h"
#include "3DObject2.h"
#include "3DLight.h"

#define RENDERLIST_MAX_POLYS 32768

typedef struct RENDERLIST4DV2_TYP
{
	int state;
	int attr;

	POLYF4DV2_PTR poly_ptrs[RENDERLIST_MAX_POLYS];
	POLYF4DV2 poly_data[RENDERLIST_MAX_POLYS];

	int num_polys;
}RENDERLIST4DV2, *RENDERLIST4DV2_PTR;


//重置渲染列表
void ResetRenderlist(RENDERLIST4DV2_PTR renderlist);
//将多边形面插入渲染列表
void InsertPolyToRenderlist(RENDERLIST4DV2_PTR renderlist, POLY4DV2_PTR poly);
void InsertPolyToRenderlist(RENDERLIST4DV2_PTR renderlist, POLYF4DV2_PTR poly);
//物体插入渲染列表
void InsertObjToRenderlist(RENDERLIST4DV2_PTR renderlist, OBJECT4DV2_PTR obj, int islocal);
//渲染列表背面消除
void RemoveRendlistBackface(RENDERLIST4DV2_PTR renderlist, POINT4D_PTR cam_pos);
//渲染列表世界坐标转换为相机坐标
void WorldToCameraRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mcam);
//渲染列表相机坐标转换为透视坐标
void CameraToPerspectRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mper);
//渲染列表透视坐标转换成屏幕坐标
void PerspectToScreenRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mscr);
//渲染列表进行矩阵运算变换
void Transform_Renderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mt, int coord_select);
//渲染列表齐次坐标转换
void ConvertRenderlistByW(RENDERLIST4DV2_PTR renderlist);
//3D裁剪
void ClipPolysRenderlist(RENDERLIST4DV2_PTR renderlist, CAM4DV1_PTR cam, int cull_flags);
//光照处理
void LightRenderlistFlat(RENDERLIST4DV2_PTR renderlist, LIGHTV1_PTR lights, int max_lights);
void LightRenderlistGouraud(RENDERLIST4DV2_PTR renderlist, LIGHTV1_PTR lights, int max_lights);
//z排序
void RenderlistSortByZ(RENDERLIST4DV2_PTR renderlist);
#endif