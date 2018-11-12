#include "3DLib1.h"
#include <math.h>

//ʸ����
void VECTOR4D_ADD(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc)
{
	vc->x = va->x + vb->x;
	vc->y = va->y + vb->y;
	vc->z = va->z + vb->z;
	vc->w = 1;
}

//ʸ����
void VECTOR4D_SUB(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc)
{
	vc->x = va->x - vb->x;
	vc->y = va->y - vb->y;
	vc->z = va->z - vb->z;
	vc->w = 1;
}

//ʸ�����
float VECTOR4D_DOT(VECTOR4D_PTR va, VECTOR4D_PTR vb)
{
	float res = va->x * vb->x + va->y * vb->y + va->z * vb->z;
	return res;
}

//ʸ�����
void VECTOR4D_CROSS(VECTOR4D_PTR va, VECTOR4D_PTR vb, VECTOR4D_PTR vc)
{
	vc->x = va->y*vb->z - vb->y*va->z;
	vc->y = vb->x*va->z - va->x*vb->z;
	vc->z = va->x*vb->y - vb->x*va->y;
	vc->w = 1;
}

//������һ��
void VECTOR4D_Normalize(VECTOR4D_PTR va)
{
	float length = sqrtf(va->x*va->x+va->y*va->y+va->z*va->z);
	float length_inv = 1.0 / length;

	va->x = va->x *length_inv;
	va->y = va->y *length_inv;
	va->z = va->z *length_inv;
}

//�����������
void VECTOR4D_DIV_BY_W(VECTOR4D_PTR va)
{
	va->x = va->x / va->w;
	va->y = va->y / va->w;
	va->z = va->z / va->w;
	va->w = 1;
}

//��������
void VECTOR4D_COPY(VECTOR4D_PTR res, VECTOR4D_PTR src)
{
	res->x = src->x;
	res->y = src->y;
	res->z = src->z;
	res->w = src->w;
}

//������ʼ��
void VECTOR4D_INITXYZ(VECTOR4D_PTR vt, float x, float y, float z)
{
	vt->x = x;
	vt->y = y;
	vt->z = z;
	vt->w = 1;
}
//�����ʼ��
void Mat_Init_4X4(MATRIX4X4_PTR ma,
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)

{
	ma->M00 = m00; ma->M01 = m01; ma->M02 = m02; ma->M03 = m03;
	ma->M10 = m10; ma->M11 = m11; ma->M12 = m12; ma->M13 = m13;
	ma->M20 = m20; ma->M21 = m21; ma->M22 = m22; ma->M23 = m23;
	ma->M30 = m30; ma->M31 = m31; ma->M32 = m32; ma->M33 = m33;

}

//4x4���õ�λ����
void Mat_IDENTITY_4X4(MATRIX4X4_PTR ma)
{
	Mat_Init_4X4(ma, 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

//��;���ʸ���任����
void Mat_Mul_VECTOR4D_4X4(VECTOR4D_PTR va, MATRIX4X4_PTR mb, VECTOR4D_PTR vprod)
{
	for (int i = 0; i < 4; i++)
	{
		float tmp = 0;
		for (int j = 0; j < 4; j++)
		{
			tmp += va->M[j] * mb->M[j][i];
		}
		vprod->M[i] = tmp;
	}
}

//����;���任����
void Mat_Mul_4X4(MATRIX4X4_PTR ma, MATRIX4X4_PTR mb, MATRIX4X4_PTR mc)
{
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			float tmp = 0;
			for (int i = 0; i < 4; i++)
			{
				tmp += ma->M[row][i] * mb->M[i][col];
			}
			mc->M[row][col] = tmp;
		}
	}
}

//���ŷ���Ƕ�ת��
void Eu_Dir_Transform(VECTOR4D_PTR dir)
{
	dir->x = (dir->x / 180)*PI;
	dir->y = (dir->y / 180)*PI;
	dir->z = (dir->z / 180)*PI;
	dir->w = 1;
}