#ifndef _3DMD2_H
#define _3DMD2_H

#include "3DLib1.h"
#include "3DMesh.h"

#define MD2_MAGIC_NUM (('I') + ('D' << 8) + ('P' << 16) + ('2' << 24))
#define MD2_VERSION 8

//md2文件头
typedef struct MD2_HEADER_TYP
{
	int identifier; //文件类型，应为IDP2
	int version; //版本号，为8
	int skin_width; //用作皮肤的纹理图宽度
	int skin_height; //用作皮肤的纹理图高度
	int framesize; //每个动画帧的大小，单位为字节
	int num_skins; //总皮肤数
	int num_verts; //每帧的顶点数
	int num_textcoords; //纹理坐标数量
	int num_polys; //每帧面数
	int num_openglcmds;//opengl命令数
	int num_frames; //总帧数

	int offset_skins; //皮肤数组文件偏移位置，单位为字节，一个数组64个字节，存储了皮肤文件名
	int offset_textcoords; //纹理坐标数组文件偏移
	int offset_polys; //面数组文件偏移
	int offset_frames; //帧数据文件偏移
	int offset_openglcmds; //opengl命令数组文件偏移
	int offset_end; //文件开头到文件尾的距离
}MD2_HEADER, *MD2_HEADER_PTR;

//md2纹理坐标数据结构
typedef struct MD2_TEXTCOORDS_TYP 
{
	short u, v;//纹理坐标
}MD2_TEXTCOORDS, *MD2_TEXTCOORDS_PTR;

//md2多边形数据结构
typedef struct MD2_POLY_TYP
{
	unsigned short vindex[3]; //顶点坐标索引
	unsigned short tindex[3]; //纹理坐标索引
}MD2_POLY, *MD2_POLY_PTR;

//md2顶点数据结构
typedef struct MD2_POINT_TYP
{
	unsigned char v[3];
	unsigned char normal_index;
}MD2_POINT, *MD2_POINT_PTR;

//md2帧数据结构
typedef struct  MD2_FRAME_TYP
{
	float scale[3]; //顶点缩放因子
	float translate[3]; //顶点平移因子
	char name[16]; //帧描述
	MD2_POINT vlist[1]; //顶点列表
}MD2_FRAME, *MD2_FRAME_PTR;

void LoadMd2FileToMesh(MODEL_PTR model, const char* modelpath, const char* skinpath, MESH_PTR mesh);

#endif