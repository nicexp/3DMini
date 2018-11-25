#ifndef _3DVIEW_H
#define _3DVIEW_H

#include <windows.h>
#include "3DLib1.h"
#include "3DLib2.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject.h"
#include "3DObject2.h"
#include "3DRenderlist.h"
#include "3DShader.h"
#include "3DLight.h"
#include "3DLog.h"

#define CAMERA_DISTANCE 500
#define CAMERA_SPEED 20

//所有光源与属性
static LIGHTV1 lights[MAX_LIGHTS];
//物体
static OBJECT4DV1 obj;
static OBJECT4DV2 obj2;
//渲染列表
static RENDERLIST4DV1 renderlist;
//相机及属性
static CAM4DV1 cam;
static POINT4D cam_pos = { -600, 0, 0, 1 };
static VECTOR4D cam_dir = { PI / 2, 0, 0, 1 };

static VECTOR4D world_pos = { -125, 0, -125, 1 };
static float view_angle_x = 90;
static float view_angle_y = 0;

void RenderlistView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, RENDERLIST4DV1_PTR renderlist);
void ObjectView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj);
void SetCameraPos(POINT4D_PTR cam_pos, CAM4DV1_PTR cam, int direct, int speed);
//物体绘制2
void ObjectView2(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV2_PTR obj, LIGHTV1_PTR lights);

#endif
