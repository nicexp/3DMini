#include "3DMesh.h"
#include "3DBitmap.h"
#include "3DModel.h"

ANIMATION animations[ANIM_STATE_NUM] = {
	{ 0, 39, 1 },	//ANIM_STATE_STANDING_IDLE
	{ 40, 45, 3 },	//ANIM_STATE_RUN
	{ 46, 53, 2 },	//ANIM_STATE_ATTACK
	{ 54, 57, 0 },	//ANIM_STATE_PAIN_1
	{ 58, 61, 0 },	//ANIM_STATE_PAIN_2
	{ 62, 65, 0 },	//ANIM_STATE_PAIN_3
	{ 66, 71, 1 },	//ANIM_STATE_JUMP
	{ 72, 83, 0 },	//ANIM_STATE_FLIP
	{ 84, 94, 0 },	//ANIM_STATE_SALUTE
	{ 95, 111, 0 },	//ANIM_STATE_TAUNT
	{ 112, 122, 0 },	//ANIM_STATE_WAVE
	{ 123, 134, 0 }, //ANIM_STATE_POINT
	{ 135, 153, 0 }, //ANIM_STATE_CROUCH_STAND
	{ 154, 159, 0 }, //ANIM_STATE_CROUCH_WALK
	{ 160, 168, 0 }, //ANIM_STATE_CROUCH_ATTACK
	{ 169, 172, 0 }, //ANIM_STATE_CROUCH_PAIN
	{ 173, 177, 0 }, //ANIM_STATE_CROUCH_DEATH
	{ 178, 183, 0 }, //ANIM_STATE_DEATH_BACK
	{ 184, 189, 0 }, //ANIM_STATE_DEATH_FORWARD
	{ 190, 197, 0 }, //ANIM_STATE_DEATH_SLOW
};
//缓存表，避免每一帧都重新计算初始帧与结束帧
ANIMATION animtable[ANIM_STATE_NUM] =
{
	{ 0, 39, 0 },	//ANIM_STATE_STANDING_IDLE
	{ 40, 45, 0 },	//ANIM_STATE_RUN
	{ 46, 53, 0 },	//ANIM_STATE_ATTACK
	{ 54, 57, 0 },	//ANIM_STATE_PAIN_1
	{ 58, 61, 0 },	//ANIM_STATE_PAIN_2
	{ 62, 65, 0 },	//ANIM_STATE_PAIN_3
	{ 66, 71, 0 },	//ANIM_STATE_JUMP
	{ 72, 83, 0 },	//ANIM_STATE_FLIP
	{ 84, 94, 0 },	//ANIM_STATE_SALUTE
	{ 95, 111, 0 },	//ANIM_STATE_TAUNT
	{ 112, 122, 0 },	//ANIM_STATE_WAVE
	{ 123, 134, 0 }, //ANIM_STATE_POINT
	{ 135, 153, 0 }, //ANIM_STATE_CROUCH_STAND
	{ 154, 159, 0 }, //ANIM_STATE_CROUCH_WALK
	{ 160, 168, 0 }, //ANIM_STATE_CROUCH_ATTACK
	{ 169, 172, 0 }, //ANIM_STATE_CROUCH_PAIN
	{ 173, 177, 0 }, //ANIM_STATE_CROUCH_DEATH
	{ 178, 183, 0 }, //ANIM_STATE_DEATH_BACK
	{ 184, 189, 0 }, //ANIM_STATE_DEATH_FORWARD
	{ 190, 197, 0 }, //ANIM_STATE_DEATH_SLOW
};

void InitAnimTable()
{
	for (int i = 0; i < ANIM_STATE_NUM; i++)
	{
		int tmpframe = 0;
		for (int j = 0; j < i; j++)
		{
			ANIMATION_PTR anim = &animations[j];
			tmpframe += (anim->endframe - anim->startframe) * anim->interpolation + (anim->endframe - anim->startframe + 1);
		}
		ANIMATION_PTR animt = &animtable[i];
		animt->startframe = tmpframe;
		animt->endframe = (animt->startframe-1) + (animations[i].endframe - animations[i].startframe) * animations[i].interpolation + (animations[i].endframe - animations[i].startframe + 1);
	}
}

void InitMesh(MESH_PTR mesh)
{
	mesh->id = 0;
	mesh->attr = 0;
	mesh->state = 0;
	SET_BIT(mesh->state, MESH_STATE_ACTIVE);
	SET_BIT(mesh->state, MESH_STATE_VISIBLE);

	VECTOR4D_INITXYZ(&mesh->ux, 1, 0, 0);
	VECTOR4D_INITXYZ(&mesh->uy, 0, 1, 0);
	VECTOR4D_INITXYZ(&mesh->uz, 0, 0, 1);

	mesh->animate_state = ANIM_STATE_NONE;
	mesh->animate_mode = ANIM_MODE_SINGLE;
	mesh->animate_switch = 1;
}

