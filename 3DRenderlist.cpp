#include "3DRenderlist.h"

Renderlist::Renderlist()
{
}

Renderlist::~Renderlist()
{
}

//重置渲染列表
void Renderlist::ResetRenderlist()
{
	this->num_polys = 0;
}

//将多边形面插入渲染列表
void Renderlist::InsertPolyToRenderlist(POLY4DV1_PTR poly)
{
	if (this->num_polys >= RENDERLIST_MAX_POLYS)
		return;

	this->poly_ptrs[this->num_polys] = &this->poly_data[this->num_polys];

	this->poly_data[this->num_polys].state = poly->state;
	this->poly_data[this->num_polys].attr = poly->attr;
	this->poly_data[this->num_polys].color = poly->color;

	for (int vertex = 0; vertex < 3; vertex++)
	{
		VECTOR4D_COPY(&this->poly_data[this->num_polys].tvlist[vertex], &poly->vlist[poly->vert[vertex]]);
		VECTOR4D_COPY(&this->poly_data[this->num_polys].vlist[vertex], &poly->vlist[poly->vert[vertex]]);
	}

	if (this->num_polys == 0)
	{
		this->poly_data[this->num_polys].pre = NULL;
		this->poly_data[this->num_polys].next = NULL;
	}
	else
	{
		this->poly_data[this->num_polys].next = NULL;
		this->poly_data[this->num_polys].pre = &this->poly_data[this->num_polys - 1];
		this->poly_data[this->num_polys - 1].next = &this->poly_data[this->num_polys];
	}

	this->num_polys++;
}

//物体插入渲染列表
void Renderlist::InsertObjToRenderlist(Object3D* obj, int islocal)
{
	if (!(obj->state & OBJECT4DV1_STATE_ACTIVE)
		|| (obj->state & OBJECT4DV1_STATE_CULLED)
		|| !(obj->state & OBJECT4DV1_STATE_VISIBLE))
		return;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR cur_poly = &obj->plist[poly];

		if (!(cur_poly->state & POLY4DV1_STATE_ACTIVE)
			|| (cur_poly->state & POLY4DV1_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV1_STATE_CLIPPED))
			continue;

		POINT4D_PTR vlist_old = cur_poly->vlist;

		if (islocal)
			cur_poly->vlist = obj->vlist_local;
		else
			cur_poly->vlist = obj->vlist_trans;

		InsertPolyToRenderlist(cur_poly);

		cur_poly->vlist = vlist_old;
	}
}

//渲染列表进行矩阵变换
void Renderlist::Transform_Renderlist(MATRIX4X4_PTR mt, int coord_select)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
		for (int poly = 0; poly < this->num_polys; poly++)
		{
			POLYF4DV1_PTR cur_poly = this->poly_ptrs[poly];
			if (!(cur_poly->state & POLY4DV1_STATE_ACTIVE)
				|| (cur_poly->state & POLY4DV1_STATE_BACKFACE)
				|| (cur_poly->state & POLY4DV1_STATE_CLIPPED))
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D presult;
				Mat_Mul_VECTOR4D_4X4(&cur_poly->vlist[vertex], mt, &presult);
				VECTOR4D_COPY(&cur_poly->vlist[vertex], &presult);
			}
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (int poly = 0; poly < this->num_polys; poly++)
		{
			POLYF4DV1_PTR cur_poly = this->poly_ptrs[poly];
			if (!(cur_poly->state & POLY4DV1_STATE_ACTIVE)
				|| (cur_poly->state & POLY4DV1_STATE_BACKFACE)
				|| (cur_poly->state & POLY4DV1_STATE_CLIPPED))
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D presult;
				Mat_Mul_VECTOR4D_4X4(&cur_poly->tvlist[vertex], mt, &presult);
				VECTOR4D_COPY(&cur_poly->tvlist[vertex], &presult);
			}
		}
		break;
	default:
		break;
	}
}

//渲染列表齐次坐标转换
void Renderlist::ConvertRenderlistByW()
{
	POLYF4DV1_PTR cur_poly;

	for (int poly = 0; poly < this->num_polys; poly++)
	{
		cur_poly = this->poly_ptrs[poly];

		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[0]);
		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[1]);
		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[2]);
	}
}

//渲染列表背面消除
void Renderlist::RemoveRendlistBackface(POINT4D_PTR cam_pos)
{
	//计算多边形面法线
	VECTOR4D u, v, n;
	//指向视点的向量
	VECTOR4D view;

	for (int poly = 0; poly < this->num_polys; poly++)
	{
		POLYF4DV1_PTR cur_poly = this->poly_ptrs[poly];

		if (cur_poly == NULL)
			continue;

		VECTOR4D_SUB(&cur_poly->tvlist[1], &cur_poly->tvlist[0], &u);
		VECTOR4D_SUB(&cur_poly->tvlist[2], &cur_poly->tvlist[1], &v);
		//计算法向量
		VECTOR4D_CROSS(&u, &v, &n);
		//指向视点的向量
		VECTOR4D_SUB(cam_pos, &cur_poly->tvlist[0], &view);
		float dp = VECTOR4D_DOT(&n, &view);
		if (dp < 0)
			SET_BIT(cur_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}

//渲染列表世界坐标转换为相机坐标
void Renderlist::WorldToCameraRenderlist(MATRIX4X4_PTR mcam)
{
	Transform_Renderlist(mcam, TRANSFORM_TRANS_ONLY);
}

//渲染列表相机坐标转换为透视坐标
void Renderlist::CameraToPerspectRenderlist(MATRIX4X4_PTR mper)
{
	Transform_Renderlist(mper, TRANSFORM_TRANS_ONLY);
	ConvertRenderlistByW();
}

//渲染列表透视坐标转换成屏幕坐标
void Renderlist::PerspectToScreenRenderlist(MATRIX4X4_PTR mscr)
{
	Transform_Renderlist(mscr, TRANSFORM_TRANS_ONLY);
}

//渲染列表3D流水线
void Renderlist::Renderlist3DLine(Camera* cam)
{
	RemoveRendlistBackface(&cam->pos);
	WorldToCameraRenderlist(&cam->mcam);
	CameraToPerspectRenderlist(&cam->mper);
	PerspectToScreenRenderlist(&cam->mscr);
}