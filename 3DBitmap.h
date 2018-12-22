#ifndef _3DBITMAP_H
#define _3DBITMAP_H

#include "3DLib1.h"

//加载位图数据
int Load_Bitmap_File(BITMAP_IMG_PTR bitmap, char* filename);
//创建位图数据
void CreateBitmap(BITMAP_IMG_PTR bitmap, int width, int height);
#endif