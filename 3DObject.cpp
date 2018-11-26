#include "3DObject.h"

void InitObject(OBJECT4DV1_PTR obj)
{
	obj->id = 0;

	SET_BIT(obj->state, OBJECT4DV1_STATE_ACTIVE);
	SET_BIT(obj->state, OBJECT4DV1_STATE_VISIBLE);

	obj->attr = 0;
	obj->num_vertices = 8;

	obj->avg_radius = BORDER_LENGTH * 1.732;
	obj->max_radius = BORDER_LENGTH * 1.732;

	obj->world_pos.x = 0;
	obj->world_pos.y = 0;
	obj->world_pos.z = 0;
	obj->world_pos.w = 1;

	obj->dir.x = 0;
	obj->dir.y = 0;
	obj->dir.z = 1;
	obj->dir.w = 1;

	VECTOR4D_INITXYZ(&obj->ux, 1, 0, 0);
	VECTOR4D_INITXYZ(&obj->uy, 0, 1, 0);
	VECTOR4D_INITXYZ(&obj->uz, 0, 0, 1);

	//�߳�
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
	//���ö�������
	for (int i = 0; i < 8; i++)
	{
		obj->vlist_local[i].x = temp_verts[i].x;
		obj->vlist_local[i].y = temp_verts[i].y;
		obj->vlist_local[i].z = temp_verts[i].z;
		obj->vlist_local[i].w = 1;
	}
	//���������
	obj->num_polys = 12;
	//����һ�������εĶ����б�
	int temp_poly_indices[12 * 3] =
	{
		0, 1, 2, 0, 2, 3,
		0, 7, 1, 0, 4, 7,
		1, 7, 6, 1, 6, 2,
		2, 6, 5, 2, 5, 3,
		0, 5, 4, 0, 3, 5,
		5, 6, 7, 4, 5, 7
	};
	//��ʼ��ÿ��������
	for (int tri = 0; tri < 12; tri++)
	{
		SET_BIT(obj->plist[tri].state, POLY4DV1_STATE_ACTIVE);
		obj->plist[tri].attr = 0;
		obj->plist[tri].color = _RGBTOINT(255, 255, 255);

		obj->plist[tri].vlist = obj->vlist_trans;

		obj->plist[tri].vert[0] = temp_poly_indices[3 * tri + 0];
		obj->plist[tri].vert[1] = temp_poly_indices[3 * tri + 1];
		obj->plist[tri].vert[2] = temp_poly_indices[3 * tri + 2];
	}
}

//������о���任����
void Transform_Object(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mt, int coord_select, int bTransformDir)
{
	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
								 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
								 {
									 POINT4D presult;
									 Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &presult);
									 VECTOR4D_COPY(&obj->vlist_local[vertex], &presult);
								 }
								 break;
	}
	case TRANSFORM_TRANS_ONLY:
	{
								 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
								 {
									 POINT4D presult;
									 Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex], mt, &presult);
									 VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
								 }
								 break;
	}
	case TRANSFORM_LOCAL_TO_TRANS:
	{
									 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
									 {
										 POINT4D presult;
										 Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex], mt, &presult);
										 VECTOR4D_COPY(&obj->vlist_trans[vertex], &presult);
									 }
									 break;
	}
	default:
		break;
	}
	//��ת���峯��
	if (bTransformDir)
	{
		VECTOR4D vresult;
		Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		VECTOR4D_COPY(&obj->ux, &vresult);
		Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		VECTOR4D_COPY(&obj->uy, &vresult);
		Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);
		VECTOR4D_COPY(&obj->uz, &vresult);
	}
}

//�ֲ�����ת��Ϊ��������
void ModelToWorldObj(OBJECT4DV1_PTR obj)
{
	//��������ת������
	MATRIX4X4 matworld;
	Mat_Init_4X4(&matworld,
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		obj->world_pos.x, obj->world_pos.y, obj->world_pos.z, 1);

	Transform_Object(obj, &matworld, TRANSFORM_LOCAL_TO_TRANS, 1);
}

//��������ת���������
void WorldToCameraObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mcam)
{
	Transform_Object(obj, mcam, TRANSFORM_TRANS_ONLY, 1);
}

//�������ת��Ϊ͸������
void CameraToPerspectObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mper)
{
	Transform_Object(obj, mper, TRANSFORM_TRANS_ONLY, 1);
	ConvertObjByW(obj);
}

//͸������ת������Ļ����
void PerspectToScreenObj(OBJECT4DV1_PTR obj, MATRIX4X4_PTR mscr)
{
	Transform_Object(obj, mscr, TRANSFORM_TRANS_ONLY, 1);
}

//���屳������
void RemoveObjBackface(OBJECT4DV1_PTR obj, POINT4D_PTR cam_pos)
{
	//���������淨��
	VECTOR4D u, v, n;
	//ָ���ӵ������
	VECTOR4D view;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR curpoly = &obj->plist[poly];
		//ȡ����
		int vertex_idx1 = curpoly->vert[0];
		int vertex_idx2 = curpoly->vert[1];
		int vertex_idx3 = curpoly->vert[2];
		//ȡu��v
		VECTOR4D_SUB(&curpoly->vlist[vertex_idx2], &curpoly->vlist[vertex_idx1], &u);
		VECTOR4D_SUB(&curpoly->vlist[vertex_idx3], &curpoly->vlist[vertex_idx2], &v);
		//����n
		VECTOR4D_CROSS(&u, &v, &n);
		//ָ���ӵ�
		VECTOR4D_SUB(cam_pos, &curpoly->vlist[vertex_idx1], &view);
		//������
		float dp = VECTOR4D_DOT(&n, &view);
		if (dp <= 0.0)
			SET_BIT(curpoly->state, POLY4DV1_STATE_BACKFACE);
	}
}

//�����������ת��
void ConvertObjByW(OBJECT4DV1_PTR obj)
{
	for (int k = 0; k < obj->num_vertices; k++)
	{
		VECTOR4D_DIV_BY_W(&obj->vlist_trans[k]);
	}
}

//�޳�����
int CullObj(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam, int cull_flags)
{
	POINT4D sphere_pos; //��Χ������

	//����Χ��ת���������
	Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);

	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		if ((sphere_pos.z - obj->max_radius > cam->far_clip_z)
			|| (sphere_pos.z + obj->max_radius < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		float z_test = (cam->viewplane_width / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.x - obj->max_radius > z_test)
			|| (sphere_pos.x + obj->max_radius < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		float z_test = (cam->viewplane_height / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.y - obj->max_radius > z_test)
			|| (sphere_pos.y + obj->max_radius < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);
			return 1;
		}
	}

	return 0;
}

//��������״̬
void ResetObjState(OBJECT4DV1_PTR obj)
{
	RESET_BIT(obj->state, OBJECT4DV1_STATE_CULLED);

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR cur_poly = &obj->plist[poly];

		RESET_BIT(cur_poly->state, POLY4DV1_STATE_CLIPPED);
		RESET_BIT(cur_poly->state, POLY4DV1_STATE_BACKFACE);
	}
}

//������Ⱦ3D��ˮ��version1
void Object3DLine(OBJECT4DV1_PTR obj, CAM4DV1_PTR cam)
{
	ModelToWorldObj(obj);//��������
	RemoveObjBackface(obj, &cam->pos);//��������
	WorldToCameraObj(obj, &cam->mcam);//�������
	CameraToPerspectObj(obj, &cam->mper);//͸������
	PerspectToScreenObj(obj, &cam->mscr);//��Ļ����
}