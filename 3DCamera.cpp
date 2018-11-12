#include "3DCamera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

//初始化相机机构
void Camera::InitCamera(int cam_attr,//相机属性
	POINT4D_PTR cam_pos,//相机位置
	VECTOR4D_PTR cam_dir,//相机朝向
	POINT4D_PTR cam_target, //uvn相机初始目标位置
	float near_clip_z,//近裁剪面
	float far_clip_z,//远裁剪面
	float fov,//视野
	float viewport_width,//视口宽度
	float viewport_height)//视口高度
{
	this->attr = cam_attr;
	VECTOR4D_COPY(&this->pos, cam_pos);
	//相机欧拉角转换为弧度
	VECTOR4D_COPY(&this->dir, cam_dir);
	//对于UVN相机
	VECTOR4D_INITXYZ(&this->u, 1, 0, 0); //设置为x轴方向
	VECTOR4D_INITXYZ(&this->v, 0, 1, 0); //设置为y轴方向
	VECTOR4D_INITXYZ(&this->n, 0, 0, 1); //设置为z轴方向

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
	//将所有变换矩阵设置为单位矩阵
	Mat_IDENTITY_4X4(&this->mcam);
	Mat_IDENTITY_4X4(&this->mper);
	Mat_IDENTITY_4X4(&this->mscr);

	this->fov = fov / 180 * PI;
	//设置视平面大小
	this->viewplane_width = 2.0;
	this->viewplane_height = 2.0 / this->aspect_radio;
	//视距
	this->view_dist = (this->viewplane_width / 2) / tan(this->fov / 2);
}

//投建欧拉相机矩阵
void Camera::BuildMatrixCamEuler()
{
	MATRIX4X4 mt_inv, mx_inv, my_inv, mz_inv, mrot, mtmp;
	//相机平移矩阵
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-this->pos.x, -this->pos.y, -this->pos.z, 1);
	//旋转矩阵
	double theta_x = this->dir.x;
	double theta_y = this->dir.y;
	double theta_z = this->dir.z;
	//x的正弦和余弦
	double cos_theta = cos(theta_x);
	double sin_theta = -sin(theta_x);
	Mat_Init_4X4(&mx_inv, 1, 0, 0, 0,
		0, cos_theta, sin_theta, 0,
		0, -sin_theta, cos_theta, 0,
		0, 0, 0, 1);
	//y的正弦和余弦
	cos_theta = cos(theta_y);
	sin_theta = -sin(theta_y);
	Mat_Init_4X4(&my_inv, cos_theta, 0, -sin_theta, 0,
		0, 1, 0, 0,
		sin_theta, 0, cos_theta, 0,
		0, 0, 0, 1);
	//z的正弦和余弦
	cos_theta = cos(theta_z);
	sin_theta = -sin(theta_z);
	Mat_Init_4X4(&mz_inv, cos_theta, sin_theta, 0, 0,
		-sin_theta, cos_theta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	//逆矩阵积yxz顺序
	Mat_Mul_4X4(&my_inv, &mx_inv, &mtmp);
	Mat_Mul_4X4(&mtmp, &mz_inv, &mrot);
	//相机变换矩阵
	Mat_Mul_4X4(&mt_inv, &mrot, &this->mcam);
}

//构建UVN相机矩阵(简单/球形)
void Camera::BuildMatrixCamUVN(int mode)
{
	MATRIX4X4 mt_inv, mt_uvn, mt_tmp;
	//平移矩阵
	Mat_Init_4X4(&mt_inv, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-this->pos.x, -this->pos.y, -this->pos.z, 1);
	if (mode == UVN_SPHERICAL)
	{
		//目标点
		float phi = this->dir.x; //仰角(绕x轴旋转的角度,与正y轴重合时为0,范围是0-180)
		float theta = this->dir.y; //方位角(绕y轴旋转,与正x轴重合时为0)

		this->target.x = 1.0 * sin(phi) * cos(theta);
		this->target.y = 1.0 * cos(phi);
		this->target.z = 1.0 * sin(phi) * sin(theta);
	}

	if (mode == UVN_SPHERICAL)
	{
		//n向量
		VECTOR4D_COPY(&this->n, &this->target);
	}
	else
	{
		VECTOR4D_SUB(&this->target, &this->pos, &this->n);
	}
	//初始化v向量
	VECTOR4D_INITXYZ(&this->v, 0, 1, 0);
	//u向量
	VECTOR4D_CROSS(&this->v, &this->n, &this->u);
	//反求v
	VECTOR4D_CROSS(&this->n, &this->u, &this->v);
	//归一化
	VECTOR4D_Normalize(&this->u);
	VECTOR4D_Normalize(&this->v);
	VECTOR4D_Normalize(&this->n);
	//uvn旋转矩阵
	Mat_Init_4X4(&mt_uvn, this->u.x, this->v.x, this->n.x, 0,
		this->u.y, this->v.y, this->n.y, 0,
		this->u.z, this->v.z, this->n.z, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&mt_inv, &mt_uvn, &this->mcam);
}

//构建透视变换矩阵
void Camera::BuildCameraToPerspectMatrix()
{
	Mat_Init_4X4(&this->mper, this->view_dist, 0, 0, 0,
		0, this->view_dist*this->aspect_radio, 0, 0,
		0, 0, 1, 1,
		0, 0, 0, 0);
}

//构建屏幕变换矩阵
void Camera::BuildPerspectToScreenMatrix()
{
	float alpha = 0.5 * this->viewport_width - 0.5;
	float beta = 0.5 * this->viewport_heght - 0.5;
	Mat_Init_4X4(&this->mscr, alpha, 0, 0, 0,
		0, -beta, 0, 0,
		0, 0, 1, 0,
		alpha, beta, 0, 1);
}

//初始化相关矩阵
void Camera::InitTransMatrix()
{
	if (this->attr == CAMERA_EULER)
		BuildMatrixCamEuler();
	else
		BuildMatrixCamUVN(UVN_SPHERICAL);

	BuildCameraToPerspectMatrix();
	BuildPerspectToScreenMatrix();
}

//相机轨迹1
void Camera::BuildCameraPosAndDir(float distance, float view_angle)
{
	this->pos.x = distance * cos(view_angle / 180 * PI);
	this->pos.y = distance * sin(view_angle / 180 * PI);
	this->pos.z = 2 * this->pos.y;
}