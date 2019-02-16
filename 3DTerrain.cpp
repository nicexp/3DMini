#include "3DTerrain.h"
#include "3DBitmap.h"

void GenerateTerrain(TERRAIN_PTR terrain, float width, float length, float maxheight, const char* fileheightmap, const char* filetextmap)
{
	char heightpath[256], textpath[256];
	GetCurrentDirectory(256, heightpath);
	strcpy(textpath, heightpath);
	strcat(heightpath, fileheightmap);
	strcat(textpath, filetextmap);

	memset(terrain, 0, sizeof(terrain));

	terrain->width = width;
	terrain->length = length;
	terrain->maxheight = maxheight;
	
	MESH_PTR mesh = &terrain->mesh;
	//初始化mesh
	InitMesh(mesh);

	mesh->num_frames = 1;
	mesh->cur_frame = 0;
	
	BITMAP_IMG_PTR heightmap = (BITMAP_IMG_PTR)malloc(sizeof(BITMAP_IMG));
	BITMAP_IMG_PTR textmap = (BITMAP_IMG_PTR)malloc(sizeof(BITMAP_IMG));
	memset(heightmap, 0, sizeof(BITMAP_IMG));
	memset(textmap, 0, sizeof(BITMAP_IMG));
	
	Load_Bitmap_File(heightmap, heightpath);
	Load_Bitmap_File(textmap, textpath);
		
	terrain->gridNormal = (VECTOR4D_PTR)malloc((heightmap->width - 1)*(heightmap->height - 1) * 2 * sizeof(POLY4DV2));
	memset(terrain->gridNormal, 0, (heightmap->width - 1)*(heightmap->height - 1) * 2 * sizeof(POLY4DV2));

	mesh->texture = textmap;
	mesh->num_vertices = heightmap->width * heightmap->height;
	mesh->total_vertices = mesh->num_vertices;
	mesh->num_polys = (heightmap->width-1)*(heightmap->height-1)*2;
	float gridwidth = width / (float)(heightmap->width - 1);
	float gridlength = length / (float)(heightmap->height - 1);

	mesh->vlist_local = (VERTEX4DTV1_PTR)malloc(mesh->num_vertices * sizeof(VERTEX4DTV1));
	mesh->vlist_trans = (VERTEX4DTV1_PTR)malloc(mesh->num_vertices * sizeof(VERTEX4DTV1));
	mesh->tlist = (POINT2D_PTR)malloc(4 * sizeof(POINT2D)); //每个grid用一个纹理，4个角共4个纹理坐标
	mesh->plist = (POLY4DV2_PTR)malloc(mesh->num_polys * sizeof(POLY4DV2));
	memset(mesh->vlist_local, 0, mesh->num_vertices * sizeof(VERTEX4DTV1));
	memset(mesh->vlist_trans, 0, mesh->num_vertices * sizeof(VERTEX4DTV1));
	memset(mesh->tlist, 0, 4 * sizeof(POINT2D));
	memset(mesh->plist, 0, mesh->num_polys * sizeof(POLY4DV2));
	//纹理坐标列表
	mesh->tlist[0] = { 0, 0 };
	mesh->tlist[1] = { (float)textmap->width - 10, 0 };
	mesh->tlist[2] = { 0, (float)textmap->height - 10 };
	mesh->tlist[3] = { (float)textmap->width - 10, (float)textmap->height - 10 };
	//顶点列表
	for (int row = 0; row < heightmap->height; row++)
	{
		for (int col = 0; col < heightmap->width; col++)
		{
			int vertex = (row * heightmap->width) + col;

			VERTEX4DTV1_PTR ver = &mesh->vlist_local[vertex];
			ver->x = col * gridwidth - width / 2;
			ver->z = row * gridlength - length / 2;
			ver->y = maxheight * ((float)(heightmap->buffer[vertex*4 + 1])) / 255;
			ver->w = 1;
		}
	}
	//多边形列表
	for (int row = 0; row < heightmap->height-1; row++)
	{
		for (int col = 0; col < heightmap->width-1; col++)
		{
			int index = (row * (heightmap->width - 1)) + col;
			//平面1
			POLY4DV2_PTR poly = &mesh->plist[2*index];
			poly->attr = 0;
			poly->state = 0;
			poly->color = _RGBTOINT(255, 255, 255);
			poly->texture = mesh->texture;
			poly->vlist = mesh->vlist_local;
			poly->tlist = mesh->tlist;
			SET_BIT(poly->state, POLY4DV2_STATE_ACTIVE);
			SET_BIT(poly->attr, POLY4DV2_ATTR_SHAD_MODE_TEXTURE);
			poly->vert[0] = col + row * heightmap->width;
			poly->vert[1] = col + (row + 1)*heightmap->width;
			poly->vert[2] = col + row * heightmap->width + 1;
			poly->text[0] = 0;
			poly->text[1] = 2;
			poly->text[2] = 1;
			//平面2
			poly = &mesh->plist[2 * index + 1];
			poly->attr = 0;
			poly->state = 0;
			poly->color = _RGBTOINT(255, 255, 255);
			poly->texture = mesh->texture;
			poly->vlist = mesh->vlist_local;
			poly->tlist = mesh->tlist;
			SET_BIT(poly->state, POLY4DV2_STATE_ACTIVE);
			SET_BIT(poly->attr, POLY4DV2_ATTR_SHAD_MODE_TEXTURE);
			poly->vert[0] = row * heightmap->width + col + 1;
			poly->vert[1] = (row + 1)*heightmap->width + col;
			poly->vert[2] = (row + 1)*heightmap->width + col + 1;
			poly->text[0] = 1;
			poly->text[1] = 2;
			poly->text[2] = 3;
		}
	}
	//顶点法线
	int* polys_touch_vertex = (int*)malloc(mesh->num_vertices * sizeof(int));
	memset(polys_touch_vertex, 0, mesh->num_vertices * sizeof(int));
	VECTOR4D u, v, n;
	int ver0, ver1, ver2;
	for (int pindex = 0; pindex < mesh->num_polys; pindex++)
	{
		POLY4DV2_PTR poly = &mesh->plist[pindex];
		ver0 = poly->vert[0];
		ver1 = poly->vert[1];
		ver2 = poly->vert[2];


		VECTOR4D_SUB(&mesh->vlist_local[ver1].v, &mesh->vlist_local[ver0].v, &u);
		VECTOR4D_SUB(&mesh->vlist_local[ver2].v, &mesh->vlist_local[ver0].v, &v);
		VECTOR4D_CROSS(&u, &v, &n);

		VECTOR4D_ADD(&mesh->vlist_local[ver0].n, &n, &mesh->vlist_local[ver0].n);
		VECTOR4D_ADD(&mesh->vlist_local[ver1].n, &n, &mesh->vlist_local[ver1].n);
		VECTOR4D_ADD(&mesh->vlist_local[ver2].n, &n, &mesh->vlist_local[ver2].n);

		polys_touch_vertex[ver0]++;
		polys_touch_vertex[ver1]++;
		polys_touch_vertex[ver2]++;
		//将面法线存储到地形数据中用于地形跟踪
		VECTOR4D_COPY(&terrain->gridNormal[pindex], &n);
	}

	for (int vindex = 0; vindex < mesh->num_vertices; vindex++)
	{
		mesh->vlist_local[vindex].nx /= polys_touch_vertex[vindex];
		mesh->vlist_local[vindex].ny /= polys_touch_vertex[vindex];
		mesh->vlist_local[vindex].nz /= polys_touch_vertex[vindex];

		VECTOR4D_Normalize(&mesh->vlist_local[vindex].n);
	}

	DestroyBitmap(heightmap);
	free(heightmap);
	free(polys_touch_vertex);
}