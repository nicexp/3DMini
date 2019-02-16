#ifndef _3DMODEL_H
#define _3DMODEL_H

#include "3DMesh.h"

#define MAX_MODEL_NUM 20

typedef struct MODEL_TYP
{
	int id;
	int attr;
	int state;
	
	char* path;			//��Դ·��
	
	VECTOR4D worldpos;	//����λ��
	VECTOR4D worldrot;  //��ת
	int	scale;			//����

	int numMesh;		//mesh����
	MESH_PTR meshes;	//����mesh
}MODEL, *MODEL_PTR;

typedef struct MODEL_MANAGER_TYP
{
	MODEL_PTR models; //����ģ��
	int num_model; //ģ������
	int hostid; //����ɫ����
}MODEL_MANAGER, *MODEL_MANAGER_PTR;

typedef struct MODEL_CFG_TYP
{
	char monster_path[256];
	char monster_skin_path[256];
	char weapon_path[256];
	char weapon_skin_path[256];
	VECTOR4D pos;
	VECTOR4D rot;
	float scale;
}MODEL_CFG, *MODEL_CFG_PTR;

void LoadAllModels(MODEL_MANAGER_PTR modelman, const char* filepath);
void Loadmodel(MODEL_PTR model);
void LoadMd2Model(MODEL_PTR model);
void ModelToWorld(MODEL_PTR model);
void SetModelWorldTransform(MODEL_PTR model, VECTOR4D_PTR worldpos, VECTOR4D_PTR worldrot);
MODEL_PTR GetHostModel(MODEL_MANAGER_PTR modelman);
void UpdateTarget(MODEL_MANAGER_PTR modelman);
#endif
