#include "3DLight.h"

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
		VECTOR4D_COPY(&lights[index].pos, pos);

	if (dir)
	{
		VECTOR4D_COPY(&lights[index].dir, dir);
		VECTOR4D_Normalize(&lights[index].dir);
	}

	lights[index].spot_inner = spot_inner;
	lights[index].spot_outer = spot_outer;
	lights[index].pf = pf;

	return index;
}
//追加环境光
void InitAllLight(LIGHTV1_PTR lights)
{
	//初始化环境光
	InitLight(lights, 0, LIGHTV1_STATE_ON, LIGHTV1_ATTR_AMBIENT, ambient, invalid_light, invalid_light, NULL, NULL, 0, 0, 0, 0, 0, 0);
	//初始化直射光
	InitLight(lights, 1, LIGHTV1_STATE_ON, LIGHTV1_ATTR_INFINITE, invalid_light, infinite_diffuse, invalid_light, NULL, &infinite_dir, 0, 0, 0, 0, 0, 0);
	//初始化点光源
	InitLight(lights, 2, LIGHTV1_STATE_ON, LIGHTV1_ATTR_POINT, invalid_light, point_diffuse, invalid_light, &point_pos, NULL, 0, 0.001, 0, 0, 0, 0);
	//初始化聚光灯
	InitLight(lights, 3, LIGHTV1_STATE_ON, LIGHTV1_ATTR_SPOTLIGHT2, invalid_light, spot_diffuse, invalid_light,&spot_pos,&spot_dir, 0, 0.001, 0, 0, 0, 1);
}
