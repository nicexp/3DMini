#include "3DRenderlist2.h"
#include "3DCamera.h"
#include "3DLib2.h"
//������Ⱦ�б�
void ResetRenderlist(RENDERLIST4DV2_PTR renderlist)
{
	renderlist->num_polys = 0;
}

//��������������Ⱦ�б�
void InsertPolyToRenderlist(RENDERLIST4DV2_PTR renderlist, POLY4DV2_PTR poly)
{
	if (renderlist->num_polys >= RENDERLIST_MAX_POLYS)
		return;

	renderlist->poly_ptrs[renderlist->num_polys] = &renderlist->poly_data[renderlist->num_polys];

	renderlist->poly_data[renderlist->num_polys].state = poly->state;
	renderlist->poly_data[renderlist->num_polys].attr = poly->attr;
	renderlist->poly_data[renderlist->num_polys].color = poly->color;
	renderlist->poly_data[renderlist->num_polys].nlength = poly->nlength;
	renderlist->poly_data[renderlist->num_polys].texture = poly->texture;

	for (int vertex = 0; vertex < 3; vertex++)
	{
		VERTEX4DTV1_COPY(&renderlist->poly_data[renderlist->num_polys].vlist[vertex], &poly->vlist[poly->vert[vertex]]);
		VERTEX4DTV1_COPY(&renderlist->poly_data[renderlist->num_polys].tvlist[vertex], &poly->vlist[poly->vert[vertex]]);
		renderlist->poly_data[renderlist->num_polys].tvlist[vertex].t = poly->tlist[poly->text[vertex]];
		renderlist->poly_data[renderlist->num_polys].vlist[vertex].t = poly->tlist[poly->text[vertex]];
		renderlist->poly_data[renderlist->num_polys].lit_color[vertex] = poly->lit_color[vertex];
		
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

//��������������Ⱦ�б�
void InsertPolyToRenderlist(RENDERLIST4DV2_PTR renderlist, POLYF4DV2_PTR poly)
{
	if (renderlist->num_polys >= RENDERLIST_MAX_POLYS)
		return;

	renderlist->poly_ptrs[renderlist->num_polys] = &renderlist->poly_data[renderlist->num_polys];

	memcpy((void*)&renderlist->poly_data[renderlist->num_polys], (void*)poly, sizeof(POLYF4DV2));

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

//���������Ⱦ�б�
void InsertObjToRenderlist(RENDERLIST4DV2_PTR renderlist, OBJECT4DV2_PTR obj, int islocal)
{
	if (!(obj->state & OBJECT4DV2_STATE_ACTIVE)
		|| (obj->state & OBJECT4DV2_STATE_CULLED)
		|| !(obj->state & OBJECT4DV2_STATE_VISIBLE))
		return;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &obj->plist[poly];

		if (!(cur_poly->state & POLY4DV2_STATE_ACTIVE)
			|| (cur_poly->state & POLY4DV2_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV2_STATE_CLIPPED))
			continue;

		VERTEX4DTV1_PTR vlist_old = cur_poly->vlist;

		if (islocal)
			cur_poly->vlist = obj->vlist_local;
		else
			cur_poly->vlist = obj->vlist_trans;

		InsertPolyToRenderlist(renderlist, cur_poly);

		cur_poly->vlist = vlist_old;
	}
}

//��ģ�Ͳ��뵽��Ⱦ�б���
void InsertMeshToRenderlist(RENDERLIST4DV2_PTR renderlist, MESH_PTR mesh)
{
	if (!(mesh->state & MESH_STATE_ACTIVE)
		|| (mesh->state & MESH_STATE_CULLED)
		|| !(mesh->state & MESH_STATE_VISIBLE))
		return;
	for (int k = 0; k < mesh->num_polys; k++)
	{
		if (renderlist->num_polys >= RENDERLIST_MAX_POLYS)
		{
			DEBUG_LOG("ERROR:poly num too many");
			return;
		}
		POLY4DV2_PTR poly = &mesh->plist[k];

		renderlist->poly_ptrs[renderlist->num_polys] = &renderlist->poly_data[renderlist->num_polys];

		renderlist->poly_data[renderlist->num_polys].state = poly->state;
		renderlist->poly_data[renderlist->num_polys].attr = poly->attr;
		renderlist->poly_data[renderlist->num_polys].color = poly->color;
		renderlist->poly_data[renderlist->num_polys].nlength = poly->nlength;
		renderlist->poly_data[renderlist->num_polys].texture = poly->texture;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			VERTEX4DTV1_COPY(&renderlist->poly_data[renderlist->num_polys].vlist[vertex], &mesh->vlist_trans[poly->vert[vertex]]);
			VERTEX4DTV1_COPY(&renderlist->poly_data[renderlist->num_polys].tvlist[vertex], &mesh->vlist_trans[poly->vert[vertex]]);
			renderlist->poly_data[renderlist->num_polys].tvlist[vertex].t = poly->tlist[poly->text[vertex]];
			renderlist->poly_data[renderlist->num_polys].vlist[vertex].t = poly->tlist[poly->text[vertex]];
			renderlist->poly_data[renderlist->num_polys].lit_color[vertex] = poly->lit_color[vertex];
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
}

//��Ⱦ�б���о���任
void Transform_Renderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mt, int coord_select, int bNormal)
{
	MATRIX4X4 mr;//ȥ��ƽ�Ʋ��������ڷ�������
	memcpy((void*)&mr, (void*)mt, sizeof(MATRIX4X4));
	mr.M30 = mr.M31 = mr.M32 = 0;

	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
		for (int poly = 0; poly < renderlist->num_polys; poly++)
		{
			POLYF4DV2_PTR cur_poly = renderlist->poly_ptrs[poly];
			if (!(cur_poly->state & POLY4DV2_STATE_ACTIVE)
				|| (cur_poly->state & POLY4DV2_STATE_BACKFACE)
				|| (cur_poly->state & POLY4DV2_STATE_CLIPPED))
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D presult;
				Mat_Mul_VECTOR4D_4X4(&cur_poly->vlist[vertex].v, mt, &presult);
				VECTOR4D_COPY(&cur_poly->vlist[vertex].v, &presult);

				if (bNormal)
				{
					Mat_Mul_VECTOR4D_4X4(&cur_poly->vlist[vertex].n, &mr, &presult);
					VECTOR4D_COPY(&cur_poly->vlist[vertex].n, &presult);
				}
			}
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (int poly = 0; poly < renderlist->num_polys; poly++)
		{
			POLYF4DV2_PTR cur_poly = renderlist->poly_ptrs[poly];
			if (!(cur_poly->state & POLY4DV2_STATE_ACTIVE)
				|| (cur_poly->state & POLY4DV2_STATE_BACKFACE)
				|| (cur_poly->state & POLY4DV2_STATE_CLIPPED))
				continue;

			for (int vertex = 0; vertex < 3; vertex++)
			{
				VECTOR4D presult;
				Mat_Mul_VECTOR4D_4X4(&cur_poly->tvlist[vertex].v, mt, &presult);
				VECTOR4D_COPY(&cur_poly->tvlist[vertex].v, &presult);

				if (bNormal)
				{
					Mat_Mul_VECTOR4D_4X4(&cur_poly->tvlist[vertex].n, &mr, &presult);
					VECTOR4D_COPY(&cur_poly->tvlist[vertex].n, &presult);
				}
			}
		}
		break;
	default:
		break;
	}
}

//��Ⱦ�б��������ת��
void ConvertRenderlistByW(RENDERLIST4DV2_PTR renderlist)
{
	POLYF4DV2_PTR cur_poly;

	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		cur_poly = renderlist->poly_ptrs[poly];

		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[0].v);
		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[1].v);
		VECTOR4D_DIV_BY_W(&cur_poly->tvlist[2].v);
	}
}

