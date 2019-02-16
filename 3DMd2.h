#ifndef _3DMD2_H
#define _3DMD2_H

#include "3DLib1.h"
#include "3DMesh.h"

#define MD2_MAGIC_NUM (('I') + ('D' << 8) + ('P' << 16) + ('2' << 24))
#define MD2_VERSION 8

//md2�ļ�ͷ
typedef struct MD2_HEADER_TYP
{
	int identifier; //�ļ����ͣ�ӦΪIDP2
	int version; //�汾�ţ�Ϊ8
	int skin_width; //����Ƥ��������ͼ���
	int skin_height; //����Ƥ��������ͼ�߶�
	int framesize; //ÿ������֡�Ĵ�С����λΪ�ֽ�
	int num_skins; //��Ƥ����
	int num_verts; //ÿ֡�Ķ�����
	int num_textcoords; //������������
	int num_polys; //ÿ֡����
	int num_openglcmds;//opengl������
	int num_frames; //��֡��

	int offset_skins; //Ƥ�������ļ�ƫ��λ�ã���λΪ�ֽڣ�һ������64���ֽڣ��洢��Ƥ���ļ���
	int offset_textcoords; //�������������ļ�ƫ��
	int offset_polys; //�������ļ�ƫ��
	int offset_frames; //֡�����ļ�ƫ��
	int offset_openglcmds; //opengl���������ļ�ƫ��
	int offset_end; //�ļ���ͷ���ļ�β�ľ���
}MD2_HEADER, *MD2_HEADER_PTR;

//md2�����������ݽṹ
typedef struct MD2_TEXTCOORDS_TYP 
{
	short u, v;//��������
}MD2_TEXTCOORDS, *MD2_TEXTCOORDS_PTR;

//md2��������ݽṹ
typedef struct MD2_POLY_TYP
{
	unsigned short vindex[3]; //������������
	unsigned short tindex[3]; //������������
}MD2_POLY, *MD2_POLY_PTR;

//md2�������ݽṹ
typedef struct MD2_POINT_TYP
{
	unsigned char v[3];
	unsigned char normal_index;
}MD2_POINT, *MD2_POINT_PTR;

//md2֡���ݽṹ
typedef struct  MD2_FRAME_TYP
{
	float scale[3]; //������������
	float translate[3]; //����ƽ������
	char name[16]; //֡����
	MD2_POINT vlist[1]; //�����б�
}MD2_FRAME, *MD2_FRAME_PTR;

void LoadMd2FileToMesh(MODEL_PTR model, const char* modelpath, const char* skinpath, MESH_PTR mesh);

#endif