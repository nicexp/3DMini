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

	VECTOR4D dir; //�����ھֲ������µ���ת�Ƕ�
	VECTOR4D ux, uy, uz; //��¼���峯���������

	int num_vertices; //���嶥����

	POINT4D vlist_local[64]; //����ֲ�����
	POINT4D vlist_trans[64]; //����ת������

	int num_polys; //����Ķ������
	POLY4DV1 plist[128];
}OBJECT4DV1, *OBJECT4DV1_PTR;

//����汾2
typedef struct OBJECT4DV2_TYP
{
	int id;
	char name[64];
	int state;
	int attr;
	float avg_radius;
	float max_radius;

	POINT4D world_pos;

	VECTOR4D dir; //�����ھֲ������µ���ת�Ƕ�
	VECTOR4D ux, uy, uz; //��¼���峯���������

	int num_vertices; //���嶥����
	VERTEX4DTV1 vlist_local[64];//����
	VERTEX4DTV1 vlist_trans[64];//ת����Ķ���
	//ָ�򶥵��б�Ŀ�ͷ
	VERTEX4DTV1_PTR head_vlist_local;
	VERTEX4DTV1_PTR head_vlist_trans;
	//��������
	POINT2D tlist[3 * 12];
	int num_polys;//���������
	POLY4DV2 plist[128];//�����
}OBJECT4DV2, *OBJECT4DV2_PTR;

void InitObject(OBJECT4DV1_PTR obj);
//�ֲ�����ת��Ϊ��������
void ModelToWorldObj(OBJECT4DV1_PTR obj);
//���屳������
void RemoveObjBackface(OBJECT4DV1_PTR obj, POINT4D_PTR cam_pos);
//��������ת���������
void WorldToCameraObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mcam);
//�������ת��Ϊ͸������
void CameraToPerspectObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mper);
//͸������ת������Ļ����
void PerspectToScreenObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mscr);
//�����������ת��
void ConvertObjByW(OBJECT4DV1_PTR obj);
//������о���任����
void Transform_Object(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mt, int coord_select, int bTransformDir);
//������Ⱦ3D��ˮ��
void Object3DLine(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam);
//�޳�����
int CullObj(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags);
//��������״̬
void ResetObjState(OBJECT4DV1_PTR obj);
//version2
//�������嶥�㷨�����ڹ��ռ���
void ComputeObject2VertexNormals(OBJECT4DV2_PTR obj);
//��������ƽ�淨��
void ComputeObject2PolyNormals(OBJECT4DV2_PTR obj);

#endif