//��Ⱦ�б�������
void RemoveRendlistBackface(RENDERLIST4DV2_PTR renderlist, POINT4D_PTR cam_pos)
{
	//���������淨��
	VECTOR4D u, v, n;
	//ָ���ӵ������
	VECTOR4D view;

	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = renderlist->poly_ptrs[poly];

		if (cur_poly == NULL)
			continue;

		VECTOR4D_SUB(&cur_poly->tvlist[1].v, &cur_poly->tvlist[0].v, &u);
		VECTOR4D_SUB(&cur_poly->tvlist[2].v, &cur_poly->tvlist[1].v, &v);
		//���㷨����
		VECTOR4D_CROSS(&u, &v, &n);
		//ָ���ӵ������
		VECTOR4D_SUB(cam_pos, &cur_poly->tvlist[0].v, &view);
		float dp = VECTOR4D_DOT(&n, &view);
		if (dp < 0)
			SET_BIT(cur_poly->state, POLY4DV2_STATE_BACKFACE);
	}
}

//��Ⱦ�б���������ת��Ϊ�������
void WorldToCameraRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mcam)
{
	Transform_Renderlist(renderlist, mcam, TRANSFORM_TRANS_ONLY, 1);
}

//��Ⱦ�б��������ת��Ϊ͸������
void CameraToPerspectRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mper)
{
	Transform_Renderlist(renderlist, mper, TRANSFORM_TRANS_ONLY, 0);
	ConvertRenderlistByW(renderlist);
}

//��Ⱦ�б�͸������ת������Ļ����
void PerspectToScreenRenderlist(RENDERLIST4DV2_PTR renderlist, MATRIX4X4_PTR mscr)
{
	Transform_Renderlist(renderlist, mscr, TRANSFORM_TRANS_ONLY, 0);
}

