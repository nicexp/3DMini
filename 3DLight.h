#ifndef _3DLIGHT_H
#define _3DLIGHT_H

#include "3DLib1.h"

#define LIGHTV1_ATTR_AMBIENT 0x0001	//环境光源
#define LIGHTV1_ATTR_INFINITE 0x0002	//无穷远光源
#define LIGHTV1_ATTR_POINT	0x0004	//点光源
#define LIGHTV1_ATTR_SPOTLIGHT1 0x0008	//聚光灯(简单)
#define LIGHTV1_ATTR_SPOTLIGHT2 0x0010	//聚光灯(复杂)

#define LIGHT_TRANSFORM_COPY_ONLY 1
#define LIGHT_TRANSFORM_LOCAL_ONLY 2
#define LIGHT_TRANSFORM_TRANS_ONLY 3
#define LIGHT_TRANSFORM_LOCAL_TO_TRANS 4

#define LIGHTV1_STATE_ON 1	//光源开启
#define LIGHTV1_STATE_OFF 0 //光源关闭

#define LIGHT_COUNT 4

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
	POINT4D tpos;//变换后的光源位置
	VECTOR4D dir;//光源方向
	VECTOR4D tdir;//变换后的光源方向

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

//追加环境光
void InitAllLight(LIGHTV1_PTR lights);
void TransformLights(LIGHTV1_PTR lights, int max_lights, MATRIX4X4_PTR mt, int oper_type);
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
#endif