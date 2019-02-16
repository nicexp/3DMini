#include "3DCamera.h"
#include "3DLib2.h"

#define CAMERA_SPEED 20

static float max_angle_y = PI * 170 / 180;
static float min_angle_y = PI * 10 / 180;

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
	VECTOR4D_INITXYZ(&cam->v, 0, 1, 0); //��n��v�ص�ʱ��������,���Ա���n��v�ص�
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
		BuildMatrixCamUVN(cam, UVN_SPHERICAL);

	BuildCameraToPerspectMatrix(cam);
	BuildPerspectToScreenMatrix(cam);
}

//�������λ��
void SetCameraPos(CAM4DV1_PTR cam, int direct, float speed)
{
	if (direct == DIRECT_FRONT)
	{
		cam->pos.x += cam->n.x * speed;
		cam->pos.z += cam->n.z * speed;
	}
	else if (direct == DIRECT_AFTER)
	{
		cam->pos.x += cam->n.x * -speed;
		cam->pos.z += cam->n.z * -speed;
	}
	else if (direct == DIRECT_LEFT)
	{
		cam->pos.x += cam->u.x * -speed;
		cam->pos.z += cam->u.z * -speed;
	}
	else if (direct == DIRECT_RIGHT)
	{
		cam->pos.x += cam->u.x * speed;
		cam->pos.z += cam->u.z * speed;
	}
	else if (direct == DIRECT_UP)
	{
		cam->pos.y += speed;
	}
	else if (direct == DIRECT_DOWN)
	{
		cam->pos.y -= speed;
	}
}

void UpdateCameraPosAndDir(CAM4DV1_PTR cam)
{
	if (KEY_DOWN(0x57) || keyboard_state[DIK_W])
	{
		SetCameraPos(cam, DIRECT_FRONT, CAMERA_SPEED);
	}
	if (KEY_DOWN(0x41) || keyboard_state[DIK_A])
	{
		SetCameraPos(cam, DIRECT_LEFT, CAMERA_SPEED);
	}
	if (KEY_DOWN(0x53) || keyboard_state[DIK_S])
	{
		SetCameraPos(cam, DIRECT_AFTER, CAMERA_SPEED);
	}
	if (KEY_DOWN(0x44) || keyboard_state[DIK_D])
	{
		SetCameraPos(cam, DIRECT_RIGHT, CAMERA_SPEED);
	}
	if (KEY_DOWN(0x45) || keyboard_state[DIK_E])
	{
		SetCameraPos(cam, DIRECT_UP, CAMERA_SPEED);
	}
	if (KEY_DOWN(0x51) || keyboard_state[DIK_Q])
	{
		SetCameraPos(cam, DIRECT_DOWN, CAMERA_SPEED);
	}
	if (KEY_DOWN(0xCB) || keyboard_state[DIK_LEFT])
	{
		cam->dir.y += (15.0 / 360.0 * PI);
		if (cam->dir.y < 0)
		{
			cam->dir.y += PI * 2;
		}
	}
	if (KEY_DOWN(0xCD) || keyboard_state[DIK_RIGHT])
	{
		cam->dir.y -= (15.0 / 360.0 * PI);
		if (cam->dir.y < 0)
		{
			cam->dir.y += PI * 2;
		}
	}
	if ((mouse_state.rgbButtons[0] & 0x80))
	{
		int lx = mouse_state.lX, ly = mouse_state.lY;
		cam->dir.y -= (((float)(lx)) / (180 * MOUSE_TO_EULER_UNIT) * PI);
		cam->dir.x += (((float)(ly)) / (180 * MOUSE_TO_EULER_UNIT) * PI);
		if (cam->dir.y < 0)
		{
			cam->dir.y += PI*2;
		}
		if (cam->dir.x > max_angle_y)
		{
			cam->dir.x = max_angle_y;
		}
		if (cam->dir.x < min_angle_y)
		{
			cam->dir.x = min_angle_y;
		}
	}
}

void UpdateCameraPosAndDirSimple(CAM4DV1_PTR cam)
{
	static float view_angle_x = 0;
	static float view_angle_y = 90;
	static int distance = 2000;
	
	int lx = mouse_state.lX, ly = mouse_state.lY, lz = mouse_state.lZ;

	distance += lz / MOUSE_TO_EULER_UNIT;
	if (distance > 1500)
		distance = 1500;
	if (distance < 500)
		distance = 500;

	if ((mouse_state.rgbButtons[0] & 0x80))
	{
		view_angle_x -= ((float)(lx)) / MOUSE_TO_EULER_UNIT;
		view_angle_y -= ((float)(ly)) / MOUSE_TO_EULER_UNIT;
		if (view_angle_x < 0)
			view_angle_x += 360;

		if (view_angle_y >= 170)
			view_angle_y = 170;
		if (view_angle_y <= 10)
			view_angle_y = 10;
	}

	float phi = view_angle_y / 180 * PI; //����(��x����ת�ĽǶ�,����y���غ�ʱΪ0,��Χ��0-180)
	float theta = view_angle_x / 180 * PI; //��λ��(��y����ת,����x���غ�ʱΪ0)

	cam->pos.x = cam->target.x + distance * sin(phi) * cos(theta);
	cam->pos.y = cam->target.y + distance * cos(phi);
	cam->pos.z = cam->target.z + distance * sin(phi) * sin(theta);

	if (cam->pos.y <= 20)
		cam->pos.y = 20;
}

//����켣1
void BuildCameraPosAndDir(CAM4DV1_PTR cam,float distance)
{
	static float view_angle = 0;
	if ((view_angle += 1) >= 360)
		view_angle = 0;

	cam->pos.x = distance * cos(view_angle / 180 * PI);
	cam->pos.y = distance * sin(view_angle / 180 * PI);
	cam->pos.z = 2 * cam->pos.y;
}

void SetCameraTargetPos(CAM4DV1_PTR cam, VECTOR4D_PTR pos)
{
	VECTOR4D_COPY(&cam->target, pos);
}