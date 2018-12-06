#include "3DObject2.h"

void InitObject(OBJECT4DV2_PTR obj)
{
	obj->id = 0;

	SET_BIT(obj->state, OBJECT4DV2_STATE_ACTIVE);
	SET_BIT(obj->state, OBJECT4DV2_STATE_VISIBLE);

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
		obj->vlist_local[i].x = temp_verts[i].x;
		obj->vlist_local[i].y = temp_verts[i].y;
		obj->vlist_local[i].z = temp_verts[i].z;
		obj->vlist_local[i].w = 1;
		//颜色
		obj->vlist_local[i].i = _RGBTOINT(255, 255, 255);
	}
	//多边形数量
	obj->num_polys = 12;
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
	//初始化纹理坐标
	int text_width = 128 - 1;
	POINT2D temp_poly_text[12*3] =
	{
		{ text_width, 0 }, { text_width, text_width }, { 0, text_width },
		{ text_width, 0 }, { 0, text_width }, {0, 0},
		{ text_width, 0 }, { 0, text_width }, {0, 0},
		{ text_width, 0 }, { text_width, text_width }, { 0, text_width },
		{ text_width, 0 }, { text_width, text_width }, { 0, text_width },
		{ text_width, 0 }, { 0, text_width }, {0, 0},
		{ text_width, 0 }, { text_width, text_width }, { 0, text_width },
		{ text_width, 0 }, { 0, text_width }, { 0, 0 },
		{ 0, 0 }, { text_width, text_width }, { 0, text_width },
		{ 0, 0 }, { text_width, 0 }, { text_width, text_width },
		{ text_width, 0 }, { text_width, text_width }, { 0, text_width },
		{ 0, 0 }, { text_width, 0 }, { 0, text_width }
	};
	//初始化每个三角形
	for (int tri = 0; tri < 12; tri++)
	{
		obj->plist[tri].attr = 0;
		obj->plist[tri].color = _RGBTOINT(255, 255, 255);
		SET_BIT(obj->plist[tri].state, POLY4DV2_STATE_ACTIVE);
		SET_BIT(obj->plist[tri].attr, POLY4DV2_ATTR_SHAD_MODE_TEXTURE);
		
		obj->plist[tri].vlist = obj->vlist_trans;

		obj->plist[tri].vert[0] = temp_poly_indices[3 * tri + 0];
		obj->plist[tri].vert[1] = temp_poly_indices[3 * tri + 1];
		obj->plist[tri].vert[2] = temp_poly_indices[3 * tri + 2];

		obj->plist[tri].text[0] = tri * 3 + 0;
		obj->plist[tri].text[1] = tri * 3 + 1;
		obj->plist[tri].text[2] = tri * 3 + 2;

		obj->tlist[3 * tri + 0] = temp_poly_text[3 * tri + 0];
		obj->tlist[3 * tri + 1] = temp_poly_text[3 * tri + 1];
		obj->tlist[3 * tri + 2] = temp_poly_text[3 * tri + 2];

		obj->plist[tri].tlist = obj->tlist;
	}

	ComputeObject2PolyNormals(obj);//计算面法线
	ComputeObject2VertexNormals(obj);//计算顶点法线
}

//物体进行矩阵变换运算
void Transform_Object(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mt, int coord_select, int bTransformDir, int bNormal)
{
	MATRIX4X4 mr;//去除平移操作，用于法线向量
	memcpy((void*)&mr, (void*)mt, sizeof(MATRIX4X4));
	mr.M30 = mr.M31 = mr.M32 = 0;

	switch (coord_select)
	{
	case TRANSFORM_LOCAL_ONLY:
	{
								 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
								 {
									 POINT4D presult;
									 Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].v, mt, &presult);
									 VECTOR4D_COPY(&obj->vlist_local[vertex].v, &presult);
									 if (bNormal)
									 {
										 Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].n, &mr, &presult);
										 VECTOR4D_COPY(&obj->vlist_local[vertex].n, &presult);
									 }
								 }
								 break;
	}
	case TRANSFORM_TRANS_ONLY:
	{
								 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
								 {
									 POINT4D presult;
									 Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex].v, mt, &presult);
									 VECTOR4D_COPY(&obj->vlist_trans[vertex].v, &presult);
									 if (bNormal)
									 {
										 Mat_Mul_VECTOR4D_4X4(&obj->vlist_trans[vertex].n, &mr, &presult);
										 VECTOR4D_COPY(&obj->vlist_trans[vertex].n, &presult);
									 }
								 }
								 break;
	}
	case TRANSFORM_LOCAL_TO_TRANS:
	{
									 for (int vertex = 0; vertex < obj->num_vertices; vertex++)
									 {
										 POINT4D presult;
										 Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].v, mt, &presult);
										 VECTOR4D_COPY(&obj->vlist_trans[vertex].v, &presult);
										 if (bNormal)
										 {
											 Mat_Mul_VECTOR4D_4X4(&obj->vlist_local[vertex].n, &mr, &presult);
											 VECTOR4D_COPY(&obj->vlist_trans[vertex].n, &presult);
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
		Mat_Mul_VECTOR4D_4X4(&obj->ux, mt, &vresult);
		VECTOR4D_COPY(&obj->ux, &vresult);
		Mat_Mul_VECTOR4D_4X4(&obj->uy, mt, &vresult);
		VECTOR4D_COPY(&obj->uy, &vresult);
		Mat_Mul_VECTOR4D_4X4(&obj->uz, mt, &vresult);
		VECTOR4D_COPY(&obj->uz, &vresult);
	}
}

