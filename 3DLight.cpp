#include "3DLight.h"

void ResetLights(LIGHTV1_PTR lights)
{
	memset(lights, 0, MAX_LIGHTS * sizeof(LIGHTV1));
}

//��ʼ����Դ
int InitLight(LIGHTV1_PTR lights, //��Դ�б�
	int index,	//��Դ����
	int state,	//״̬
	int attr,
	RGBAV1 c_ambient,//������ǿ��
	RGBAV1 c_diffuse,//ɢɫ��ǿ��
	RGBAV1 c_specular,//���淴���ǿ��
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
