#ifndef _3DLIGHT_H
#define _3DLIGHT_H

#include "3DLib1.h"

#define LIGHTV1_ATTR_AMBIENT 0x0001	//环境光源
#define LIGHTV1_ATTR_INFINITE 0x0002	//无穷远光源
#define LIGHTV1_ATTR_POINT	0x0004	//点光源
#define LIGHTV1_ATTR_SPOTLIGHT1 0x0008	//聚光灯(简单)
#define LIGHTV1_ATTR_SPOTLIGHT2 0x0010	//聚光灯(复杂)

#define LIGHTV1_STATE_ON 1	//光源开启
#define LIGHTV1_STATE_OFF 0 //光源关闭

#define MAX_LIGHTS 8

typedef struct LIGHTV1_TYP
{
	int state;
	int id; //光源ID
	int attr;

	RGBAV1 c_ambient;//环境光强度
	RGBAV1 c_diffuse;//散色光强度
	RGBAV1 c_specular;//镜面反色光强度
	POINT4D pos;//光源位置
	VECTOR4D dir;//光源方向

	float kc, kl, kq;//衰减因子
	float spot_inner;//聚光灯内锥角
	float spot_outer;//聚光灯外锥角
	float pf;//聚光灯指数因子
}LIGHTV1, *LIGHTV1_PTR;

int InitLight(LIGHTV1_PTR lights, //光源列表
	int index,	//光源索引
	int state,	//状态
	int attr,
	RGBAV1 c_ambient,//环境光强度
	RGBAV1 c_diffuse,//散色光强度
	RGBAV1 c_specular,//镜面反射光强度
	POINT4D_PTR pos,
	VECTOR4D_PTR dir,
	float kc, float kl, float kq,
	float spot_inner, float spot_outer,
	float pf);

static RGBAV1 invalid_light = { 0, 0, 0, 0 }; //无效光
static RGBAV1 ambient = { 80, 80, 80, 0 };	//环境光强度
static RGBAV1 infinite_diffuse = { 0, 0, 255, 0 }; //直射光散射强度
static RGBAV1 point_diffuse = { 255, 0, 0, 0 }; //点光源散射强度
static RGBAV1 spot_diffuse = { 0, 255, 0, 0 }; //聚光灯散射强度
static VECTOR4D infinite_dir = { -1, 1, -1, 1 }; //直射光方向
static VECTOR4D point_pos = { -500, 500, 500, 1 }; //点光源位置
static VECTOR4D spot_pos = { 500, 500, 500, 1 }; //聚光灯位置
static VECTOR4D spot_dir = { 1, 1, 1, 1 }; //聚光灯方向

//追加环境光
void InitAllLight(LIGHTV1_PTR lights);
#endif