#ifndef _3DLIGHT_H
#define _3DLIGHT_H

#include "3DLib1.h"

#define LIGHTV1_ATTR_AMBIENT 0x0001	//������Դ
#define LIGHTV1_ATTR_INFINITE 0x0002	//����Զ��Դ
#define LIGHTV1_ATTR_POINT	0x0004	//���Դ
#define LIGHTV1_ATTR_SPOTLIGHT1 0x0008	//�۹��(��)
#define LIGHTV1_ATTR_SPOTLIGHT2 0x0010	//�۹��(����)

#define LIGHT_TRANSFORM_COPY_ONLY 1
#define LIGHT_TRANSFORM_LOCAL_ONLY 2
#define LIGHT_TRANSFORM_TRANS_ONLY 3
#define LIGHT_TRANSFORM_LOCAL_TO_TRANS 4

#define LIGHTV1_STATE_ON 1	//��Դ����
#define LIGHTV1_STATE_OFF 0 //��Դ�ر�

#define LIGHT_COUNT 4

#define MAX_LIGHTS 8

typedef struct LIGHTV1_TYP
{
	int state;
	int id; //��ԴID
	int attr;

	RGBAV1 c_ambient;//������ǿ��
	RGBAV1 c_diffuse;//ɢɫ��ǿ��
	RGBAV1 c_specular;//���淴ɫ��ǿ��
	POINT4D pos;//��Դλ��
	POINT4D tpos;//�任��Ĺ�Դλ��
	VECTOR4D dir;//��Դ����
	VECTOR4D tdir;//�任��Ĺ�Դ����

	float kc, kl, kq;//˥������
	float spot_inner;//�۹����׶��
	float spot_outer;//�۹����׶��
	float pf;//�۹��ָ������
}LIGHTV1, *LIGHTV1_PTR;

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
	float pf);

//׷�ӻ�����
void InitAllLight(LIGHTV1_PTR lights);
void TransformLights(LIGHTV1_PTR lights, int max_lights, MATRIX4X4_PTR mt, int oper_type);
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
	float pf);
#endif