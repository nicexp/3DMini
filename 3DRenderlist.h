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
	//������Ⱦ�б�
	void ResetRenderlist();
	//��������������Ⱦ�б�
	void InsertPolyToRenderlist(POLY4DV1_PTR poly);
	//���������Ⱦ�б�
	void InsertObjToRenderlist(Object3D* obj, int islocal);
	//��Ⱦ�б�������
	void RemoveRendlistBackface(POINT4D_PTR cam_pos);
	//��Ⱦ�б���������ת��Ϊ�������
	void WorldToCameraRenderlist(MATRIX4X4_PTR mcam);
	//��Ⱦ�б��������ת��Ϊ͸������
	void Renderlist::CameraToPerspectRenderlist(MATRIX4X4_PTR mper);
	//��Ⱦ�б�͸������ת������Ļ����
	void PerspectToScreenRenderlist(MATRIX4X4_PTR mscr);
	//��Ⱦ�б�3D��ˮ��
	void Renderlist::Renderlist3DLine(Camera* cam);
	//��Ⱦ�б���о�������任
	void Transform_Renderlist(MATRIX4X4_PTR mt, int coord_select);
	//��Ⱦ�б��������ת��
	void ConvertRenderlistByW();
};

#endif