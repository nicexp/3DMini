#include "3DCamera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

//��ʼ���������
void Camera::InitCamera(int cam_attr,//�������
	POINT4D_PTR cam_pos,//���λ��
	VECTOR4D_PTR cam_dir,//�������
	POINT4D_PTR cam_target, //uvn�����ʼĿ��λ��
	float near_clip_z,//���ü���
	float far_clip_z,//Զ�ü���
	float fov,//��Ұ
	float viewport_width,//�ӿڿ��
	float viewport_height)//�ӿڸ߶�
{
	this->attr = cam_attr;
	VECTOR4D_COPY(&this->pos, cam_pos);
	//���ŷ����ת��Ϊ����
	VECTOR4D_COPY(&this->dir, cam_dir);
	//����UVN���
	VECTOR4D_INITXYZ(&this->u, 1, 0, 0); //����Ϊx�᷽��
	VECTOR4D_INITXYZ(&this->v, 0, 1, 0); //����Ϊy�᷽��
	VECTOR4D_INITXYZ(&this->n, 0, 0, 1); //����Ϊz�᷽��

	if (cam_target != NULL)
		VECTOR4D_COPY(&this->target, cam_target);
	else
		VECTOR4D_INITXYZ(&this->target, 0, 0, 0);

	this->near_clip_z = near_clip_z;
	this->far_clip_z = far_clip_z;
	this->viewport_width = viewport_width;
	this->viewport_heght = viewport_height;
	this->viewport_center_x = (viewport_width - 1) / 2;
	this->viewport_center_y = (viewport_height - 1) / 2;

	this->aspect_radio = viewport_width / viewport_height;
	//�����б任��������Ϊ��λ����
	Mat_IDENTITY_4X4(&this->mcam);
	Mat_IDENTITY_4X4(&this->mper);
	Mat_IDENTITY_4X4(&this->mscr);

	this->fov = fov / 180 * PI;
	//������ƽ���С
	this->viewplane_width = 2.0;
	this->viewplane_height = 2.0 / this->aspect_radio;
	//�Ӿ�
	this->view_dist = (this->viewplane_width / 2) / tan(this->fov / 2);
}

//Ͷ��ŷ���������
void Camera::BuildMatrixCamEuler()
{
	MATRIX4X4 mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
	//���ƽ�ƾ���
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-this->pos.x, -this->pos.y, -this->pos.z, 1);
	//��ת����
	double theta_x = this->dir.x;
	double theta_y = this->dir.y;
	double theta_z = this->dir.z;
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
	Mat_Mul_4X4(&mt_inv, &mrot, &this->mcam);
}

//����UVN�������(��/����)
void Camera::BuildMatrixCamUVN(int mode)
{
	MATRIX4X4 mt_inv, mt_uvn, mt_tmp;
	//ƽ�ƾ���
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-this->pos.x, -this->pos.y, -this->pos.z, 1);
	if (mode == UVN_SPHERICAL)
	{
		//Ŀ���
		float phi = this->dir.x; //����(��x����ת�ĽǶ�,����y���غ�ʱΪ0,��Χ��0-180)
		float theta = this->dir.y; //��λ��(��y����ת,����x���غ�ʱΪ0)

		this->target.x = 1.0 * sin(phi) * cos(theta);
		this->target.y = 1.0 * cos(phi);
		this->target.z = 1.0 * sin(phi) * sin(theta);
	}

	if (mode == UVN_SPHERICAL)
	{
		//n����
		VECTOR4D_COPY(&this->n, &this->target);
	}
	else
	{
		VECTOR4D_SUB(&this->target, &this->pos, &this->n);
	}
	//��ʼ��v����
	VECTOR4D_INITXYZ(&this->v, 0, 1, 0);
	//u����
	VECTOR4D_CROSS(&this->v, &this->n, &this->u);
	//����v
	VECTOR4D_CROSS(&this->n, &this->u, &this->v);
	//��һ��
	VECTOR4D_Normalize(&this->u);
	VECTOR4D_Normalize(&this->v);
	VECTOR4D_Normalize(&this->n);
	//uvn��ת����
	Mat_Init_4X4(&mt_uvn, this->u.x, this->v.x, this->n.x, 0,
		this->u.y, this->v.y, this->n.y, 0,
		this->u.z, this->v.z, this->n.z, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&mt_inv, &mt_uvn, &this->mcam);
}

//����͸�ӱ任����
void Camera::BuildCameraToPerspectMatrix()
{
	Mat_Init_4X4(&this->mper, this->view_dist, 0, 0, 0,
		0, this->view_dist*this->aspect_radio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}

//������Ļ�任����
void Camera::BuildPerspectToScreenMatrix()
{
	float alpha = 0.5 * this->viewport_width - 0.5;
	float beta = 0.5 * this->viewport_heght - 0.5;
	Mat_Init_4X4(&this->mscr, alpha, 0, 0, 0,
		0, -beta, 0, 0,
		0, 0, 1, 0,
		alpha, beta, 0, 1);
}

//��ʼ����ؾ���
void Camera::InitTransMatrix()
{
	if (this->attr == CAMERA_EULER)
		BuildMatrixCamEuler();
	else
		BuildMatrixCamUVN(UVN_SPHERICAL);

	BuildCameraToPerspectMatrix();
	BuildPerspectToScreenMatrix();
}

//����켣1
void Camera::BuildCameraPosAndDir(float distance, float view_angle)
{
	this->pos.x = distance * cos(view_angle / 180 * PI);
	this->pos.y = distance * sin(view_angle / 180 * PI);
	this->pos.z = 2 * this->pos.y;
}