//局部坐标转换为世界坐标
void ModelToWorldObj(OBJECT4DV2_PTR obj)
{
	//世界坐标转换矩阵
	MATRIX4X4 matworld;
	Mat_Init_4X4(&matworld,
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		obj->world_pos.x, obj->world_pos.y, obj->world_pos.z, 1);

	Transform_Object(obj, &matworld, TRANSFORM_LOCAL_TO_TRANS, 1, 1);
}

//世界坐标转换相机坐标
void WorldToCameraObj(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mcam)
{
	Transform_Object(obj, mcam, TRANSFORM_TRANS_ONLY, 1, 1);
}

//相机坐标转换为透视坐标
void CameraToPerspectObj(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mper)
{
	Transform_Object(obj, mper, TRANSFORM_TRANS_ONLY, 1, 0);
	ConvertObjByW(obj);
}

//透视坐标转换成屏幕坐标
void PerspectToScreenObj(OBJECT4DV2_PTR obj, MATRIX4X4_PTR mscr)
{
	Transform_Object(obj, mscr, TRANSFORM_TRANS_ONLY, 1, 0);
}

//物体背面消除
void RemoveObjBackface(OBJECT4DV2_PTR obj, POINT4D_PTR cam_pos)
{
	//计算多边形面法线
	VECTOR4D u, v, n;
	//指向视点的向量
	VECTOR4D view;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR curpoly = &obj->plist[poly];
		//取顶点
		int vertex_idx1 = curpoly->vert[0];
		int vertex_idx2 = curpoly->vert[1];
		int vertex_idx3 = curpoly->vert[2];
		//取u和v
		VECTOR4D_SUB(&curpoly->vlist[vertex_idx2].v, &curpoly->vlist[vertex_idx1].v, &u);
		VECTOR4D_SUB(&curpoly->vlist[vertex_idx3].v, &curpoly->vlist[vertex_idx2].v, &v);
		//求法线n
		VECTOR4D_CROSS(&u, &v, &n);
		//指向视点
		VECTOR4D_SUB(cam_pos, &curpoly->vlist[vertex_idx1].v, &view);
		//计算点积
		float dp = VECTOR4D_DOT(&n, &view);
		if (dp <= 0.0)
			SET_BIT(curpoly->state, POLY4DV2_STATE_BACKFACE);
	}
}

//物体齐次坐标转换
void ConvertObjByW(OBJECT4DV2_PTR obj)
{
	for (int k = 0; k < obj->num_vertices; k++)
	{
		VECTOR4D_DIV_BY_W(&obj->vlist_trans[k].v);
	}
}

//剔除物体
int CullObj(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, int cull_flags)
{
	POINT4D sphere_pos; //包围球坐标

	//将包围球转换相机坐标
	Mat_Mul_VECTOR4D_4X4(&obj->world_pos, &cam->mcam, &sphere_pos);

	if (cull_flags & CULL_OBJECT_Z_PLANE)
	{
		if ((sphere_pos.z - obj->max_radius > cam->far_clip_z)
			|| (sphere_pos.z + obj->max_radius < cam->near_clip_z))
		{
			SET_BIT(obj->state, OBJECT4DV2_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_X_PLANE)
	{
		float z_test = (cam->viewplane_width / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.x - obj->max_radius > z_test)
			|| (sphere_pos.x + obj->max_radius < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV2_STATE_CULLED);
			return 1;
		}
	}

	if (cull_flags & CULL_OBJECT_Y_PLANE)
	{
		float z_test = (cam->viewplane_height / 2) * sphere_pos.z / cam->view_dist;

		if ((sphere_pos.y - obj->max_radius > z_test)
			|| (sphere_pos.y + obj->max_radius < -z_test))
		{
			SET_BIT(obj->state, OBJECT4DV2_STATE_CULLED);
			return 1;
		}
	}

	return 0;
}

//重置物体状态
void ResetObjState(OBJECT4DV2_PTR obj)
{
	RESET_BIT(obj->state, OBJECT4DV2_STATE_CULLED);

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &obj->plist[poly];

		RESET_BIT(cur_poly->state, POLY4DV2_STATE_CLIPPED);
		RESET_BIT(cur_poly->state, POLY4DV2_STATE_BACKFACE);
	}
}

