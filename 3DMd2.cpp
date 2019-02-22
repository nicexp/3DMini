#include "3DMd2.h"
#include "3DBitmap.h"
#include "3DLog.h"

extern ANIMATION animations[ANIM_STATE_NUM];

//��md2�ļ���ȡ��MESH��
void LoadMd2FileToMesh(MODEL_PTR model, const char* modelpath, const char* skinpath, MESH_PTR mesh)
{
	FILE *fp = NULL;
	int flength = -1; //�ļ�����
	UCHAR *buffer = NULL; //���ݻ���

	MD2_HEADER_PTR md2_header; //md2�ļ�ͷ
	float tx, ty, tz, sx, sy, sz;//����ƽ������������
	
	const char *ext = NULL;
	const char ch = '.';

	//����md2�ļ�
	if ((fp = fopen(modelpath, "rb")) == NULL)
	{
		DEBUG_LOG("open file failed,filename:%s", modelpath);
		return;
	}

	//��ȡ�ļ�����
	fseek(fp, 0, SEEK_END);
	flength = ftell(fp);

	//����ָ���ļ�ͷ
	fseek(fp, 0, SEEK_SET);
	//����ռ�
	buffer = (UCHAR*)malloc(flength*sizeof(UCHAR));
	//��ȡmd2�ļ����ݵ�buffer
	fread(buffer, sizeof(UCHAR), flength, fp);
	//�ļ�ͷָ��ָ��buffer
	md2_header = (MD2_HEADER_PTR)buffer;
	//�ж��ǲ���MD2�ļ�
	if (md2_header->identifier != MD2_MAGIC_NUM || md2_header->version != MD2_VERSION)
	{
		DEBUG_LOG("not md2 file!");
		fclose(fp);
		free(buffer);
		return;
	}
	//��ȡ�ļ�ͷ���е������Ϣ
	int num_frames = (md2_header->num_frames > ANIM_MAX_FRAME) ? ANIM_MAX_FRAME : md2_header->num_frames;
	int num_polys = md2_header->num_polys;
	int num_vertices = md2_header->num_verts;
	int num_textcoords = md2_header->num_textcoords;
	//���б�
	MD2_POLY_PTR md2_polys = (MD2_POLY_PTR)malloc(num_polys * sizeof(MD2_POLY));
	//�����б�
	VECTOR3D_PTR md2_vlist = (VECTOR3D_PTR)malloc(ANIM_MAX_FRAME * num_vertices * sizeof(VECTOR3D));
	//���������б�
	VECTOR2D_PTR md2_tlist = (VECTOR2D_PTR)malloc(num_textcoords * sizeof(VECTOR2D));
	//��ȡ��������
	for (int tindex = 0; tindex < num_textcoords; tindex++)
	{
		md2_tlist[tindex].x = ((MD2_TEXTCOORDS_PTR)(buffer + md2_header->offset_textcoords))[tindex].u;
		md2_tlist[tindex].y = ((MD2_TEXTCOORDS_PTR)(buffer + md2_header->offset_textcoords))[tindex].v;
	}
	//��ȡ����
	for (int findex = 0; findex < num_frames; findex++)
	{
		MD2_FRAME_PTR frame_ptr = (MD2_FRAME_PTR)(buffer + md2_header->offset_frames + md2_header->framesize * findex);
		//��ȡƽ�����Ӻ���������
		tx = frame_ptr->translate[0];
		ty = frame_ptr->translate[1];
		tz = frame_ptr->translate[2];
		sx = frame_ptr->scale[0];
		sy = frame_ptr->scale[1];
		sz = frame_ptr->scale[2];
		for (int vindex = 0; vindex < num_vertices; vindex++)
		{
			VECTOR3D v;

			v.x = (float)frame_ptr->vlist[vindex].v[0] * sx + tx;
			v.y = (float)frame_ptr->vlist[vindex].v[1] * sy + ty;
			v.z = (float)frame_ptr->vlist[vindex].v[2] * sz + tz;

			//�����б�
			md2_vlist[findex * num_vertices + vindex] = v;
		}
	}

	//��ȡ��
	MD2_POLY_PTR poly_ptr = (MD2_POLY_PTR)(buffer + md2_header->offset_polys);

	for (int pindex = 0; pindex < num_polys; pindex++)
	{
		md2_polys[pindex].vindex[0] = poly_ptr[pindex].vindex[0];
		md2_polys[pindex].vindex[1] = poly_ptr[pindex].vindex[1];
		md2_polys[pindex].vindex[2] = poly_ptr[pindex].vindex[2];

		md2_polys[pindex].tindex[0] = poly_ptr[pindex].tindex[0];
		md2_polys[pindex].tindex[1] = poly_ptr[pindex].tindex[1];
		md2_polys[pindex].tindex[2] = poly_ptr[pindex].tindex[2];
	}

	//�ر��ļ�
	fclose(fp);
	//�ͷŻ���
	if (buffer)
		free(buffer);

	//�������������䵽mesh��
	//��ʼ��mesh
	InitMesh(mesh);
	//ģ��
	mesh->parent = model;
	//�ܶ�����
	mesh->cur_frame = 0;
	mesh->num_vertices = num_vertices;
	mesh->num_polys = num_polys;
	mesh->num_frames = 0;
	//֡���붥����
	for (int aindex = 0; aindex < ANIM_STATE_NUM; aindex++)
	{
		ANIMATION_PTR anim = &animations[aindex];
		mesh->num_frames += (anim->endframe - anim->startframe) * anim->interpolation + (anim->endframe - anim->startframe + 1);
	}
	mesh->total_vertices = num_vertices  * mesh->num_frames;
	//����
	mesh->texture = (BITMAP_IMG_PTR)malloc(sizeof(BITMAP_IMG));
	//�����������ͽ��м���
	ext = strrchr(skinpath, ch);
	if (!strcmp(ext+1, "bmp"))
		Load_Bitmap_File(mesh->texture, skinpath);
	else if (!strcmp(ext + 1, "jpg"))
		Load_Jpeg_File(mesh->texture, skinpath);
	else if (!strcmp(ext + 1, "png"))
		Load_Png_File(mesh->texture, skinpath);
	else
		DEBUG_LOG("texture skin is not bmp,jpg,png");
	
	mesh->vlist_local = (VERTEX4DTV1_PTR)malloc(mesh->total_vertices * sizeof(VERTEX4DTV1));
	mesh->vlist_trans = (VERTEX4DTV1_PTR)malloc(num_vertices * sizeof(VERTEX4DTV1));
	mesh->tlist = (POINT2D_PTR)malloc(num_textcoords * sizeof(POINT2D));
	mesh->plist = (POLY4DV2_PTR)malloc(num_polys * sizeof(POLY4DV2));
	mesh->max_radius = (float*)malloc(mesh->num_frames * sizeof(float));
	mesh->center_pos = (VECTOR4D_PTR)malloc(mesh->num_frames * sizeof(VECTOR4D));
	mesh->world_pos = (VECTOR4D_PTR)malloc(mesh->num_frames * sizeof(VECTOR4D));

	memset(mesh->vlist_local, 0, mesh->total_vertices * sizeof(VERTEX4DTV1));
	memset(mesh->vlist_trans, 0, num_vertices * sizeof(VERTEX4DTV1));
	memset(mesh->tlist, 0, num_textcoords * sizeof(POINT2D));
	memset(mesh->plist, 0, num_polys * sizeof(POLY4DV2));
	memset(mesh->max_radius, 0, mesh->num_frames * sizeof(float));
	memset(mesh->center_pos, 0, mesh->num_frames * sizeof(VECTOR4D));
	memset(mesh->world_pos, 0, mesh->num_frames * sizeof(VECTOR4D));

	//��䶥��������Ϣ
	for (int animindex = 0, fcount = 0; animindex < ANIM_STATE_NUM; animindex++)
	{
		ANIMATION_PTR anim = &animations[animindex];
		float rate = 1 / (anim->interpolation + 1);
		for (int findex = anim->startframe; findex <= anim->endframe; findex++)
		{
			//ԭʼ֡
			for (int vindex = 0; vindex < num_vertices; vindex++)
			{
				//ע��˴���Ҫ����y��z����л�����ģ���ļ����õ���������ϵ
				mesh->vlist_local[fcount * num_vertices + vindex].x = md2_vlist[findex * num_vertices + vindex].x * MODEL_POS_TRANS_UNIT;
				mesh->vlist_local[fcount * num_vertices + vindex].y = md2_vlist[findex * num_vertices + vindex].z * MODEL_POS_TRANS_UNIT;
				mesh->vlist_local[fcount * num_vertices + vindex].z = md2_vlist[findex * num_vertices + vindex].y * MODEL_POS_TRANS_UNIT;
				mesh->vlist_local[fcount * num_vertices + vindex].w = 1;
			}
			fcount++;//��һ֡
			//��ֵ֡
			if (findex < anim->endframe)
			{
				int curbase_index = findex * num_vertices;
				int nextbase_index = (findex + 1) * num_vertices;
				for (int rindex = 0; rindex < anim->interpolation; rindex++)
				{
					for (int vindex = 0; vindex < num_vertices; vindex++)
					{
						//ע��˴���Ҫ����y��z����л�����ģ���ļ����õ���������ϵ
						mesh->vlist_local[fcount * num_vertices + vindex].x = ((1 - rate) * md2_vlist[curbase_index + vindex].x + rate * md2_vlist[nextbase_index + vindex].x) * MODEL_POS_TRANS_UNIT;
						mesh->vlist_local[fcount * num_vertices + vindex].y = ((1 - rate) * md2_vlist[curbase_index + vindex].z + rate * md2_vlist[nextbase_index + vindex].z) * MODEL_POS_TRANS_UNIT;
						mesh->vlist_local[fcount * num_vertices + vindex].z = ((1 - rate) * md2_vlist[curbase_index + vindex].y + rate * md2_vlist[nextbase_index + vindex].y) * MODEL_POS_TRANS_UNIT;
						mesh->vlist_local[fcount * num_vertices + vindex].w = 1;
					}
					fcount++; //��һ֡
				}
			}
		}
	}
	//��䶥�㷨����Ϣ
	int* polys_touch_vertex = (int*)malloc(num_vertices * sizeof(int));
	VECTOR4D u, v, n;
	int ver0, ver1, ver2;
	for (int findex = 0; findex < mesh->num_frames; findex++)
	{
		//����polys_touch_vertex
		memset(polys_touch_vertex, 0, num_vertices * sizeof(int));
		VERTEX4DTV1_PTR vlistptr = (mesh->vlist_local + findex*num_vertices);
		for (int pindex = 0; pindex < num_polys; pindex++)
		{
			ver0 = md2_polys[pindex].vindex[0];
			ver1 = md2_polys[pindex].vindex[1];
			ver2 = md2_polys[pindex].vindex[2];


			VECTOR4D_SUB(&vlistptr[ver1].v, &vlistptr[ver0].v, &u);
			VECTOR4D_SUB(&vlistptr[ver2].v, &vlistptr[ver0].v, &v);
			VECTOR4D_CROSS(&u, &v, &n);

			VECTOR4D_ADD(&vlistptr[ver0].n, &n, &vlistptr[ver0].n);
			VECTOR4D_ADD(&vlistptr[ver1].n, &n, &vlistptr[ver1].n);
			VECTOR4D_ADD(&vlistptr[ver2].n, &n, &vlistptr[ver2].n);

			polys_touch_vertex[ver0]++;
			polys_touch_vertex[ver1]++;
			polys_touch_vertex[ver2]++;
		}

		for (int vindex = 0; vindex < num_vertices; vindex++)
		{
			vlistptr[vindex].nx /= polys_touch_vertex[vindex];
			vlistptr[vindex].ny /= polys_touch_vertex[vindex];
			vlistptr[vindex].nz /= polys_touch_vertex[vindex];

			VECTOR4D_Normalize(&vlistptr[vindex].n);
		}
	}
	//���ÿһ֡�����뾶
	for (int findex = 0; findex < mesh->num_frames; findex++)
	{
		float min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
		VERTEX4DTV1_PTR vlistptr = (mesh->vlist_local + findex*num_vertices);
		for (int vindex = 0; vindex < num_vertices; vindex++)
		{
			min_x = min(min_x, vlistptr[vindex].x);
			max_x = max(max_x, vlistptr[vindex].x);
			min_y = min(min_y, vlistptr[vindex].y);
			max_y = max(max_y, vlistptr[vindex].y);
			min_z = min(min_z, vlistptr[vindex].z);
			max_z = max(max_z, vlistptr[vindex].z);
		}
		
		*(mesh->max_radius + findex) = sqrtf(((max_x - min_x)*(max_x - min_x)) + ((max_y - min_y)*(max_y - min_y)) + ((max_z - min_z)*(max_z - min_z))) / 2;
		VECTOR4D_INITXYZ(&mesh->center_pos[findex], (min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	}
	//������������б�
	for (int tindex = 0; tindex < num_textcoords; tindex++)
	{
		mesh->tlist[tindex].x = md2_tlist[tindex].x;
		mesh->tlist[tindex].y = md2_tlist[tindex].y;
	}
	//����������б�
	for (int pindex = 0; pindex < num_polys; pindex++)
	{
		mesh->plist[pindex].attr = 0;
		mesh->plist[pindex].state = 0;
		mesh->plist[pindex].color = _RGBTOINT(255, 255, 255);
		mesh->plist[pindex].texture = mesh->texture;

		SET_BIT(mesh->plist[pindex].state, POLY4DV2_STATE_ACTIVE);
		SET_BIT(mesh->plist[pindex].attr, POLY4DV2_ATTR_SHAD_MODE_TEXTURE);

		mesh->plist[pindex].vlist = mesh->vlist_trans;
		mesh->plist[pindex].tlist = mesh->tlist;

		for (int idx = 0; idx < 3; idx++)
		{
			mesh->plist[pindex].vert[idx] = md2_polys[pindex].vindex[idx];
			mesh->plist[pindex].text[idx] = md2_polys[pindex].tindex[idx];
		}

		ComputePolyNormals(&mesh->plist[pindex]);
	}

	free(md2_polys);
	free(md2_tlist);
	free(md2_vlist);
	free(polys_touch_vertex);
}