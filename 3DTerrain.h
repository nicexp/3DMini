#ifndef _3DTERRAIN_H
#define _3DTERRAIN_H

#include "3DMesh.h"

typedef struct TERRAIN_TYP
{
	MESH mesh; //网格数据

	float width; //地形宽度
	float length; //地形长度
	int col;	//高度图宽度
	int row;	//高度图长度
	int maxheight; //地图最大高度

	VECTOR4D_PTR gridNormal; //地格面片法线，用于地形跟踪
}TERRAIN, *TERRAIN_PTR;

void GenerateTerrain(TERRAIN_PTR terrain, float width, float length, float maxheight, const char* fileheightmap, const char* filetextmap);

#endif