/*�������ռ���3D�ü�
�ȶ���������Զ���ü�����м򵥽���/�޳�
Ȼ��Խ��ü�����вü�*/
void ClipPolysRenderlist(RENDERLIST4DV2_PTR renderlist, CAM4DV1_PTR cam, int cull_flags)
{
	int last_poly_index = renderlist->num_polys;

	//x��y����ü���б��
	float z_factorx = 0.5 * cam->viewplane_width / cam->view_dist;
	float z_factory = 0.5 * cam->viewplane_height / cam->view_dist;
	float z_test[3] = {0};
	float t_param1 = 0, t_param2 = 0;//������
	float x1, y1, x2, y2;//����
	float tu1, tv1, tu2, tv2;//��������
	float nx1, ny1, nz1, nx2, ny2, nz2;//���㷨��
	int nums_in = 0;
	int index = 0;//ѭ������
	int v0 = 0, v1 = 1, v2 = 2; //�����ζ�������
	VECTOR4D u, v, n;//���㷨����ʱ����
	POLYF4DV2 temp_poly;//�ü���������

	for (int poly = 0; poly < last_poly_index; poly++)
	{
		POLYF4DV2_PTR cur_poly = &renderlist->poly_data[poly];
		//�ж��Ƿ�ü�ƽ��
		if ((cur_poly == NULL) || (!(cur_poly->state & POLY4DV2_STATE_ACTIVE))
			|| (cur_poly->state & POLY4DV2_STATE_BACKFACE) || (cur_poly->state & POLY4DV2_STATE_CLIPPED))
			continue;

		if (cull_flags & CULL_OBJECT_X_PLANE) //���Ҳü���
		{
			for (index = 0; index < 3; index++)
			{
				z_test[index] = z_factorx * cur_poly->tvlist[index].z;
			}
			//�ü�
			if (((cur_poly->tvlist[0].x > z_test[0]) &&
				(cur_poly->tvlist[1].x > z_test[1]) &&
				(cur_poly->tvlist[2].x > z_test[2])) ||
				(cur_poly->tvlist[0].x < -z_test[0]) &&
				(cur_poly->tvlist[1].x < -z_test[1]) &&
				(cur_poly->tvlist[2].x < -z_test[2]))
			{
				SET_BIT(cur_poly->state, POLY4DV2_STATE_CLIPPED);
				continue;
			}
		}
		if (cull_flags & CULL_OBJECT_Y_PLANE) //���²ü���
		{
			for (int i = 0; i < 3; i++)
			{
				z_test[i] = z_factory * cur_poly->tvlist[i].z;
			}
			//�ü�
			if (((cur_poly->tvlist[0].y > z_test[0]) &&
				(cur_poly->tvlist[1].y > z_test[1]) &&
				(cur_poly->tvlist[2].y > z_test[2])) ||
				(cur_poly->tvlist[0].y < -z_test[0]) &&
				(cur_poly->tvlist[1].y < -z_test[1]) &&
				(cur_poly->tvlist[2].y < -z_test[2]))
			{
				SET_BIT(cur_poly->state, POLY4DV2_STATE_CLIPPED);
				continue;
			}
		}
		if (cull_flags & CULL_OBJECT_Z_PLANE) //Զ���ü���
		{
			//�ü�
			if (((cur_poly->tvlist[0].z > cam->far_clip_z) &&
				(cur_poly->tvlist[1].z > cam->far_clip_z) &&
				(cur_poly->tvlist[2].z > cam->far_clip_z)) ||
				(cur_poly->tvlist[0].z < cam->near_clip_z) &&
				(cur_poly->tvlist[1].z < cam->near_clip_z) &&
				(cur_poly->tvlist[2].z < cam->near_clip_z))
			{
				SET_BIT(cur_poly->state, POLY4DV2_STATE_CLIPPED);
				continue;
			}
		}
		//�Խ��ü������3D�ü�
		//�ж��м��������ڽ��ü�����
		nums_in = 0;
		for (index = 0; index < 3; index++)
		{
			if (cur_poly->tvlist[index].z >= cam->near_clip_z)
				nums_in++;
		}
		//�����һ�������ڽ��ü����ڲ�
		if (nums_in == 1)
		{
			//�ж����ĸ��������ڲ�
			if (cur_poly->tvlist[0].z >= cam->near_clip_z)
			{
				v0 = 0; v1 = 1; v2 = 2;
			}
			else if (cur_poly->tvlist[1].z >= cam->near_clip_z)
			{
				v0 = 1; v1 = 2; v2 = 0;
			}
			else
			{
				v0 = 2; v1 = 0; v2 = 1;
			}
			//v0->v1��ò���t
			t_param1 = (cam->near_clip_z - cur_poly->tvlist[v0].z) / (cur_poly->tvlist[v1].z - cur_poly->tvlist[v0].z);
			//����
			x1 = cur_poly->tvlist[v0].x + (cur_poly->tvlist[v1].x - cur_poly->tvlist[v0].x) * t_param1;
			y1 = cur_poly->tvlist[v0].y + (cur_poly->tvlist[v1].y - cur_poly->tvlist[v0].y) * t_param1;
			//����v1
			cur_poly->tvlist[v1].x = x1;
			cur_poly->tvlist[v1].y = y1;
			cur_poly->tvlist[v1].z = cam->near_clip_z;
			//���¼��㶥�㷨��
			nx1 = cur_poly->tvlist[v0].nx + (cur_poly->tvlist[v1].nx - cur_poly->tvlist[v0].nx) * t_param1;
			ny1 = cur_poly->tvlist[v0].ny + (cur_poly->tvlist[v1].ny - cur_poly->tvlist[v0].ny) * t_param1;
			nz1 = cur_poly->tvlist[v0].nz + (cur_poly->tvlist[v1].nz - cur_poly->tvlist[v0].nz) * t_param1;
			cur_poly->tvlist[v1].nx = nx1;
			cur_poly->tvlist[v1].ny = ny1;
			cur_poly->tvlist[v1].nz = nz1;
			//v0->v2��ò���t
			t_param2 = (cam->near_clip_z - cur_poly->tvlist[v0].z) / (cur_poly->tvlist[v2].z - cur_poly->tvlist[v0].z);
			//����
			x2 = cur_poly->tvlist[v0].x + (cur_poly->tvlist[v2].x - cur_poly->tvlist[v0].x) * t_param2;
			y2 = cur_poly->tvlist[v0].y + (cur_poly->tvlist[v2].y - cur_poly->tvlist[v0].y) * t_param2;
			//����v2
			cur_poly->tvlist[v2].x = x2;
			cur_poly->tvlist[v2].y = y2;
			cur_poly->tvlist[v2].z = cam->near_clip_z;
			//����
			nx2 = cur_poly->tvlist[v0].nx + (cur_poly->tvlist[v2].nx - cur_poly->tvlist[v0].nx) * t_param2;
			ny2 = cur_poly->tvlist[v0].ny + (cur_poly->tvlist[v2].ny - cur_poly->tvlist[v0].ny) * t_param2;
			nz2 = cur_poly->tvlist[v0].nz + (cur_poly->tvlist[v2].nz - cur_poly->tvlist[v0].nz) * t_param2;
			cur_poly->tvlist[v2].nx = nx2;
			cur_poly->tvlist[v2].ny = ny2;
			cur_poly->tvlist[v2].nz = nz2;
			//�ж�����
			if (cur_poly->attr & POLY4DV2_ATTR_SHAD_MODE_TEXTURE)
			{
				//v1��������
				tu1 = cur_poly->tvlist[v0].u0 + t_param1 * (cur_poly->tvlist[v1].u0 - cur_poly->tvlist[v0].u0);
				tv1 = cur_poly->tvlist[v0].v0 + t_param1 * (cur_poly->tvlist[v1].v0 - cur_poly->tvlist[v0].v0);
				//����v1
				cur_poly->tvlist[v1].u0 = tu1;
				cur_poly->tvlist[v1].v0 = tv1;
				//v2��������
				tu2 = cur_poly->tvlist[v0].u0 + t_param2 * (cur_poly->tvlist[v2].u0 - cur_poly->tvlist[v0].u0);
				tv2 = cur_poly->tvlist[v0].v0 + t_param2 * (cur_poly->tvlist[v2].v0 - cur_poly->tvlist[v0].v0);
				//����v2
				cur_poly->tvlist[v2].u0 = tu2;
				cur_poly->tvlist[v2].v0 = tv2;
			}
			//���¼��㷨�߳���
			VECTOR4D_SUB(&cur_poly->tvlist[1].v, &cur_poly->tvlist[0].v, &u);
			VECTOR4D_SUB(&cur_poly->tvlist[2].v, &cur_poly->tvlist[0].v, &v);
			VECTOR4D_CROSS(&u, &v, &n);
			cur_poly->nlength = VECTOR4D_Length(&n);
		}
		else if (nums_in == 2)
		{
			//����ǰ�渴�Ƶ���������
			memcpy((void*)&temp_poly, (void*)cur_poly, sizeof(POLYF4DV2));

			if (cur_poly->tvlist[0].z < cam->near_clip_z)
			{
				v0 = 0; v1 = 1; v2 = 2;
			}
			else if (cur_poly->tvlist[1].z < cam->near_clip_z)
			{
				v0 = 1; v1 = 2; v2 = 0;
			}
			else
			{
				v0 = 2; v1 = 0; v2 = 1;
			}

			//��ò���t
			t_param1 = (cam->near_clip_z - cur_poly->tvlist[v0].z) / (cur_poly->tvlist[v1].z - cur_poly->tvlist[v0].z);
			t_param2 = (cam->near_clip_z - cur_poly->tvlist[v0].z) / (cur_poly->tvlist[v2].z - cur_poly->tvlist[v0].z);
			//v0->v1����
			x1 = cur_poly->tvlist[v0].x + t_param1 *(cur_poly->tvlist[v1].x - cur_poly->tvlist[v0].x);
			y1 = cur_poly->tvlist[v0].y + t_param1 *(cur_poly->tvlist[v1].y - cur_poly->tvlist[v0].y);
			//v0->v2����
			x2 = cur_poly->tvlist[v0].x + t_param2 *(cur_poly->tvlist[v2].x - cur_poly->tvlist[v0].x);
			y2 = cur_poly->tvlist[v0].y + t_param2 *(cur_poly->tvlist[v2].y - cur_poly->tvlist[v0].y);
			//v0->v1����
			nx1 = cur_poly->tvlist[v0].nx + (cur_poly->tvlist[v1].nx - cur_poly->tvlist[v0].nx) * t_param1;
			ny1 = cur_poly->tvlist[v0].ny + (cur_poly->tvlist[v1].ny - cur_poly->tvlist[v0].ny) * t_param1;
			nz1 = cur_poly->tvlist[v0].nz + (cur_poly->tvlist[v1].nz - cur_poly->tvlist[v0].nz) * t_param1;
			//v0->v2����
			nx2 = cur_poly->tvlist[v0].nx + (cur_poly->tvlist[v2].nx - cur_poly->tvlist[v0].nx) * t_param2;
			ny2 = cur_poly->tvlist[v0].ny + (cur_poly->tvlist[v2].ny - cur_poly->tvlist[v0].ny) * t_param2;
			nz2 = cur_poly->tvlist[v0].nz + (cur_poly->tvlist[v2].nz - cur_poly->tvlist[v0].nz) * t_param2;
			
			//v0->v1�����뷨�߸���v0
			cur_poly->tvlist[v0].x = x1;
			cur_poly->tvlist[v0].y = y1;
			cur_poly->tvlist[v0].z = cam->near_clip_z;
			cur_poly->tvlist[v0].nx = nx1;
			cur_poly->tvlist[v0].ny = ny1;
			cur_poly->tvlist[v0].nz = nz1;

			//v0->v1���㸲�������v1
			temp_poly.tvlist[v1].x = x1;
			temp_poly.tvlist[v1].y = y1;
			temp_poly.tvlist[v1].z = cam->near_clip_z;
			temp_poly.tvlist[v1].nx = nx1;
			temp_poly.tvlist[v1].ny = ny1;
			temp_poly.tvlist[v1].nz = nz1;
			//v0->v2���㸲�������v0
			temp_poly.tvlist[v0].x = x2;
			temp_poly.tvlist[v0].y = y2;
			temp_poly.tvlist[v0].z = cam->near_clip_z;
			temp_poly.tvlist[v0].nx = nx2;
			temp_poly.tvlist[v0].ny = ny2;
			temp_poly.tvlist[v0].nz = nz2;

			if (cur_poly->attr & POLY4DV2_ATTR_SHAD_MODE_TEXTURE)
			{
				//v1��������
				tu1 = cur_poly->tvlist[v0].u0 + t_param1 * (cur_poly->tvlist[v1].u0 - cur_poly->tvlist[v0].u0);
				tv1 = cur_poly->tvlist[v0].v0 + t_param1 * (cur_poly->tvlist[v1].v0 - cur_poly->tvlist[v0].v0);
				//v2��������
				tu2 = cur_poly->tvlist[v0].u0 + t_param2 * (cur_poly->tvlist[v2].u0 - cur_poly->tvlist[v0].u0);
				tv2 = cur_poly->tvlist[v0].v0 + t_param2 * (cur_poly->tvlist[v2].v0 - cur_poly->tvlist[v0].v0);

				cur_poly->tvlist[v0].u0 = tu1;
				cur_poly->tvlist[v0].v0 = tv1;

				temp_poly.tvlist[v1].u0 = tu1;
				temp_poly.tvlist[v1].v0 = tv1;
				temp_poly.tvlist[v0].u0 = tu2;
				temp_poly.tvlist[v0].v0 = tv2;
			}
			//���¼��㷨�߳���
			VECTOR4D_SUB(&cur_poly->tvlist[1].v, &cur_poly->tvlist[0].v, &u);
			VECTOR4D_SUB(&cur_poly->tvlist[2].v, &cur_poly->tvlist[0].v, &v);
			VECTOR4D_CROSS(&u, &v, &n);
			cur_poly->nlength = VECTOR4D_Length(&n);

			VECTOR4D_SUB(&temp_poly.tvlist[1].v, &temp_poly.tvlist[0].v, &u);
			VECTOR4D_SUB(&temp_poly.tvlist[2].v, &temp_poly.tvlist[0].v, &v);
			VECTOR4D_CROSS(&u, &v, &n);
			temp_poly.nlength = VECTOR4D_Length(&n);

			//��������뵽��Ⱦ�б���
			InsertPolyToRenderlist(renderlist, &temp_poly);
		}
	}
}

