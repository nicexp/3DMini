#ifndef _3DCAMERA_H
#define _3DCAMERA_H

#include "3DLib1.h"

typedef struct CAM4DV1_TYP
{
	int state;
	int attr;

	POINT4D pos; //��������������е�λ��
	VECTOR4D dir; //ŷ���ǶȻ���UVN���ģ�͵�ע�ӷ���

	VECTOR4D u;
	VECTOR4D v;
	VECTOR4D n;
	POINT4D target;

	float view_dist;//�Ӿ�
	float fov; //ˮƽ����ʹ�ֱ������Ұ
	float near_clip_z;//���ü���
	float far_clip_z;//Զ�ü���

	//�������Ҳü��� ��

	float viewplane_width;//��ƽ����
	float viewplane_height;//��ƽ��߶�

	float viewport_width;//�ӿڿ��
	float viewport_heght;//�ӿڸ߶�
	float viewport_center_x;//�ӿ�����x
	float viewport_center_y;//�ӿ�����y

	float aspect_radio; //��߱�

	MATRIX4X4 mcam; //����任����
	MATRIX4X4 mper; //͸�ӱ任����
	MATRIX4X4 mscr; //��Ļ�任����
}CAM4DV1, *CAM4DV1_PTR;

void InitCamera(CAM4DV1_PTR cam,
	int cam_attr,//�������
	POINT4D_PTR cam_pos,//���λ��
	VECTOR4D_PTR cam_dir,//�������
	POINT4D_PTR cam_target, //uvn�����ʼĿ��λ��
	float near_clip_z,//���ü���
	float far_clip_z,//Զ�ü���
	float fov,//��Ұ
	float viewport_width,//�ӿڿ��
	float viewport_height);
//����ŷ���������
void BuildMatrixCamEuler(CAM4DV1_PTR cam);
//����UVN�������(��/����)
void BuildMatrixCamUVN(CAM4DV1_PTR cam, int mode);
//����͸�ӱ任����
void BuildCameraToPerspectMatrix(CAM4DV1_PTR cam);
//������Ļ�任����
void BuildPerspectToScreenMatrix(CAM4DV1_PTR cam);
//��ʼ����ؾ���
void InitTransMatrix(CAM4DV1_PTR cam);
//����켣1
void BuildCameraPosAndDir(CAM4DV1_PTR cam, float distance);
//����켣�볯��
void UpdateCameraPosAndDir(CAM4DV1_PTR cam);
void UpdateCameraPosAndDirSimple(CAM4DV1_PTR cam);
void SetCameraTargetPos(CAM4DV1_PTR cam, VECTOR4D_PTR pos);
#endif