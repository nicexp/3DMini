#ifndef _3DOBJECT_H
#define _3DOBJECT_H

#include "3DLib1.h"
#include "3DCamera.h"

#define BORDER_LENGTH 100

#define OBJECT4DV1_STATE_ACTIVE           0x0001
#define OBJECT4DV1_STATE_VISIBLE          0x0002 
#define OBJECT4DV1_STATE_CULLED           0x0004

class Object3D //���ڶ�����б��Ͷ�����б�������
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

	VECTOR4D dir; //�����ھֲ������µ���ת�Ƕ�
	VECTOR4D ux, uy, uz; //��¼���峯���������

	int num_vertices; //���嶥����

	POINT4D vlist_local[64]; //����ֲ�����
	POINT4D vlist_trans[64]; //����ת������

	int num_polys; //����Ķ������
	POLY4DV1 plist[128];
//mehtod:
	//��ʼ������
	void InitObject();
	//�ֲ�����ת��Ϊ��������
	void ModelToWorldObj();
	//���屳������
	void RemoveObjBackface(POINT4D_PTR cam_pos);
	//��������ת���������
	void WorldToCameraObj(MATRIX4X4_PTR mcam);
	//�������ת��Ϊ͸������
	void CameraToPerspectObj(MATRIX4X4_PTR mper);
	//͸������ת������Ļ����
	void PerspectToScreenObj(MATRIX4X4_PTR mscr);
	//�����������ת��
	void ConvertObjByW();
	//������о���任����
	void Transform_Object(MATRIX4X4_PTR mt, int coord_select, int bTransformDir);
	//������Ⱦ3D��ˮ��
	void Object3DLine(Camera* cam);
	//�޳�����
	int CullObj(Camera* cam, int cull_flags);
	//��������״̬
	void ResetObjState();
};

#endif