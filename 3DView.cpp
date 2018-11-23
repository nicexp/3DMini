#include "3DView.h"

#define NUM_OBJECTS 2
#define OBJECT_SPACING  250

//渲染列表绘制
void RenderlistView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, RENDERLIST4DV1_PTR renderlist, HDC* hdc)
{
	//初始化相机
	InitCamera(cam,
		CAMERA_UVN,
		cam_pos,
		cam_dir,
		NULL,
		20, 8000, 120, WINDOW_WIDTH, WINDOW_HEIGHT);
	//初始化变换矩阵
	InitTransMatrix(cam);
	//重置渲染列表
	ResetRenderlist(renderlist);
	//初始化物体
	InitObject(obj);
	//加入渲染列表
	for (int x = -NUM_OBJECTS / 2; x < 0 / 2; x++)
	{
		for (int z = -NUM_OBJECTS / 2; z < 0 / 2; z++)
		{
			ResetObjState(obj);

			obj->world_pos.x = x * OBJECT_SPACING + OBJECT_SPACING / 2;
			obj->world_pos.y = 0;
			obj->world_pos.z = z * OBJECT_SPACING + OBJECT_SPACING / 2;
			DEBUG_LOG("obj.pos(%f, %f, %f)", obj->world_pos.x, obj->world_pos.y, obj->world_pos.z);

			if (!(CullObj(obj, cam, CULL_OBJECT_XYZ_PLANES)))
			{
				ModelToWorldObj(obj);
				InsertObjToRenderlist(renderlist, obj, 0);
			}
		}
	}
	//构建3d流水线
	Renderlist3DLine(renderlist, cam);
	//描绘
	for (int poly = 0; poly < renderlist->num_polys; poly++)
	{
		POLYF4DV1_PTR cur_poly = renderlist->poly_ptrs[poly];

		if ((cur_poly->state & POLY4DV1_STATE_BACKFACE)
			|| (cur_poly->state & POLY4DV1_STATE_CLIPPED))
			continue;

		MoveToEx(*hdc, cur_poly->tvlist[0].x, cur_poly->tvlist[0].y, NULL);
		LineTo(*hdc, cur_poly->tvlist[1].x, cur_poly->tvlist[1].y);
		LineTo(*hdc, cur_poly->tvlist[2].x, cur_poly->tvlist[2].y);
		LineTo(*hdc, cur_poly->tvlist[0].x, cur_poly->tvlist[0].y);
	}
}

//物体绘制
void ObjectView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, HDC* hdc)
{
	//初始化相机
	InitCamera(cam,
		CAMERA_UVN,
		cam_pos,
		cam_dir,
		NULL,
		20, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//初始化变换矩阵
	InitTransMatrix(cam);
	//初始化物体
	InitObject(obj);

	//剔除物体
	if (CullObj(obj, cam, CULL_OBJECT_XYZ_PLANES))
		return;
	//设置物体位置
	//VECTOR4D_COPY(&obj->world_pos, &world_pos);
	//重置物体状态
	ResetObjState(obj);
	//构建3D流水线
	Object3DLine(obj, cam);

	//描绘
	int v0, v1, v2 = 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV1_PTR cur_poly = &obj->plist[poly];

		if (obj->plist[poly].state & POLY4DV1_STATE_BACKFACE)
			continue;

		MoveToEx(*hdc, cur_poly->vlist[cur_poly->vert[0]].x, cur_poly->vlist[cur_poly->vert[0]].y, NULL);
		LineTo(*hdc, cur_poly->vlist[cur_poly->vert[1]].x, cur_poly->vlist[cur_poly->vert[1]].y);
		LineTo(*hdc, cur_poly->vlist[cur_poly->vert[2]].x, cur_poly->vlist[cur_poly->vert[2]].y);
		LineTo(*hdc, cur_poly->vlist[cur_poly->vert[0]].x, cur_poly->vlist[cur_poly->vert[0]].y);
	}
}
//设置相机位置
void SetCameraPos(POINT4D_PTR cam_pos, CAM4DV1_PTR cam, int direct, int speed)
{
	if (direct == DIRECT_FRONT)
	{
		cam_pos->x += cam->n.x * speed;
		cam_pos->z += cam->n.z * speed;
	}
	else if (direct == DIRECT_AFTER)
	{
		cam_pos->x += cam->n.x * -speed;
		cam_pos->z += cam->n.z * -speed;
	}
	else if (direct == DIRECT_LEFT)
	{
		cam_pos->x += cam->u.x * -speed;
		cam_pos->z += cam->u.z * -speed;
	}
	else if (direct == DIRECT_RIGHT)
	{
		cam_pos->x += cam->u.x * speed;
		cam_pos->z += cam->u.z * speed;
	}
}
//物体绘制2
void ObjectView2(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV2_PTR obj,LIGHTV1_PTR lights, HDC* hdc)
{
	//初始化相机
	InitCamera(cam,
		CAMERA_UVN,
		cam_pos,
		cam_dir,
		NULL,
		20, 8000, 90, WINDOW_WIDTH, WINDOW_HEIGHT);
	//初始化变换矩阵
	InitTransMatrix(cam);
	//初始化物体
	InitObject(obj);
	//初始化光源
	InitAllLight(lights);
	//设置物体位置
	//VECTOR4D_COPY(&obj->world_pos, &world_pos);
	//剔除物体
	if (CullObj(obj, cam, CULL_OBJECT_XYZ_PLANES))
		return;
	//重置物体状态
	ResetObjState(obj);
	//构建3D流水线
	Object3DLine(obj, cam, lights);

	//描绘
	int v0, v1, v2 = 0;
	for (int poly = 0; poly < obj->num_polys; poly++)
	{
		POLY4DV2_PTR cur_poly = &obj->plist[poly];

		if (cur_poly->state & POLY4DV1_STATE_BACKFACE)
			continue;

		//恒定着色
		//ShaderFlat(cur_poly,1, *hdc);
		//高洛德着色
		ShaderGouraud(cur_poly, *hdc);
	}
}