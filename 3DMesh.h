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

	VECTOR4D ux, uy, uz; //记录物体朝向的坐标轴

	MODEL_PTR parent;

	VECTOR4D_PTR center_pos; //每一帧包围球中心局部位置
	VECTOR4D_PTR world_pos; //每一帧包围球中心世界位置

	int num_vertices;//顶点数
	int num_frames; //总帧数
	int total_vertices; //总顶点数
	int cur_frame; //当前帧

	int animate_state; //动画状态
	int animate_mode; //动画模式
	int animate_switch; //bool 当前动画是否可以切换状态

	VERTEX4DTV1_PTR vlist_local;//顶点
	VERTEX4DTV1_PTR vlist_trans;//转换后的顶点

	POINT2D_PTR tlist; //纹理坐标列表
	BITMAP_IMG_PTR texture;//纹理
	int num_polys;//多边形面数
	POLY4DV2_PTR plist;//多边形
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