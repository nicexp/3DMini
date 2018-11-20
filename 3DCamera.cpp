#include "3DCamera.h"

//��ʼ���������
void InitCamera(CAM4DV1_PTR cam,
	int cam_attr,//�������
	POINT4D_PTR cam_pos,//���λ��
	VECTOR4D_PTR cam_dir,//�������
	POINT4D_PTR cam_target, //uvn�����ʼĿ��λ��
	float near_clip_z,//���ü���
	float far_clip_z,//Զ�ü���
	float fov,//��Ұ
	float viewport_width,//�ӿڿ��
	float viewport_height)//�ӿڸ߶�
{
	cam->attr = cam_attr;
	VECTOR4D_COPY(&cam->pos, cam_pos);
	//���ŷ����ת��Ϊ����
	VECTOR4D_COPY(&cam->dir, cam_dir);
	//����UVN���
	VECTOR4D_INITXYZ(&cam->u, 1, 0, 0); //����Ϊx�᷽��
	VECTOR4D_INITXYZ(&cam->v, 0, 1, 0); //����Ϊy�᷽��
	VECTOR4D_INITXYZ(&cam->n, 0, 0, 1); //����Ϊz�᷽��

	if (cam_target != NULL)
		VECTOR4D_COPY(&cam->target, cam_target);
	else
		VECTOR4D_INITXYZ(&cam->target, 0, 0, 0);

	cam->near_clip_z = near_clip_z;
	cam->far_clip_z = far_clip_z;
	cam->viewport_width = viewport_width;
	cam->viewport_heght = viewport_height;
	cam->viewport_center_x = (viewport_width - 1) / 2;
	cam->viewport_center_y = (viewport_height - 1) / 2;

	cam->aspect_radio = viewport_width / viewport_height;
	//�����б任��������Ϊ��λ����
	Mat_IDENTITY_4X4(&cam->mcam);
	Mat_IDENTITY_4X4(&cam->mper);
	Mat_IDENTITY_4X4(&cam->mscr);

	cam->fov = fov / 180 * PI;
	//������ƽ���С
	cam->viewplane_width = 2.0;
	cam->viewplane_height = 2.0 / cam->aspect_radio;
	//�Ӿ�
	cam->view_dist = (cam->viewplane_width / 2) / tan(cam->fov / 2);
}

//Ͷ��ŷ���������
void BuildMatrixCamEuler(CAM4DV1_PTR cam)
{
	MATRIX4X4 mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
	//���ƽ�ƾ���
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);
	//��ת����
	double theta_x = cam->dir.x;
	double theta_y = cam->dir.y;
	double theta_z = cam->dir.z;
	//x�����Һ�����
	double cos_theta = cos(theta_x);
	double sin_theta = -sin(theta_x);
	Mat_Init_4X4(&mx_inv, 1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);
	//y�����Һ�����
	cos_theta = cos(theta_y);
	sin_theta = -sin(theta_y);
	Mat_Init_4X4(&my_inv, cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);
	//z�����Һ�����
	cos_theta = cos(theta_z);
	sin_theta = -sin(theta_z);
	Mat_Init_4X4(&mz_inv, cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	//������yxz˳��
	Mat_Mul_4X4(&my_inv, &mx_inv, &mtmp);
	Mat_Mul_4X4(&mtmp, &mz_inv, &mrot);
	//����任����
	Mat_Mul_4X4(&mt_inv, &mrot, &cam->mcam);
}

//����UVN�������(��/����)
void BuildMatrixCamUVN(CAM4DV1_PTR cam, int mode)
{
	MATRIX4X4 mt_inv, mt_uvn, mt_tmp;
	//ƽ�ƾ���
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-cam->pos.x, -cam->pos.y, -cam->pos.z, 1);
	if (mode == UVN_SPHERICAL)
	{
		//Ŀ���
		float phi = cam->dir.x; //����(��x����ת�ĽǶ�,����y���غ�ʱΪ0,��Χ��0-180)
		float theta = cam->dir.y; //��λ��(��y����ת,����x���غ�ʱΪ0)

		cam->target.x = 1.0 * sin(phi) * cos(theta);
		cam->target.y = 1.0 * cos(phi);
		cam->target.z = 1.0 * sin(phi) * sin(theta);
	}

	if (mode == UVN_SPHERICAL)
	{
		//n����
		VECTOR4D_COPY(&cam->n, &cam->target);
	}
	else
	{
		VECTOR4D_SUB(&cam->target, &cam->pos, &cam->n);
	}
	//��ʼ��v����
	VECTOR4D_INITXYZ(&cam->v, 0, 1, 0);
	//u����
	VECTOR4D_CROSS(&cam->v, &cam->n, &cam->u);
	//����v
	VECTOR4D_CROSS(&cam->n, &cam->u, &cam->v);
	//��һ��
	VECTOR4D_Normalize(&cam->u);
	VECTOR4D_Normalize(&cam->v);
	VECTOR4D_Normalize(&cam->n);
	//uvn��ת����
	Mat_Init_4X4(&mt_uvn, cam->u.x, cam->v.x, cam->n.x, 0,
		cam->u.y, cam->v.y, cam->n.y, 0,
		cam->u.z, cam->v.z, cam->n.z, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&mt_inv, &mt_uvn, &cam->mcam);
}

//����͸�ӱ任����
void BuildCameraToPerspectMatrix(CAM4DV1_PTR cam)
{
	Mat_Init_4X4(&cam->mper, cam->view_dist, 0, 0, 0,
		0, cam->view_dist*cam->aspect_radio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}

//������Ļ�任����
void BuildPerspectToScreenMatrix(CAM4DV1_PTR cam)
{
	float alpha = 0.5 * cam->viewport_width - 0.5;
	float beta = 0.5 * cam->viewport_heght - 0.5;
	Mat_Init_4X4(&cam->mscr, alpha, 0, 0, 0,
		0, -beta, 0, 0,
		0, 0, 1, 0,
		alpha, beta, 0, 1);
}

//��ʼ����ؾ���
void InitTransMatrix(CAM4DV1_PTR cam)
{
	if (cam->attr == CAMERA_EULER)
		BuildMatrixCamEuler(cam);
	else
		BuildMatrixCamUVN(cam,UVN_SPHERICAL);

	BuildCameraToPerspectMatrix(cam);
	BuildPerspectToScreenMatrix(cam);
}

//����켣1
void BuildCameraPosAndDir(CAM4DV1_PTR cam,float distance, float view_angle)
{
	cam->pos.x = distance * cos(view_angle / 180 * PI);
	cam->pos.y = distance * sin(view_angle / 180 * PI);
	cam->pos.z = 2 * cam->pos.y;
}