#ifdef ASSIMP_SUPPORT
void LoadMesh(aiMesh* mesh, const aiScene* scene, MESH_PTR resMesh)
{
	if (mesh->mNumVertices == 0)
		return;

	const char *ext = NULL;
	const char ch = '.';
	char skinpath[256];
	GetCurrentDirectory(256, skinpath);
	//初始化mesh
	InitMesh(resMesh);
	//更新mesh
	resMesh->num_vertices = mesh->mNumVertices;
	resMesh->total_vertices = mesh->mNumVertices;
	resMesh->cur_frame = 0;
	resMesh->num_frames = 1;
	resMesh->vlist_local = (VERTEX4DTV1_PTR)malloc(mesh->mNumVertices * sizeof(VERTEX4DTV1));
	resMesh->vlist_trans = (VERTEX4DTV1_PTR)malloc(mesh->mNumVertices * sizeof(VERTEX4DTV1));
	resMesh->tlist = (POINT2D_PTR)malloc(mesh->mNumVertices * sizeof(POINT2D));
	//先更新纹理，根据纹理长度来决定面纹理坐标
	if (mesh->mMaterialIndex >= 0)
	{
		aiString str;

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);

		resMesh->texture = (BITMAP_IMG_PTR)malloc(sizeof(BITMAP_IMG));

		ext = strrchr(str.C_Str(), ch);
		strcat(skinpath, str.C_Str());
		if (!strcmp(ext + 1, "bmp"))
			Load_Bitmap_File(resMesh->texture, skinpath);
		else if (!strcmp(ext + 1, "jpg"))
			Load_Jpeg_File(resMesh->texture, skinpath);
		else if (!strcmp(ext + 1, "png"))
			Load_Png_File(resMesh->texture, skinpath);
		else
			DEBUG_LOG("texture skin is not bmp,jpg,png");
	}
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		//坐标
		resMesh->vlist_local[i].x = mesh->mVertices[i].x * MODEL_POS_TRANS_UNIT;
		resMesh->vlist_local[i].y = mesh->mVertices[i].y * MODEL_POS_TRANS_UNIT;
		resMesh->vlist_local[i].z = mesh->mVertices[i].z * MODEL_POS_TRANS_UNIT;
		resMesh->vlist_local[i].w = 1;
		//DEBUG_LOG("POS:%f, %f, %f", resMesh->vlist_local[i].x, resMawocal[i].y, resMesh->vlist_local[i].z)
		//颜色
		resMesh->vlist_local[i].i = _RGBTOINT(255, 255, 255);
		//法线
		resMesh->vlist_local[i].nx = mesh->mNormals[i].x;
		resMesh->vlist_local[i].ny = mesh->mNormals[i].y;
		resMesh->vlist_local[i].nz = mesh->mNormals[i].z;
		resMesh->vlist_local[i].nw = 1;
		//纹理坐标
		if (mesh->mTextureCoords[0])
		{
			resMesh->tlist[i].x = mesh->mTextureCoords[0][i].x * (resMesh->texture->width - 1);
			resMesh->tlist[i].y = mesh->mTextureCoords[0][i].y * (resMesh->texture->height - 1);
			/*if (mesh->mTextureCoords[0][i].x > 1.0)
			{
				resMesh->tlist[i].x = resMesh->texture->width - 1;
				DEBUG_LOG("ERR x:%f", mesh->mTextureCoords[0][i].x);
			}
			if (mesh->mTextureCoords[0][i].y > 1.0)
			{
				resMesh->tlist[i].y = resMesh->texture->height - 1;
				DEBUG_LOG("ERR y:%f", mesh->mTextureCoords[0][i].y);
			}*/
		}
	}
	//更新面
	resMesh->num_polys = mesh->mNumFaces;
	resMesh->plist = (POLY4DV2_PTR)malloc(mesh->mNumFaces * sizeof(POLY4DV2));
	for (int k = 0; k < mesh->mNumFaces; k++)
	{
		resMesh->plist[k].attr = 0;
		resMesh->plist[k].state = 0;
		resMesh->plist[k].color = _RGBTOINT(255, 255, 255);
		resMesh->plist[k].texture = resMesh->texture;

		SET_BIT(resMesh->plist[k].state, POLY4DV2_STATE_ACTIVE);
		SET_BIT(resMesh->plist[k].attr, POLY4DV2_ATTR_SHAD_MODE_TEXTURE);

		resMesh->plist[k].vlist = resMesh->vlist_local;
		resMesh->plist[k].tlist = resMesh->tlist;

		aiFace face = mesh->mFaces[k];
		for (int idx = 0; idx < 3; idx++)
		{
			resMesh->plist[k].vert[idx] = face.mIndices[idx];
			resMesh->plist[k].text[idx] = face.mIndices[idx];
		}

		ComputePolyNormals(&resMesh->plist[k]);
	}
}
#endif

