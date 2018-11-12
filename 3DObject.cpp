#include "3DObject.h"

Object3D::Object3D()
{
}

Object3D::~Object3D()
{
}

void Object3D::InitObject()
{
	this->id = 0;

	SET_BIT(this->state, OBJECT4DV1_STATE_ACTIVE);
	SET_BIT(this->state, OBJECT4DV1_STATE_VISIBLE);

	this->attr = 0;
	this->num_vertices = 8;

	this->avg_radius = BORDER_LENGTH * 1.732;
	this->max_radius = BORDER_LENGTH * 1.732;

	this->world_pos.x = 0;
	this->world_pos.y = 0;
	this->world_pos.z = 0;
	this->world_pos.w = 1;

	this->dir.x = 0;
	this->dir.y = 0;
	this->dir.z = 1;
	this->dir.w = 1;

	VECTOR4D_INITXYZ(&this->ux, 1, 0, 0);
	VECTOR4D_INITXYZ(&this->uy, 0, 1, 0);
	VECTOR4D_INITXYZ(&this->uz, 0, 0, 1);

	//边长
	int length = BORDER_LENGTH;
	POINT4D temp_verts[8] = {
		{ length, length, length },
		{ length, length, -length },
		{ -length, length, -length },
		{ -length, length, length },
		{ length, -length, length },
		{ -length, -length, length },
		{ -length, -length, -length },
		{ length, -length, -length },
	};
	//设置顶点坐标
	for (int i = 0; i < 8; i++)
	{
		this->vlist_local[i].x = temp_verts[i].x;
		this->vlist_local[i].y = temp_verts[i].y;
		this->vlist_local[i].z = temp_verts[i].z;
		this->vlist_local[i].w = 1;
	}
	//多边形数量
	this->num_polys = 12;
	//定义一个三角形的顶点列表
	int temp_poly_indices[12 * 3] =
	{
		0, 1, 2, 0, 2, 3,
		0, 7, 1, 0, 4, 7,
		1, 7, 6, 1, 6, 2,
		2, 6, 5, 2, 5, 3,
		0, 5, 4, 0, 3, 5,
		5, 6, 7, 4, 5, 7
	};
	//初始化每个三角形
	for (int tri = 0; tri < 12; tri++)
	{
		SET_BIT(this->plist[tri].state, POLY4DV1_STATE_ACTIVE);
		this->plist[tri].attr = 0;
		this->plist[tri].color = 0;

		this->plist[tri].vlist = this->vlist_trans;

		this->plist[tri].vert[0] = temp_poly_indices[3 * tri + 0];
		this->plist[tri].vert[1] = temp_poly_indices[3 * tri + 1];
		this->plist[tri].vert[2] = temp_poly_indices[3 * tri + 2];
	}
}

//局部坐标转换为世界坐标
void Object3D::ModelToWorldObj()
{
	//世界坐标转换矩阵
	MATRIX4X4 matworld;
	Mat_Init_4X4(&matworld,
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		this->world_pos.x, this->world_pos.y, this->world_pos.z, 1);

	Transform_Object(&matworld, TRANSFORM_LOCAL_TO_TRANS, 1);
}

//世界坐标转换相机坐标
void Object3D::WorldToCameraObj(MATRIX4X4_PTR mcam)
{
	Transform_Object(mcam, TRANSFORM_TRANS_ONLY, 1);
}

//相机坐标转换为透视坐标
void Object3D::CameraToPerspectObj(MATRIX4X4_PTR mper)
{
	Transform_Object(mper, TRANSFORM_TRANS_ONLY, 1);
	ConvertObjByW();
}

//透视坐标转换成屏幕坐标
void Object3D::PerspectToScreenObj(MATRIX4X4_PTR mscr)
{
	Transform_Object(mscr, TRANSFORM_TRANS_ONLY, 1);
}