//��Ⱦ�б�㶨��ɫ���մ���
void LightRenderlistFlat(RENDERLIST4DV2_PTR renderlist, LIGHTV1_PTR lights, int max_lights)
{
	unsigned int rbase, gbase, bbase = 0;
	unsigned int rsum, gsum, bsum;

	float dp, dist, dists, i, nl, atten;
	VECTOR4D u, v, n, l, s;

	int ver1 = 0, ver2 = 1, ver3 = 2;

	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = &renderlist->poly_data[poly];
		if (!(cur_poly->state & POLY4DV2_STATE_ACTIVE)
			|| (cur_poly->state & POLY4DV2_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV2_STATE_CLIPPED))
			continue;
		//��������Ⱦ�б������½��й��մ���
		SET_BIT(cur_poly->state, POLY4DV2_STATE_LIT);
		//��ȡ��ɫ
		_RGBFROMINT(cur_poly->color, &rbase, &gbase, &bbase);

		rsum = 0;
		gsum = 0;
		bsum = 0;

		for (int cur_light = 0; cur_light < max_lights; cur_light++)
		{
			if (lights[cur_light].state == LIGHTV1_STATE_OFF)
				continue;

			//������
			if (lights[cur_light].attr & LIGHTV1_ATTR_AMBIENT)
			{
				rsum += (lights[cur_light].c_ambient.r * rbase) / 256;
				gsum += (lights[cur_light].c_ambient.r * gbase) / 256;
				bsum += (lights[cur_light].c_ambient.r * bbase) / 256;
			}
			//ֱ���
			else if (lights[cur_light].attr & LIGHTV1_ATTR_INFINITE)
			{
				VECTOR4D_SUB(&cur_poly->tvlist[ver2].v, &cur_poly->tvlist[ver1].v, &u);
				VECTOR4D_SUB(&cur_poly->tvlist[ver3].v, &cur_poly->tvlist[ver1].v, &v);

				VECTOR4D_CROSS(&u, &v, &n);

				nl = cur_poly->nlength;

				dp = VECTOR4D_DOT(&n, &lights[cur_light].tdir);
				if (dp > 0)
				{
					i = 128 * dp / nl;
					rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
					gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
					bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
				}
			}
			//���Դ(˥����ʽ��(Ipoint * Cpoint) / (kc+kl*d+kq*d^2))
			else if (lights[cur_light].attr & LIGHTV1_ATTR_POINT)
			{
				VECTOR4D_SUB(&cur_poly->tvlist[ver2].v, &cur_poly->tvlist[ver1].v, &u);
				VECTOR4D_SUB(&cur_poly->tvlist[ver3].v, &cur_poly->tvlist[ver1].v, &v);
				//�淨��
				VECTOR4D_CROSS(&u, &v, &n);
				//���߳���
				nl = cur_poly->nlength;
				//���㵽��Դ����
				VECTOR4D_SUB(&lights[cur_light].tpos, &cur_poly->tvlist[ver1].v, &l);
				//���㵽��Դ����
				dist = VECTOR4D_Length(&l);
				//���
				dp = VECTOR4D_DOT(&n, &l);
				if (dp > 0)
				{
					//˥������
					atten = (lights[cur_light].kc + lights[cur_light].kl*dist + lights[cur_light].kq * dist*dist);
					//ǿ������
					i = 128 * dp / (nl*dist*atten);
					rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
					gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
					bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
				}
			}
			//�۹�Ƽ�
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
			{
				VECTOR4D_SUB(&cur_poly->tvlist[ver2].v, &cur_poly->tvlist[ver1].v, &u);
				VECTOR4D_SUB(&cur_poly->tvlist[ver3].v, &cur_poly->tvlist[ver1].v, &v);
				//�淨��
				VECTOR4D_CROSS(&u, &v, &n);
				//���߳���
				nl = cur_poly->nlength;
				//���㵽��Դ����
				VECTOR4D_SUB(&lights[cur_light].tpos, &cur_poly->tvlist[ver1].v, &l);
				//���㵽��Դ����
				dist = VECTOR4D_Length(&l);
				//���
				dp = VECTOR4D_DOT(&n, &lights[cur_light].tdir);

				if (dp > 0)
				{
					//˥������
					atten = (lights[cur_light].kc + lights[cur_light].kl*dist + lights[cur_light].kq * dist*dist);
					//ǿ������
					i = 128 * dp / (nl*atten);
					rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
					gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
					bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
				}
			}
			//�۹�Ƹ���
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
			{
				VECTOR4D_SUB(&cur_poly->tvlist[ver2].v, &cur_poly->tvlist[ver1].v, &u);
				VECTOR4D_SUB(&cur_poly->tvlist[ver3].v, &cur_poly->tvlist[ver1].v, &v);
				//�淨��
				VECTOR4D_CROSS(&u, &v, &n);
				//���߳���
				nl = cur_poly->nlength;
				//���
				dp = VECTOR4D_DOT(&n, &lights[cur_light].tdir);
				if (dp > 0)
				{
					//���㵽��Դ����
					VECTOR4D_SUB(&lights[cur_light].tpos, &cur_poly->tvlist[ver1].v, &s);
					//���㵽��Դ����
					dists = VECTOR4D_Length(&s);

					float dpsl = VECTOR4D_DOT(&lights[cur_light].tdir, &s) / dists;
					if (dpsl > 0)
					{
						//˥������
						atten = (lights[cur_light].kc + lights[cur_light].kl*dists + lights[cur_light].kq * dists*dists);

						float dpsl_exp = dpsl;
						for (int e_idnex = 1; e_idnex < (int)lights[cur_light].pf; e_idnex++)
							dpsl_exp *= dpsl;
						//ǿ��
						i = (128 * dp * dpsl_exp) / (nl * atten);

						rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
						gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
						bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
					}
				}
			}
			if (rsum > 255) rsum = 255;
			if (gsum > 255) gsum = 255;
			if (bsum > 255) bsum = 255;
			//�洢���պ������ɫ
			cur_poly->lit_color[0] = _RGBTOINT(rsum, gsum, bsum);
		}
	}
}

