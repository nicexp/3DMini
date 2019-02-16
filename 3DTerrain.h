#ifndef _3DTERRAIN_H
#define _3DTERRAIN_H

#include "3DMesh.h"

typedef struct TERRAIN_TYP
{
	MESH mesh; //��������

	float width; //���ο��
	float length; //���γ���
	int col;	//�߶�ͼ���
	int row;	//�߶�ͼ����
	int maxheight; //��ͼ���߶�

	VECTOR4D_PTR gridNormal; //�ظ���Ƭ���ߣ����ڵ��θ���
}TERRAIN, *TERRAIN_PTR;

void GenerateTerrain(TERRAIN_PTR terrain, float width, float length, float maxheight, const char* fileheightmap, const char* filetextmap);

#endif