//物体背面消除
void Object3D::RemoveObjBackface(POINT4D_PTR cam_pos)
{
	//计算多边形面法线
	VECTOR4D u, v, n;
	//指向视点的向量
	VECTOR4D view;

	for (int poly = 0; poly < this->num_polys; poly++)
	{
		POLY4DV1_PTR curpoly = &this->plist[poly];
		//取顶点
		int vertex_idx1 = curpoly->vert[0];
		int vertex_idx2 = curpoly->vert[1];
		int vertex_idx3 = curpoly->vert[2];
		//取u和v
		VECTOR4D_SUB(&curpoly->vlist[vertex_idx2], &curpoly->vlist[vertex_idx1], &u);
		VECTOR4D_SUB(&curpoly->vlist[vertex_idx3], &curpoly->vlist[vertex_idx2], &v);
		//求法线n
		VECTOR4D_CROSS(&u, &v, &n);
		//指向视点
		VECTOR4D_SUB(cam_pos, &curpoly->vlist[vertex_idx1], &view);
		//计算点积
		float dp = VECTOR4D_DOT(&n, &view);
		if (dp <= 0.0)
			SET_BIT(curpoly->state, POLY4DV1_STATE_BACKFACE);
	}
}

//物体进行矩阵变换运算
void Object3D::Transform_Object(MATRIX4X4_PTR mt, int coord_select, int bTransformDir)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
		for (int vertex = 0; vertex < this->num_vertices; vertex++)
		{
			POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&this->vlist_local[vertex], mt, &presult);
			VECTOR4D_COPY(&this->vlist_local[vertex], &presult);
		}
		break;
	}
	case TRANSFORM_TRANS_ONLY:
	{
		for (int vertex = 0; vertex < this->num_vertices; vertex++)
		{
			POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&this->vlist_trans[vertex], mt, &presult);
			VECTOR4D_COPY(&this->vlist_trans[vertex], &presult);
		}
		break;
	}
	case TRANSFORM_LOCAL_TO_TRANS:
	{
		for (int vertex = 0; vertex < this->num_vertices; vertex++)
		{
			POINT4D presult;
			Mat_Mul_VECTOR4D_4X4(&this->vlist_local[vertex], mt, &presult);
			VECTOR4D_COPY(&this->vlist_trans[vertex], &presult);
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
		Mat_Mul_VECTOR4D_4X4(&this->ux, mt, &vresult);
		VECTOR4D_COPY(&this->ux, &vresult);
		Mat_Mul_VECTOR4D_4X4(&this->uy, mt, &vresult);
		VECTOR4D_COPY(&this->uy, &vresult);
		Mat_Mul_VECTOR4D_4X4(&this->uz, mt, &vresult);
		VECTOR4D_COPY(&this->uz, &vresult);
	}
}
//物体齐次坐标转换
void Object3D::ConvertObjByW()
{
	for (int k = 0; k < this->num_vertices; k++)
	{
		VECTOR4D_DIV_BY_W(&this->vlist_trans[k]);
	}
}

//剔除物体
int Object3D::CullObj(Camera* cam, int cull_flags)
{
	POINT4D sphere_pos; //包围球坐标

	//将包围球转换相机坐标
	Mat_Mul_VECTOR4D_4X4(&this->world_pos, &cam->mcam, &sphere_pos);

	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		if ((sphere_pos.z - this->max_radius > cam->far_clip_z)
			|| (sphere_pos.z + this->max_radius < cam->near_clip_z))
		{
			SET_BIT(this->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		float z_test = (cam->viewplane_width / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.x - this->max_radius > z_test)
			|| (sphere_pos.x + this->max_radius < -z_test))
		{
			SET_BIT(this->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		float z_test = (cam->viewplane_height / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.y - this->max_radius > z_test)
			|| (sphere_pos.y + this->max_radius < -z_test))
		{
			SET_BIT(this->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}

	return 0;
}

//重置物体状态
void Object3D::ResetObjState()
{
	RESET_BIT(this->state, OBJECT4DV1_STATE_CULLED);

	for (int poly = 0; poly < this->num_polys; poly++)
	{
		POLY4DV1_PTR cur_poly = &this->plist[poly];

		RESET_BIT(cur_poly->state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(cur_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}

//物体渲染3D流水线
void Object3D::Object3DLine(Camera* cam)
{
	ModelToWorldObj();//世界坐标
	RemoveObjBackface(&cam->pos);//消除背面
	WorldToCameraObj(&cam->mcam);//相机坐标
	CameraToPerspectObj(&cam->mper);//透视坐标
	PerspectToScreenObj(&cam->mscr);//屏幕坐标
}