//gouraud��ɫ���մ���
void LightRenderlistGouraud(RENDERLIST4DV2_PTR renderlist, LIGHTV1_PTR lights, int max_lights)
{
	unsigned int rbase, gbase, bbase = 0;
	unsigned int ri, gi, bi;

	unsigned int rsum[3] = { 0 };
	unsigned int gsum[3] = { 0 };
	unsigned int bsum[3] = { 0 };

	float dp, dist, dists, i, atten;
	VECTOR4D l, s;

	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		POLYF4DV2_PTR cur_poly = &renderlist->poly_data[poly];
		if (!(cur_poly->state & POLY4DV2_STATE_ACTIVE)
			|| (cur_poly->state & POLY4DV2_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV2_STATE_CLIPPED))
			continue;
		//��������Ⱦ�б������½��й��մ���
		SET_BIT(cur_poly->state, POLY4DV2_STATE_LIT);
		//��ȡ��ɫ
		_RGBFROMINT(cur_poly->color, &rbase, &gbase, &bbase);

		for (int ver = 0; ver < 3; ver++)
		{
			rsum[ver] = 0;
			gsum[ver] = 0;
			bsum[ver] = 0;
		}

		for (int cur_light = 0; cur_light < max_lights; cur_light++)
		{
			if (lights[cur_light].state == LIGHTV1_STATE_OFF)
				continue;

			//������
			if (lights[cur_light].attr & LIGHTV1_ATTR_AMBIENT)
			{
				ri = ((lights[cur_light].c_ambient.r * rbase) / 256);
				gi = ((lights[cur_light].c_ambient.g * gbase) / 256);
				bi = ((lights[cur_light].c_ambient.b * bbase) / 256);

				for (int ver = 0; ver < 3; ver++)
				{
					rsum[ver] += ri;
					gsum[ver] += gi;
					bsum[ver] += bi;
				}
			}
			//ֱ���
			else if (lights[cur_light].attr & LIGHTV1_ATTR_INFINITE)
			{
				for (int ver = 0; ver < 3; ver++)
				{
					dp = VECTOR4D_DOT(&cur_poly->tvlist[ver].n, &lights[cur_light].tdir);
					if (dp > 0)
					{
						i = 128 * dp;
						rsum[ver] += ((lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128));
						gsum[ver] += ((lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128));
						bsum[ver] += ((lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128));
					}
				}
			}
			//���Դ(˥����ʽ��(Ipoint * Cpoint) / (kc+kl*d+kq*d^2))
			else if (lights[cur_light].attr & LIGHTV1_ATTR_POINT)
			{
				for (int ver = 0; ver < 3; ver++)
				{
					//���㵽��Դ����
					VECTOR4D_SUB(&lights[cur_light].tpos, &cur_poly->tvlist[ver].v, &l);
					//��Դ�������
					dist = VECTOR4D_Length(&l);

					dp = VECTOR4D_DOT(&cur_poly->tvlist[ver].n, &l);
					if (dp > 0)
					{
						atten = (lights[cur_light].kc + lights[cur_light].kl * dist + lights[cur_light].kq*dist*dist);
						i = 128 * dp / (dist * atten);

						rsum[ver] += ((lights[cur_light].c_diffuse.r * rbase * i) / (128 * 256));
						gsum[ver] += ((lights[cur_light].c_diffuse.g * gbase * i) / (128 * 256));
						bsum[ver] += ((lights[cur_light].c_diffuse.b * bbase * i) / (128 * 256));

					}
				}
			}
			//�۹�Ƽ�
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
			{
				//���㵽��Դ����
				VECTOR4D_SUB(&lights[cur_light].tpos, &cur_poly->tvlist[0].v, &l);
				//��Դ�������
				dist = VECTOR4D_Length(&l);

				for (int ver = 0; ver < 3; ver++)
				{
					dp = VECTOR4D_DOT(&cur_poly->tvlist[ver].n, &lights[cur_light].tdir);
					if (dp > 0)
					{
						atten = (lights[cur_light].kc + lights[cur_light].kl * dist + lights[cur_light].kq*dist*dist);
						i = 128 * dp / (atten);

						rsum[ver] += ((lights[cur_light].c_diffuse.r * rbase * i) / (128 * 256));
						gsum[ver] += ((lights[cur_light].c_diffuse.g * gbase * i) / (128 * 256));
						bsum[ver] += ((lights[cur_light].c_diffuse.b * bbase * i) / (128 * 256));

					}
				}
			}
			//�۹�Ƹ���
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
			{
				for (int ver = 0; ver < 3; ver++)
				{
					dp = VECTOR4D_DOT(&cur_poly->tvlist[ver].n, &lights[cur_light].tdir);
					if (dp>0)
					{
						//���㵽��Դ����
						VECTOR4D_SUB(&lights[cur_light].tpos, &cur_poly->tvlist[ver].v, &s);
						//��Դ���������
						dists = VECTOR4D_Length(&s);

						float dpsp = VECTOR4D_DOT(&lights[cur_light].tdir, &s) / dists;
						if (dpsp > 0)
						{
							atten = (lights[cur_light].kc + lights[cur_light].kl * dists + lights[cur_light].kq*dists*dists);
							float dpsl_exp = dpsp;
							for (int e_index = 1; e_index < lights[cur_light].pf; e_index++)
								dpsl_exp *= dpsp;

							i = (128 * dp * dpsl_exp) / (atten);

							rsum[ver] += (lights[cur_light].c_diffuse.r * rbase * i) / (128 * 256);
							gsum[ver] += (lights[cur_light].c_diffuse.g * gbase * i) / (128 * 256);
							bsum[ver] += (lights[cur_light].c_diffuse.b * bbase * i) / (128 * 256);
						}
					}
				}
			}
		}

		for (int ver = 0; ver < 3; ver++)
		{
			if (rsum[ver] > 255) rsum[ver] = 255;
			if (gsum[ver] > 255) gsum[ver] = 255;
			if (bsum[ver] > 255) bsum[ver] = 255;
		}

		cur_poly->lit_color[0] = _RGBTOINT(rsum[0], gsum[0], bsum[0]);
		cur_poly->lit_color[1] = _RGBTOINT(rsum[1], gsum[1], bsum[1]);
		cur_poly->lit_color[2] = _RGBTOINT(rsum[2], gsum[2], bsum[2]);
	}
}

