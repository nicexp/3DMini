#include "3DLight.h"

static RGBAV1 invalid_light = { 0, 0, 0, 0 }; //��Ч��
static RGBAV1 ambient = { 80, 80, 80, 0 };	//������ǿ��
static RGBAV1 infinite_diffuse = { 0, 0, 255, 0 }; //ֱ���ɢ��ǿ��
static RGBAV1 point_diffuse = { 255, 0, 0, 0 }; //���Դɢ��ǿ��
static RGBAV1 spot_diffuse = { 0, 255, 0, 0 }; //�۹��ɢ��ǿ��
static VECTOR4D infinite_dir = { -1, 1, -1, 1 }; //ֱ��ⷽ��
static VECTOR4D point_pos = { -500, 500, 500, 1 }; //���Դλ��
static VECTOR4D spot_pos = { 500, 500, 500, 1 }; //�۹��λ��
static VECTOR4D spot_dir = { 1, 1, 1, 1 }; //�۹�Ʒ���

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

//�任��Դλ��(ּ���������ռ���ִ�й��ռ���,��3D�ü������)
void TransformLights(LIGHTV1_PTR lights, int max_lights, MATRIX4X4_PTR mt,int oper_type)
{
	MATRIX4X4 mr;//ȥ��ƽ�ƺ�ľ���
	memcpy((void*)&mr, (void*)mt, sizeof(MATRIX4X4));
	mr.M30 = mr.M31 = mr.M32 = 0;//ȥ��ƽ��

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

//׷�ӹ�Դ
void InitAllLight(LIGHTV1_PTR lights)
{
	//�������й�Դ
	ResetLights(lights);
	//��ʼ��������
	InitLight(lights, 0, LIGHTV1_STATE_ON, LIGHTV1_ATTR_AMBIENT, ambient, invalid_light, invalid_light, NULL, NULL, 0, 0, 0, 0, 0, 0);
	//��ʼ��ֱ���
	InitLight(lights, 1, LIGHTV1_STATE_OFF, LIGHTV1_ATTR_INFINITE, invalid_light, infinite_diffuse, invalid_light, NULL, &infinite_dir, 0, 0, 0, 0, 0, 0);
	//��ʼ�����Դ
	InitLight(lights, 2, LIGHTV1_STATE_ON, LIGHTV1_ATTR_POINT, invalid_light, point_diffuse, invalid_light, &point_pos, NULL, 0, 0.001, 0, 0, 0, 0);
	//��ʼ���۹��
	InitLight(lights, 3, LIGHTV1_STATE_OFF, LIGHTV1_ATTR_SPOTLIGHT2, invalid_light, spot_diffuse, invalid_light, &spot_pos, &spot_dir, 0, 0.001, 0, 0, 0, 1);
}
