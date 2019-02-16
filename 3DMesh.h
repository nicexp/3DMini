#ifndef _3DMESH_H
#define _3DMESH_H

#include "3DLib2.h"
#include "3DCamera.h"
#ifdef ASSIMP_SUPPORT
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#endif

#define MESH_STATE_ACTIVE           0x0001
#define MESH_STATE_VISIBLE          0x0002 
#define MESH_STATE_CULLED           0x0004

#define MODEL_POS_TRANS_UNIT 10

typedef struct MODEL_TYP *MODEL_PTR;

typedef struct MESH_TYP
{
	int id;
	char name[64];
	int state;
	int attr;
	float avg_radius;
	float* max_radius;

	VECTOR4D ux, uy, uz; //��¼���峯���������

	MODEL_PTR parent;

	VECTOR4D_PTR center_pos; //ÿһ֡��Χ�����ľֲ�λ��
	VECTOR4D_PTR world_pos; //ÿһ֡��Χ����������λ��

	int num_vertices;//������
	int num_frames; //��֡��
	int total_vertices; //�ܶ�����
	int cur_frame; //��ǰ֡

	int animate_state; //����״̬
	int animate_mode; //����ģʽ
	int animate_switch; //bool ��ǰ�����Ƿ�����л�״̬

	VERTEX4DTV1_PTR vlist_local;//����
	VERTEX4DTV1_PTR vlist_trans;//ת����Ķ���

	POINT2D_PTR tlist; //���������б�
	BITMAP_IMG_PTR texture;//����
	int num_polys;//���������
	POLY4DV2_PTR plist;//�����
}MESH, *MESH_PTR;

#ifdef ASSIMP_SUPPORT
void LoadMesh(aiMesh* mesh, const aiScene* scene, MESH_PTR resMesh);
#endif
void InitMesh(MESH_PTR mesh);
void TransformMesh(MESH_PTR mesh, MATRIX4X4_PTR mt, int coord_select, int bNormal, int bTransformDir);
int CullMesh(MESH_PTR mesh, CAM4DV1_PTR cam, int cull_flags);
void MeshToWorld(MESH_PTR mesh, VECTOR4D_PTR worldpos, VECTOR4D_PTR worldrot);
void ResetMeshState(MESH_PTR mesh);
void SetMeshAnimStateAndMode(MESH_PTR mesh, int state, int mode);
void PlayMeshAnim(MESH_PTR mesh);
void MeshStateByInput(MESH_PTR mesh);
void InitAnimTable();
#endif