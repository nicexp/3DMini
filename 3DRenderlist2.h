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


//������Ⱦ�б�
void ResetRenderlist(RENDERLIST4DV2_PTR renderlist);
//��������������Ⱦ�б�
void InsertPolyToRenderlist(RENDERLIST4DV2_PTR renderlist, POLY4DV2_PTR poly);
void InsertPolyToRenderlist(RENDERLIST4DV2_PTR renderlist, POLYF4DV2_PTR poly);
//���������Ⱦ�б�
void InsertObjToRenderlist(RENDERLIST4DV2_PTR renderlist, OBJECT4DV2_PTR obj, int islocal);
//��Ⱦ�б�������
void RemoveRendlistBackface(RENDERLIST4DV2_PTR renderlist, POINT4D_PTR cam_pos);
//��Ⱦ�б���������ת��Ϊ�������
void WorldToCameraRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mcam);
//��Ⱦ�б��������ת��Ϊ͸������
void CameraToPerspectRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mper);
//��Ⱦ�б�͸������ת������Ļ����
void PerspectToScreenRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mscr);
//��Ⱦ�б���о�������任
void Transform_Renderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mt, int coord_select);
//��Ⱦ�б��������ת��
void ConvertRenderlistByW(RENDERLIST4DV2_PTR renderlist);
//3D�ü�
void ClipPolysRenderlist(RENDERLIST4DV2_PTR renderlist, CAM4DV1_PTR cam, int cull_flags);
//���մ���
void LightRenderlistFlat(RENDERLIST4DV2_PTR renderlist, LIGHTV1_PTR lights, int max_lights);
void LightRenderlistGouraud(RENDERLIST4DV2_PTR renderlist, LIGHTV1_PTR lights, int max_lights);
//z����
void RenderlistSortByZ(RENDERLIST4DV2_PTR renderlist);
#endif