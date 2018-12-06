#include "3DLight.h"

static RGBAV1 invalid_light = { 0, 0, 0, 0 }; //无效光
static RGBAV1 ambient = { 80, 80, 80, 0 };	//环境光强度
static RGBAV1 infinite_diffuse = { 0, 0, 255, 0 }; //直射光散射强度
static RGBAV1 point_diffuse = { 255, 0, 0, 0 }; //点光源散射强度
static RGBAV1 spot_diffuse = { 0, 255, 0, 0 }; //聚光灯散射强度
static VECTOR4D infinite_dir = { -1, 1, -1, 1 }; //直射光方向
static VECTOR4D point_pos = { -500, 500, 500, 1 }; //点光源位置
static VECTOR4D spot_pos = { 500, 500, 500, 1 }; //聚光灯位置
static VECTOR4D spot_dir = { 1, 1, 1, 1 }; //聚光灯方向

void ResetLights(LIGHTV1_PTR lights)
{
	memset(lights, 0, MAX_LIGHTS * sizeof(LIGHTV1));
}

//初始化光源
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
	float pf)
{
	if (index < 0 || index >= MAX_LIGHTS)
		return 0;

	lights[index].state = state;
	lights[index].attr = attr;
	lights[index].c_ambient = c_ambient;
	lights[index].c_diffuse = c_diffuse;
	lights[index].c_specular = c_specular;
	lights[index].kc = kc;
	lights[index].kl = kl;
	lights[index].kq = kq;

	if (pos)
	{
		VECTOR4D_COPY(&lights[index].pos, pos);
		VECTOR4D_COPY(&lights[index].tpos, pos);
	}

	if (dir)
	{
		VECTOR4D_COPY(&lights[index].dir, dir);
		VECTOR4D_Normalize(&lights[index].dir);
		VECTOR4D_COPY(&lights[index].tdir, &lights[index].dir);
	}

	lights[index].spot_inner = spot_inner;
	lights[index].spot_outer = spot_outer;
	lights[index].pf = pf;

	return index;
}

//变换光源位置(旨在相机坐标空间中执行光照计算,在3D裁剪处理后)
void TransformLights(LIGHTV1_PTR lights, int max_lights, MATRIX4X4_PTR mt,int oper_type)
{
	MATRIX4X4 mr;//去除平移后的矩阵
	memcpy((void*)&mr, (void*)mt, sizeof(MATRIX4X4));
	mr.M30 = mr.M31 = mr.M32 = 0;//去掉平移

	for (int index = 0; index < max_lights; index++)
	{
		if (oper_type == LIGHT_TRANSFORM_COPY_ONLY)
		{
			VECTOR4D_COPY(&lights[index].tpos, &lights[index].pos);
			VECTOR4D_COPY(&lights[index].tdir, &lights[index].dir);
		}
		else if (oper_type == LIGHT_TRANSFORM_LOCAL_ONLY)
		{
			VECTOR4D tempRes;
			Mat_Mul_VECTOR4D_4X4(&lights[index].pos, mt, &tempRes);
			VECTOR4D_COPY(&lights[index].pos, &tempRes);
			Mat_Mul_VECTOR4D_4X4(&lights[index].dir, &mr, &tempRes);
			VECTOR4D_COPY(&lights[index].dir, &tempRes);
		}
		else if (oper_type == LIGHT_TRANSFORM_TRANS_ONLY)
		{
			VECTOR4D tempRes;
			Mat_Mul_VECTOR4D_4X4(&lights[index].tpos, mt, &tempRes);
			VECTOR4D_COPY(&lights[index].tpos, &tempRes);
			Mat_Mul_VECTOR4D_4X4(&lights[index].tdir, &mr, &tempRes);
			VECTOR4D_COPY(&lights[index].tdir, &tempRes);
		}
		else if (oper_type == LIGHT_TRANSFORM_LOCAL_TO_TRANS)
		{
			Mat_Mul_VECTOR4D_4X4(&lights[index].pos, mt, &lights[index].tpos);
			Mat_Mul_VECTOR4D_4X4(&lights[index].dir, &mr, &lights[index].tdir);
		}
	}
}

//追加光源
void InitAllLight(LIGHTV1_PTR lights)
{
	//重置所有光源
	ResetLights(lights);
	//初始化环境光
	InitLight(lights, 0, LIGHTV1_STATE_ON, LIGHTV1_ATTR_AMBIENT, ambient, invalid_light, invalid_light, NULL, NULL, 0, 0, 0, 0, 0, 0);
	//初始化直射光
	InitLight(lights, 1, LIGHTV1_STATE_OFF, LIGHTV1_ATTR_INFINITE, invalid_light, infinite_diffuse, invalid_light, NULL, &infinite_dir, 0, 0, 0, 0, 0, 0);
	//初始化点光源
	InitLight(lights, 2, LIGHTV1_STATE_ON, LIGHTV1_ATTR_POINT, invalid_light, point_diffuse, invalid_light, &point_pos, NULL, 0, 0.001, 0, 0, 0, 0);
	//初始化聚光灯
	InitLight(lights, 3, LIGHTV1_STATE_OFF, LIGHTV1_ATTR_SPOTLIGHT2, invalid_light, spot_diffuse, invalid_light, &spot_pos, &spot_dir, 0, 0.001, 0, 0, 0, 1);
}