//物体渲染3D流水线version1
void Object3DLine(OBJECT4DV2_PTR obj, CAM4DV1_PTR cam, LIGHTV1_PTR lights)
{
	ModelToWorldObj(obj);//世界坐标
	RemoveObjBackface(obj, &cam->pos);//消除背面

	//恒定着色光照处理start
	//ComputeObject2PolyNormals(obj);
	//LightObject2ByFlat(obj, lights, 4);
	//恒定着色光照处理end

	//gouraud着色光照处理start
	ComputeObject2VertexNormals(obj);
	LightObject2ByGouraud(obj, lights, 4);
	//gouraud着色光照处理end

	WorldToCameraObj(obj, &cam->mcam);//相机坐标
	CameraToPerspectObj(obj, &cam->mper);//透视坐标
	PerspectToScreenObj(obj, &cam->mscr);//屏幕坐标
}

//计算物体顶点法线用于光照计算
void ComputeObject2VertexNormals(OBJECT4DV2_PTR obj)
{
	if (!obj)
		return;

	int polys_touch_vertex[OBJECT4DV2_MAX_VERTICES];
	memset(polys_touch_vertex, 0, sizeof(polys_touch_vertex));

	int ver0, ver1, ver2;
	VECTOR4D u, v, n;
	POLY4DV2_PTR cur_poly;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		cur_poly = &obj->plist[poly];

		ver0 = cur_poly->vert[0];
		ver1 = cur_poly->vert[1];
		ver2 = cur_poly->vert[2];


		VECTOR4D_SUB(&obj->vlist_local[ver1].v, &obj->vlist_local[ver0].v, &u);
		VECTOR4D_SUB(&obj->vlist_local[ver2].v, &obj->vlist_local[ver0].v, &v);
		VECTOR4D_CROSS(&u, &v, &n);

		VECTOR4D_ADD(&obj->vlist_local[ver0].n, &n, &obj->vlist_local[ver0].n);
		VECTOR4D_ADD(&obj->vlist_local[ver1].n, &n, &obj->vlist_local[ver1].n);
		VECTOR4D_ADD(&obj->vlist_local[ver2].n, &n, &obj->vlist_local[ver2].n);

		polys_touch_vertex[ver0]++;
		polys_touch_vertex[ver1]++;
		polys_touch_vertex[ver2]++;
	}

	for (int vertex = 0; vertex < obj->num_polys; vertex++)
	{
		obj->vlist_local[vertex].nx /= polys_touch_vertex[vertex];
		obj->vlist_local[vertex].ny /= polys_touch_vertex[vertex];
		obj->vlist_local[vertex].nz /= polys_touch_vertex[vertex];

		VECTOR4D_Normalize(&obj->vlist_local[vertex].n);
	}
}

//计算物体平面法线
void ComputeObject2PolyNormals(OBJECT4DV2_PTR obj)
{
	if (!obj)
		return;

	int ver0, ver1, ver2;
	VECTOR4D u, v, n;
	POLY4DV2_PTR cur_poly;

	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		cur_poly = &obj->plist[poly];

		ver0 = cur_poly->vert[0];
		ver1 = cur_poly->vert[1];
		ver2 = cur_poly->vert[2];


		VECTOR4D_SUB(&obj->vlist_local[ver1].v, &obj->vlist_local[ver0].v, &u);
		VECTOR4D_SUB(&obj->vlist_local[ver2].v, &obj->vlist_local[ver0].v, &v);
		VECTOR4D_CROSS(&u, &v, &n);

		cur_poly->nlength = VECTOR4D_Length(&n);
	}
}

