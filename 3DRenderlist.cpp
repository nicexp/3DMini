#include "3DRenderlist.h"

//重置渲染列表
void ResetRenderlist(RENDERLIST4DV1_PTR renderlist)
{
	renderlist->num_polys = 0;
}

//将多边形面插入渲染列表
void InsertPolyToRenderlist(RENDERLIST4DV1_PTR renderlist, POLY4DV1_PTR poly)
{
	if (renderlist->num_polys >= RENDERLIST_MAX_POLYS)
		return;

	renderlist->poly_ptrs[renderlist->num_polys] = &renderlist->poly_data[renderlist->num_polys];

	renderlist->poly_data[renderlist->num_polys].state = poly->state;
	renderlist->poly_data[renderlist->num_polys].attr = poly->attr;
	renderlist->poly_data[renderlist->num_polys].color = poly->color;

	for (int vertex = 0; vertex < 3; vertex++)
	{
		VECTOR4D_COPY(&renderlist->poly_data[renderlist->num_polys].tvlist[vertex], &poly->vlist[poly->vert[vertex]]);
		VECTOR4D_COPY(&renderlist->poly_data[renderlist->num_polys].tvlist[vertex], &poly->vlist[poly->vert[vertex]]);
	}

	if (renderlist->num_polys == 0)
	{
		renderlist->poly_data[renderlist->num_polys].pre = NULL;
		renderlist->poly_data[renderlist->num_polys].next = NULL;
	}
	else
	{
		renderlist->poly_data[renderlist->num_polys].next = NULL;
		renderlist->poly_data[renderlist->num_polys].pre = &renderlist->poly_data[renderlist->num_polys - 1];
		renderlist->poly_data[renderlist->num_polys - 1].next = &renderlist->poly_data[renderlist->num_polys];
	}

	renderlist->num_polys++;
}

//物体插入渲染列表
void InsertObjToRenderlist(RENDERLIST4DV1_PTR renderlist, OBJECT4DV1_PTR obj, int islocal)
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

		InsertPolyToRenderlist(renderlist,cur_poly);

		cur_poly->vlist = vlist_old;
	}
}

//渲染列表进行矩阵变换
void Transform_Renderlist(RENDERLIST4DV1_PTR renderlist, MATRIX4X4_PTR mt, int coord_select)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
		for (int poly = 0; poly < renderlist->num_polys; poly++)
		{
			POLYF4DV1_PTR cur_poly = renderlist->poly_ptrs[poly];
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
		for (int poly = 0; poly < renderlist->num_polys; poly++)
		{
			POLYF4DV1_PTR cur_poly = renderlist->poly_ptrs[poly];
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
void ConvertRenderlistByW(RENDERLIST4DV1_PTR renderlist)
{
	POLYF4DV1_PTR cur_poly;

	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		cur_poly = renderlist->poly_ptrs[poly];

		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[0]);
		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[1]);
		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[2]);
	}
}

//渲染列表背面消除
void RemoveRendlistBackface(RENDERLIST4DV1_PTR renderlist,POINT4D_PTR cam_pos)
{
	//计算多边形面法线
	VECTOR4D u, v, n;
	//指向视点的向量
	VECTOR4D view;

	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		POLYF4DV1_PTR cur_poly = renderlist->poly_ptrs[poly];

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
void WorldToCameraRenderlist(RENDERLIST4DV1_PTR renderlist,MATRIX4X4_PTR mcam)
{
	Transform_Renderlist(renderlist,mcam, TRANSFORM_TRANS_ONLY);
}

//渲染列表相机坐标转换为透视坐标
void CameraToPerspectRenderlist(RENDERLIST4DV1_PTR renderlist,MATRIX4X4_PTR mper)
{
	Transform_Renderlist(renderlist,mper, TRANSFORM_TRANS_ONLY);
	ConvertRenderlistByW(renderlist);
}

//渲染列表透视坐标转换成屏幕坐标
void PerspectToScreenRenderlist(RENDERLIST4DV1_PTR renderlist,MATRIX4X4_PTR mscr)
{
	Transform_Renderlist(renderlist,mscr, TRANSFORM_TRANS_ONLY);
}

//渲染列表3D流水线
void Renderlist3DLine(RENDERLIST4DV1_PTR renderlist, CAM4DV1_PTR cam)
{
	RemoveRendlistBackface(renderlist, &cam->pos);
	WorldToCameraRenderlist(renderlist, &cam->mcam);
	CameraToPerspectRenderlist(renderlist,  &cam->mper);
	PerspectToScreenRenderlist(renderlist, &cam->mscr);
}