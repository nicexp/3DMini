#ifndef _3DCAMERA_H
#define _3DCAMERA_H

#include "3DLib1.h"

class Camera
{
public:
	Camera();
	~Camera();
//field
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

//method:
	//��ʼ�����
	void InitCamera(int cam_attr,//�������
		POINT4D_PTR cam_pos,//���λ��
		VECTOR4D_PTR cam_dir,//�������
		POINT4D_PTR cam_target, //uvn�����ʼĿ��λ��
		float near_clip_z,//���ü���
		float far_clip_z,//Զ�ü���
		float fov,//��Ұ
		float viewport_width,//�ӿڿ��
		float viewport_height);
	//����ŷ���������
	void BuildMatrixCamEuler();
	//����UVN�������(��/����)
	void BuildMatrixCamUVN(int mode);
	//����͸�ӱ任����
	void BuildCameraToPerspectMatrix();
	//������Ļ�任����
	void BuildPerspectToScreenMatrix();
	//��ʼ����ؾ���
	void InitTransMatrix();
	//����켣1
	void BuildCameraPosAndDir(float distance, float view_angle);
};

#endif