#ifndef _3DVIEW_H
#define _3DVIEW_H

#include <windows.h>
#include "3DLib1.h"
#include "3DTool.h"
#include "3DCamera.h"
#include "3DObject.h"
#include "3DObject2.h"
#include "3DRenderlist.h"
#include "3DShader.h"
#include "3DLight.h"
#include "3DLog.h"

void RenderlistView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, RENDERLIST4DV1_PTR renderlist, HDC* hdc);
void ObjectView(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV1_PTR obj, HDC* hdc);
void SetCameraPos(POINT4D_PTR cam_pos, CAM4DV1_PTR cam, int direct, int speed);
//ŒÔÃÂªÊ÷∆2
void ObjectView2(POINT4D_PTR cam_pos, POINT4D_PTR cam_dir, CAM4DV1_PTR cam, OBJECT4DV2_PTR obj, LIGHTV1_PTR lights, HDC* hdc);

#endif