//mesh坐标转换
void TransformMesh(MESH_PTR mesh, MATRIX4X4_PTR mt, int coord_select, int bNormal, int bTransformDir)
{
	MATRIX4X4 mr;//去除平移操作，用于法线向量
	memcpy((void*)&mr, (void*)mt, sizeof(MATRIX4X4));
	mr.M30 = mr.M31 = mr.M32 = 0;

	switch (coord_select)
	{
	case TRANSFORM_TRANS_ONLY:
	{
								 for (int vertex = 0; vertex < mesh->num_vertices; vertex++)
								 {
									 POINT4D presult;
									 Mat_Mul_VECTOR4D_4X4(&mesh->vlist_trans[vertex].v, mt, &presult);
									 VECTOR4D_COPY(&mesh->vlist_trans[vertex].v, &presult);
									 if (bNormal)
									 {
										 Mat_Mul_VECTOR4D_4X4(&mesh->vlist_trans[vertex].n, &mr, &presult);
										 VECTOR4D_COPY(&mesh->vlist_trans[vertex].n, &presult);
									 }
								 }
								 break;
	}
	case TRANSFORM_LOCAL_TO_TRANS:
	{
									 for (int vertex = 0; vertex < mesh->num_vertices; vertex++)
									 {
										 POINT4D presult;
										 Mat_Mul_VECTOR4D_4X4(&mesh->vlist_local[mesh->cur_frame*mesh->num_vertices+vertex].v, mt, &presult);
										 VECTOR4D_COPY(&mesh->vlist_trans[vertex].v, &presult);
										 if (bNormal)
										 {
											 Mat_Mul_VECTOR4D_4X4(&mesh->vlist_local[vertex].n, &mr, &presult);
											 VECTOR4D_COPY(&mesh->vlist_trans[vertex].n, &presult);
										 }
									 }
									 break;
	}
	default:
		break;
	}

	//旋转物体朝向
	if (bTransformDir)
	{
		VECTOR4D vresult;
		Mat_Mul_VECTOR4D_4X4(&mesh->ux, mt, &vresult);
		VECTOR4D_COPY(&mesh->ux, &vresult);
		Mat_Mul_VECTOR4D_4X4(&mesh->uy, mt, &vresult);
		VECTOR4D_COPY(&mesh->uy, &vresult);
		Mat_Mul_VECTOR4D_4X4(&mesh->uz, mt, &vresult);
		VECTOR4D_COPY(&mesh->uz, &vresult);
	}
}

void MeshToWorld(MESH_PTR mesh, VECTOR4D_PTR worldpos, VECTOR4D_PTR worldrot)
{
	VECTOR4D empty = { 0, 0, 0, 1 };
	if (!worldpos)
		worldpos = &empty;
	if (!worldrot)
		worldrot = &empty;

	//旋转
	MATRIX4X4 my_inv;

	double theta_y = worldrot->y / 180 * PI;
	double cos_theta = cos(theta_y);
	double sin_theta = sin(theta_y);
	Mat_Init_4X4(&my_inv, cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);
	TransformMesh(mesh, &my_inv, TRANSFORM_LOCAL_TO_TRANS, true, true);
	//平移
	MATRIX4X4 mt = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		worldpos->x, worldpos->y, worldpos->z, 1
	};
	TransformMesh(mesh, &mt, TRANSFORM_TRANS_ONLY, true, false);
}

