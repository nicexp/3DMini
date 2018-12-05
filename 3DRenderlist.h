#ifndef _3DRENDERLIST_H
#define _3DRENDERLIST_H

#include "3DLib1.h"
#include "3DObject.h"
#include "3DCamera.h"

#define RENDERLIST_MAX_POLYS 32768

typedef struct RENDERLIST4DV1_TYP
{
	int state;
	int attr;

	POLYF4DV1_PTR poly_ptrs[RENDERLIST_MAX_POLYS];
	POLYF4DV1 poly_data[RENDERLIST_MAX_POLYS];

	int num_polys;
}RENDERLIST4DV1, *RENDERLIST4DV1_PTR;


//������Ⱦ�б�
void ResetRenderlist(RENDERLIST4DV1_PTR renderlist);
//��������������Ⱦ�б�
void InsertPolyToRenderlist(RENDERLIST4DV1_PTR renderlist, POLY4DV1_PTR poly);
//���������Ⱦ�б�
void InsertObjToRenderlist(RENDERLIST4DV1_PTR renderlist, OBJECT4DV1_PTR obj, int islocal);
//��Ⱦ�б�������
void RemoveRendlistBackface(RENDERLIST4DV1_PTR renderlist,POINT4D_PTR cam_pos);
//��Ⱦ�б���������ת��Ϊ�������
void WorldToCameraRenderlist(RENDERLIST4DV1_PTR renderlist,MATRIX4X4_PTR mcam);
//��Ⱦ�б��������ת��Ϊ͸������
void CameraToPerspectRenderlist(RENDERLIST4DV1_PTR renderlist,MATRIX4X4_PTR mper);
//��Ⱦ�б�͸������ת������Ļ����
void PerspectToScreenRenderlist(RENDERLIST4DV1_PTR renderlist,MATRIX4X4_PTR mscr);
//��Ⱦ�б�3D��ˮ��
void Renderlist3DLine(RENDERLIST4DV1_PTR renderlist, CAM4DV1_PTR cam);
//��Ⱦ�б���о�������任
void Transform_Renderlist(RENDERLIST4DV1_PTR renderlist, MATRIX4X4_PTR mt, int coord_select);
//��Ⱦ�б��������ת��
void ConvertRenderlistByW(RENDERLIST4DV1_PTR renderlist);

#endif