//对恒定着色物体执行光照处理
void LightObject2ByFlat(OBJECT4DV2_PTR obj, LIGHTV1_PTR lights, int max_lights)
{
	unsigned int rbase, gbase, bbase = 0;
	unsigned int rsum, gsum, bsum;

	float dp, dist, dists, i, nl, atten;
	VECTOR4D u, v, n, l, s;

	int ver1, ver2, ver3;

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
		//避免在渲染列表中重新进行光照处理
		SET_BIT(cur_poly->state, POLY4DV2_STATE_LIT);
		//提取颜色
		_RGBFROMINT(cur_poly->color, &rbase, &gbase, &bbase);

		rsum = 0;
		gsum = 0;
		bsum = 0;

		ver1 = cur_poly->vert[0];
		ver2 = cur_poly->vert[1];
		ver3 = cur_poly->vert[2];

		for (int cur_light = 0; cur_light < max_lights; cur_light++)
		{
			if (lights[cur_light].state == LIGHTV1_STATE_OFF)
				continue;

			//环境光
			if (lights[cur_light].attr & LIGHTV1_ATTR_AMBIENT)
			{
				rsum += (lights[cur_light].c_ambient.r * rbase) / 256;
				gsum += (lights[cur_light].c_ambient.r * gbase) / 256;
				bsum += (lights[cur_light].c_ambient.r * bbase) / 256;
			}
			//直射光
			else if (lights[cur_light].attr & LIGHTV1_ATTR_INFINITE)
			{
				VECTOR4D_SUB(&obj->vlist_trans[ver2].v, &obj->vlist_trans[ver1].v, &u);
				VECTOR4D_SUB(&obj->vlist_trans[ver3].v, &obj->vlist_trans[ver1].v, &v);

				VECTOR4D_CROSS(&u, &v, &n);

				nl = cur_poly->nlength;

				dp = VECTOR4D_DOT(&n, &lights[cur_light].dir);
				if (dp > 0)
				{
					i = 128 * dp / nl;
					rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
					gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
					bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
				}
			}
			//点光源(衰减公式：(Ipoint * Cpoint) / (kc+kl*d+kq*d^2))
			else if (lights[cur_light].attr & LIGHTV1_ATTR_POINT)
			{
				VECTOR4D_SUB(&obj->vlist_trans[ver2].v, &obj->vlist_trans[ver1].v, &u);
				VECTOR4D_SUB(&obj->vlist_trans[ver3].v, &obj->vlist_trans[ver1].v, &v);
				//面法线
				VECTOR4D_CROSS(&u, &v, &n);
				//法线长度
				nl = cur_poly->nlength;
				//顶点到光源向量
				VECTOR4D_SUB(&lights[cur_light].pos, &obj->vlist_trans[ver1].v, &l);
				//顶点到光源距离
				dist = VECTOR4D_Length(&l);
				//点积
				dp = VECTOR4D_DOT(&n, &l);

				if (dp > 0)
				{
					//衰减因子
					atten = (lights[cur_light].kc + lights[cur_light].kl*dist + lights[cur_light].kq * dist*dist);
					//强度因子
					i = 128 * dp / (nl*dist*atten);
					rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
					gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
					bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
				}
			}
			//聚光灯简单
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
			{
				VECTOR4D_SUB(&obj->vlist_trans[ver2].v, &obj->vlist_trans[ver1].v, &u);
				VECTOR4D_SUB(&obj->vlist_trans[ver3].v, &obj->vlist_trans[ver1].v, &v);
				//面法线
				VECTOR4D_CROSS(&u, &v, &n);
				//法线长度
				nl = cur_poly->nlength;
				//顶点到光源向量
				VECTOR4D_SUB(&lights[cur_light].pos, &obj->vlist_trans[ver1].v, &l);
				//顶点到光源距离
				dist = VECTOR4D_Length(&l);
				//点积
				dp = VECTOR4D_DOT(&n, &lights[cur_light].dir);

				if (dp > 0)
				{
					//衰减因子
					atten = (lights[cur_light].kc + lights[cur_light].kl*dist + lights[cur_light].kq * dist*dist);
					//强度因子
					i = 128 * dp / (nl*atten);
					rsum += (lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128);
					gsum += (lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128);
					bsum += (lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128);
				}
			}
			//聚光灯复杂
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
			{
				VECTOR4D_SUB(&obj->vlist_trans[ver2].v, &obj->vlist_trans[ver1].v, &u);
				VECTOR4D_SUB(&obj->vlist_trans[ver3].v, &obj->vlist_trans[ver1].v, &v);
				//面法线
				VECTOR4D_CROSS(&u, &v, &n);
				//法线长度
				nl = cur_poly->nlength;
				//点积
				dp = VECTOR4D_DOT(&n, &lights[cur_light].dir);
				if (dp > 0)
				{
					//顶点到光源向量
					VECTOR4D_SUB(&lights[cur_light].pos, &obj->vlist_trans[ver1].v, &s);
					//顶点到光源距离
					dists = VECTOR4D_Length(&s);

					float dpsl = VECTOR4D_DOT(&lights[cur_light].dir, &s) / dists;
					if (dpsl > 0)
					{
						//衰减因子
						atten = (lights[cur_light].kc + lights[cur_light].kl*dists + lights[cur_light].kq * dists*dists);

						float dpsl_exp = dpsl;
						for (int e_idnex = 1; e_idnex < (int)lights[cur_light].pf; e_idnex++)
							dpsl_exp *= dpsl;
						//强度
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
			//存储光照后的面颜色
			cur_poly->lit_color[0] = _RGBTOINT(rsum, gsum, bsum);
		}
	}
}

//对Gouraud着色光照处理
void LightObject2ByGouraud(OBJECT4DV2_PTR obj, LIGHTV1_PTR lights, int max_lights)
{
	unsigned int rbase, gbase, bbase = 0;
	unsigned int ri, gi, bi;

	unsigned int rsum[3] = { 0 };
	unsigned int gsum[3] = { 0 };
	unsigned int bsum[3] = { 0 };

	float dp, dist, dists, i, atten;
	VECTOR4D l, s;

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
		//避免在渲染列表中重新进行光照处理
		SET_BIT(cur_poly->state, POLY4DV2_STATE_LIT);
		//提取颜色
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

			//环境光
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
			//直射光
			else if (lights[cur_light].attr & LIGHTV1_ATTR_INFINITE)
			{
				for (int ver = 0; ver < 3; ver++)
				{
					dp = VECTOR4D_DOT(&obj->vlist_trans[cur_poly->vert[ver]].n, &lights[cur_light].dir);
					if (dp > 0)
					{
						i = 128 * dp;
						rsum[ver] += ((lights[cur_light].c_diffuse.r * rbase * i) / (256 * 128));
						gsum[ver] += ((lights[cur_light].c_diffuse.g * gbase * i) / (256 * 128));
						bsum[ver] += ((lights[cur_light].c_diffuse.b * bbase * i) / (256 * 128));
					}
				}
			}
			//点光源(衰减公式：(Ipoint * Cpoint) / (kc+kl*d+kq*d^2))
			else if (lights[cur_light].attr & LIGHTV1_ATTR_POINT)
			{
				for (int ver = 0; ver < 3; ver++)
				{
					//顶点到光源向量
					VECTOR4D_SUB(&lights[cur_light].pos, &obj->vlist_trans[cur_poly->vert[ver]].v, &l);
					//光源到面距离
					dist = VECTOR4D_Length(&l);

					dp = VECTOR4D_DOT(&obj->vlist_trans[cur_poly->vert[ver]].n, &l);
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
			//聚光灯简单
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT1)
			{
				//顶点到光源向量
				VECTOR4D_SUB(&lights[cur_light].pos, &obj->vlist_trans[cur_poly->vert[0]].v, &l);
				//光源到面距离
				dist = VECTOR4D_Length(&l);

				for (int ver = 0; ver < 3; ver++)
				{
					dp = VECTOR4D_DOT(&obj->vlist_trans[cur_poly->vert[ver]].n, &lights[cur_light].dir);
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
			//聚光灯复杂
			else if (lights[cur_light].attr & LIGHTV1_ATTR_SPOTLIGHT2)
			{
				for (int ver = 0; ver < 3; ver++)
				{
					dp = VECTOR4D_DOT(&obj->vlist_trans[cur_poly->vert[ver]].n, &lights[cur_light].dir);
					if (dp>0)
					{
						//顶点到光源向量
						VECTOR4D_SUB(&lights[cur_light].pos, &obj->vlist_trans[cur_poly->vert[ver]].v, &s);
						//光源到顶点距离
						dists = VECTOR4D_Length(&s);

						float dpsp = VECTOR4D_DOT(&lights[cur_light].dir, &s) / dists;
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

void UpdateObjectPosAndDir(OBJECT4DV2_PTR obj)
{
	static float view_angle = 1;
	MATRIX4X4 m_inv, m_inv2, m_inv3;

	double theta = view_angle / 180 * PI;
	double cos_theta = cos(theta);
	double sin_theta = -sin(theta);

	Mat_Init_4X4(&m_inv, 1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);

	cos_theta = cos(theta);
	sin_theta = -sin(theta);
	Mat_Init_4X4(&m_inv2, cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&m_inv, &m_inv2, &m_inv3);

	Transform_Object(obj, &m_inv3, TRANSFORM_LOCAL_ONLY, 1, 1);
}