//剔除网格模型
int CullMesh(MESH_PTR mesh, CAM4DV1_PTR cam, int cull_flags)
{
	POINT4D sphere_pos; //包围球坐标

	//将包围球转换相机坐标
	Mat_Mul_VECTOR4D_4X4(&mesh->world_pos[mesh->cur_frame], &cam->mcam, &sphere_pos);

	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		if ((sphere_pos.z - mesh->max_radius[mesh->cur_frame] > cam->far_clip_z)
			|| (sphere_pos.z + mesh->max_radius[mesh->cur_frame] < cam->near_clip_z))
		{
			SET_BIT(mesh->state, MESH_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		float z_test = (cam->viewplane_width / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.x - mesh->max_radius[mesh->cur_frame] > z_test)
			|| (sphere_pos.x + mesh->max_radius[mesh->cur_frame] < -z_test))
		{
			SET_BIT(mesh->state, MESH_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		float z_test = (cam->viewplane_height / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.y - mesh->max_radius[mesh->cur_frame] > z_test)
			|| (sphere_pos.y + mesh->max_radius[mesh->cur_frame] < -z_test))
		{
			SET_BIT(mesh->state, MESH_STATE_CULLED);
			return 1;
		}
	}

	return 0;
}

//重置物体状态
void ResetMeshState(MESH_PTR mesh)
{
	RESET_BIT(mesh->state, MESH_STATE_CULLED);

	for (int poly = 0; poly < mesh->num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &mesh->plist[poly];

		RESET_BIT(cur_poly->state, POLY4DV2_STATE_CLIPPED);
		RESET_BIT(cur_poly->state, POLY4DV2_STATE_BACKFACE);
	}
}

void SetMeshAnimStateAndMode(MESH_PTR mesh, int state, int mode)
{
	if (state == ANIM_STATE_NONE)
	{
		mesh->animate_state = ANIM_STATE_NONE;
		mesh->animate_mode = ANIM_MODE_SINGLE;
		mesh->animate_switch = 1;
		mesh->cur_frame = 0;
	}
	else
	{
		if (mesh->animate_state != state && mesh->animate_switch == 1)
		{
			mesh->animate_state = state;
			mesh->animate_mode = mode;
			mesh->animate_switch = (mode == ANIM_MODE_LOOP) ? 1 : 0;
			mesh->cur_frame = animtable[state].startframe;
		}
	}
}

void PlayMeshAnim(MESH_PTR mesh)
{
	if (mesh->animate_state == ANIM_STATE_NONE)
		return;
	
	ANIMATION_PTR anim = &animtable[mesh->animate_state];

	if (mesh->cur_frame < anim->endframe)
	{
		mesh->cur_frame++;
	}
	else
	{
		if (mesh->animate_mode == ANIM_MODE_LOOP)
			mesh->cur_frame = anim->startframe;
		else
		{
			SetMeshAnimStateAndMode(mesh, ANIM_STATE_NONE, ANIM_MODE_SINGLE);
		}
	}
}

void MeshStateByInput(MESH_PTR mesh)
{
	if (KEY_DOWN(0x57) || keyboard_state[DIK_W])
	{
		float theta = (360.0 - mesh->parent->worldrot.y) / 180 * PI;
		float x = cos(theta) * HOST_PLAYER_SPEED;
		float z = sin(theta) * HOST_PLAYER_SPEED;
		mesh->parent->worldpos.x += x;
		mesh->parent->worldpos.z += z;
		if (mesh->parent->worldpos.x >= TERRAIN_WIDTH / 2 - 2000)
			mesh->parent->worldpos.x = TERRAIN_WIDTH / 2 - 2000;
		if (mesh->parent->worldpos.x <= -TERRAIN_WIDTH / 2 + 2000)
			mesh->parent->worldpos.x = -TERRAIN_WIDTH / 2 + 2000;
		if (mesh->parent->worldpos.z >= TERRAIN_LENGTH / 2 - 2000)
			mesh->parent->worldpos.z = TERRAIN_LENGTH / 2 - 2000;
		if (mesh->parent->worldpos.z <= -TERRAIN_LENGTH / 2 + 2000)
			mesh->parent->worldpos.z = -TERRAIN_LENGTH / 2 + 2000;
		SetMeshAnimStateAndMode(mesh, ANIM_STATE_RUN, ANIM_MODE_SINGLE);
	}
	if (KEY_DOWN(0x19) || keyboard_state[DIK_P])
	{
		SetMeshAnimStateAndMode(mesh, ANIM_STATE_ATTACK, ANIM_MODE_SINGLE);
	}
	if (KEY_DOWN(0x39) || keyboard_state[DIK_SPACE])
	{
		SetMeshAnimStateAndMode(mesh, ANIM_STATE_JUMP, ANIM_MODE_SINGLE);
	}
	if ((mouse_state.rgbButtons[1] & 0x80))
	{
		int lx = mouse_state.lX;
		float theta = ((float)(lx)) / MOUSE_TO_EULER_UNIT;
		if (mesh->parent)
		{
			MODEL_PTR model = mesh->parent;
			model->worldrot.y += theta;
			if (model->worldrot.y >= 360)
				model->worldrot.y -= 360;
			if (model->worldrot.y <= 0)
				model->worldrot.y += 360;
		}
	}
}