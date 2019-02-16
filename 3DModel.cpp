#include "3DLog.h"
#include "3DModel.h"
#include "3DMd2.h"

#ifdef ASSIMP_SUPPORT
Assimp::Importer importer;

void Loadmodel(MODEL_PTR model)
{
	char exepath[256];
	GetCurrentDirectory(256, exepath);
	strcat(exepath, "\\Models\\nanosuit\\nanosuit.obj");
	model->path = exepath;
	//aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals
	const aiScene *scene = importer.ReadFile(model->path, aiProcess_Triangulate |aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		DEBUG_LOG("load model failed path:%s", model->path);
		return;
	}

	model->numMesh = scene->mNumMeshes;
	model->meshes = (MESH_PTR)malloc(scene->mNumMeshes * sizeof(MESH));
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[i], scene, &model->meshes[i]);
	}
}
#endif

void LoadMd2Model(MODEL_PTR model)
{
	char* path = "E:\\MyProject\\3DMini\\3DMini\\Models\\blade\\blade_red.bmp";
	model->path = "E:\\MyProject\\3DMini\\3DMini\\Models\\blade\\tris.md2";
	model->numMesh = 1;
	model->meshes = (MESH_PTR)malloc(sizeof(MESH));
	memset(model->meshes, 0, sizeof(MESH));
	LoadMd2FileToMesh(model, model->path, path, model->meshes);
	////初始化模型位置
	VECTOR4D_INITXYZ(&model->worldpos, 0, 0, 0);
	ModelToWorld(model);
}

void ModelToWorld(MODEL_PTR model)
{
	MATRIX4X4 mt = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		model->worldpos.x, model->worldpos.y, model->worldpos.z, 1
	};
	for (int i = 0; i < model->numMesh; i++)
	{
		MESH_PTR mesh = (model->meshes + i);
		TransformMesh(mesh, &mt, TRANSFORM_LOCAL_TO_TRANS, true, false);
	}
}

void SetModelWorldTransform(MODEL_PTR model, VECTOR4D_PTR worldpos, VECTOR4D_PTR worldrot)
{
	VECTOR4D_COPY(&model->worldpos, worldpos);
	VECTOR4D_COPY(&model->worldrot, worldrot);

	MATRIX4X4 my_inv;
	VECTOR4D vresult;

	double theta_y = worldrot->y / 180 * PI;
	double cos_theta = cos(theta_y);
	double sin_theta = sin(theta_y);
	Mat_Init_4X4(&my_inv, cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);

	for (int i = 0; i < model->numMesh; i++)
	{
		MESH_PTR mesh = &model->meshes[i];
		//设置每一帧的mesh的包围球中心位置
		for (int findex = 0; findex < mesh->num_frames; findex++)
		{
			Mat_Mul_VECTOR4D_4X4(&mesh->center_pos[findex], &my_inv, &vresult);
			VECTOR4D_COPY(&mesh->world_pos[findex], &vresult);
			VECTOR4D_ADD(&mesh->world_pos[findex], &model->worldpos, &mesh->world_pos[findex]);
		}
	}
}

void LoadAllModels(MODEL_MANAGER_PTR modelman, const char* filepath)
{
	modelman->models = NULL;
	modelman->num_model = 0;
	modelman->hostid = 0;

	FILE* fp = fopen(filepath, "r");
	if (!fp)
		return;

	char exepath[256], path[256];
	GetCurrentDirectory(256, exepath);
	//GetModuleFileName(NULL, exepath, 256);

	MODEL_CFG modelcfgs[MAX_MODEL_NUM];//最大配置64个模型
	float x, y, z;
	int count = 0;
	while (!feof(fp))
	{
		strcpy(modelcfgs[count].monster_path, exepath);
		strcpy(modelcfgs[count].monster_skin_path, exepath);
		strcpy(modelcfgs[count].weapon_path, exepath);
		strcpy(modelcfgs[count].weapon_skin_path, exepath);

		fscanf(fp, "%s", path);
		strcat(modelcfgs[count].monster_path, path);
		fscanf(fp, "%s", path);
		strcat(modelcfgs[count].monster_skin_path, path);
		fscanf(fp, "%s", path);
		strcat(modelcfgs[count].weapon_path, path);
		fscanf(fp, "%s", path);
		strcat(modelcfgs[count].weapon_skin_path, path);
		fscanf(fp, "%f", &x);
		fscanf(fp, "%f", &y);
		fscanf(fp, "%f", &z);
		VECTOR4D_INITXYZ(&modelcfgs[count].pos, x, y + MODEL_POS_UP, z);
		fscanf(fp, "%f", &x);
		fscanf(fp, "%f", &y);
		fscanf(fp, "%f", &z);
		VECTOR4D_INITXYZ(&modelcfgs[count].rot, x, y, z);
		fscanf(fp, "%f", &modelcfgs[count].scale);
		count++;
		if (count >= MAX_MODEL_NUM)
			break;
	}

	modelman->models = (MODEL_PTR)malloc(count * sizeof(MODEL));
	memset(modelman->models, 0, count*sizeof(MODEL));
	modelman->num_model = count;
	for (int mindex = 0; mindex < count; mindex++)
	{
		MODEL_PTR model = &modelman->models[mindex];
		MODEL_CFG_PTR cfg = &modelcfgs[mindex];
		model->id = mindex;
		model->attr = 0;
		model->state = 0;

		VECTOR4D_COPY(&model->worldpos, &cfg->pos);
		VECTOR4D_COPY(&model->worldrot, &cfg->rot);
		model->scale = cfg->scale;
		//判断有武器没
		strcpy(path, exepath);
		strcat(path, "none");
		int hasweapon = (strcmp(cfg->weapon_path, path) == 0) ? 0 : 1;

		model->numMesh = hasweapon ? 2 : 1;
		model->meshes = (MESH_PTR)malloc(model->numMesh * sizeof(MESH));
		memset(model->meshes, 0, model->numMesh * sizeof(MESH));
		LoadMd2FileToMesh(model, cfg->monster_path, cfg->monster_skin_path, &model->meshes[0]);
		if (hasweapon)
		{
			LoadMd2FileToMesh(model, cfg->weapon_path, cfg->weapon_skin_path, &model->meshes[1]);
		}
	
		SetModelWorldTransform(model, &model->worldpos, &model->worldrot);
	}
}

MODEL_PTR GetHostModel(MODEL_MANAGER_PTR modelman)
{
	return &modelman->models[modelman->hostid];
}

void UpdateTarget(MODEL_MANAGER_PTR modelman)
{
	if (KEY_DOWN(0x31) || keyboard_state[DIK_N])
	{
		modelman->hostid++;
		if (modelman->hostid >= modelman->num_model)
			modelman->hostid = 0;

		Wait_Clock(100);
	}
}