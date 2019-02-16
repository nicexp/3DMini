#ifndef _3DCAMERA_H
#define _3DCAMERA_H

#include "3DLib1.h"

typedef struct CAM4DV1_TYP
{
	int state;
	int attr;

	POINT4D pos; //相机在世界坐标中的位置
	VECTOR4D dir; //欧拉角度或者UVN相机模型的注视方向

	VECTOR4D u;
	VECTOR4D v;
	VECTOR4D n;
	POINT4D target;

	float view_dist;//视距
	float fov; //水平方向和垂直方向视野
	float near_clip_z;//近裁剪面
	float far_clip_z;//远裁剪面

	//上下左右裁剪面 略

	float viewplane_width;//视平面宽度
	float viewplane_height;//视平面高度

	float viewport_width;//视口宽度
	float viewport_heght;//视口高度
	float viewport_center_x;//视口中心x
	float viewport_center_y;//视口中心y

	float aspect_radio; //宽高比

	MATRIX4X4 mcam; //相机变换矩阵
	MATRIX4X4 mper; //透视变换矩阵
	MATRIX4X4 mscr; //屏幕变换矩阵
}CAM4DV1, *CAM4DV1_PTR;

void InitCamera(CAM4DV1_PTR cam,
	int cam_attr,//相机属性
	POINT4D_PTR cam_pos,//相机位置
	VECTOR4D_PTR cam_dir,//相机朝向
	POINT4D_PTR cam_target, //uvn相机初始目标位置
	float near_clip_z,//近裁剪面
	float far_clip_z,//远裁剪面
	float fov,//视野
	float viewport_width,//视口宽度
	float viewport_height);
//构建欧拉相机矩阵
void BuildMatrixCamEuler(CAM4DV1_PTR cam);
//构建UVN相机矩阵(简单/球形)
void BuildMatrixCamUVN(CAM4DV1_PTR cam, int mode);
//构建透视变换矩阵
void BuildCameraToPerspectMatrix(CAM4DV1_PTR cam);
//构建屏幕变换矩阵
void BuildPerspectToScreenMatrix(CAM4DV1_PTR cam);
//初始化相关矩阵
void InitTransMatrix(CAM4DV1_PTR cam);
//相机轨迹1
void BuildCameraPosAndDir(CAM4DV1_PTR cam, float distance);
//相机轨迹与朝向
void UpdateCameraPosAndDir(CAM4DV1_PTR cam);
void UpdateCameraPosAndDirSimple(CAM4DV1_PTR cam);
void SetCameraTargetPos(CAM4DV1_PTR cam, VECTOR4D_PTR pos);
#endif