//z��������alpha���ʱ��Ⱦ
int compare(const void* a, const void* b)
{
	float z1, z2;

	POLYF4DV2_PTR poly1, poly2;

	poly1 = *((POLYF4DV2_PTR*)a);
	poly2 = *((POLYF4DV2_PTR*)b);

	z1 = 0.3333333 * (poly1->tvlist[0].z + poly1->tvlist[1].z + poly1->tvlist[2].z);
	z2 = 0.3333333 * (poly2->tvlist[0].z + poly1->tvlist[1].z + poly1->tvlist[2].z);

	if (z1 > z2)
		return -1;
	else if (z1 < z2)
		return 1;
	else
		return 0; 
}

void RenderlistSortByZ(RENDERLIST4DV2_PTR renderlist)
{
	qsort((void*)(renderlist->poly_ptrs), renderlist->num_polys, sizeof(POLYF4DV2_PTR), compare);
}

void DrawRenderlistLine(POLYF4DV2_PTR face, unsigned char *back_buffer, int back_lpitch)
{
	Draw_Clip_Line(face->tvlist[0].x, face->tvlist[0].y,
		face->tvlist[1].x, face->tvlist[1].y, face->color,
	back_buffer, back_lpitch);

	Draw_Clip_Line(face->tvlist[1].x, face->tvlist[1].y,
		face->tvlist[2].x, face->tvlist[2].y, face->color,
	back_buffer, back_lpitch);

	Draw_Clip_Line(face->tvlist[2].x, face->tvlist[2].y,
		face->tvlist[0].x, face->tvlist[0].y, face->color,
	back_buffer, back_lpitch);
}