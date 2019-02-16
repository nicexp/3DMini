#ifndef _3DBITMAP_H
#define _3DBITMAP_H

#include "3DLib2.h"

//创建位图数据
void CreateBitmap(BITMAP_IMG_PTR bitmap, int width, int height);
//销毁位图数据
void DestroyBitmap(BITMAP_IMG_PTR bitmap);
//移动位图数据
void ScrollBitmap(BITMAP_IMG_PTR image, int dx);
//拷贝位图数据
void CopyBitmap(BITMAP_IMG_PTR dest_bitmap, int dest_x, int dest_y,BITMAP_IMG_PTR source_bitmap, int source_x, int source_y,int width, int height);
//加载位图数据
int Load_Bitmap_File(BITMAP_IMG_PTR bitmap, const char* filename);
//加载png到位图结构
void Load_Png_File(BITMAP_IMG_PTR bitmap, const char* filename);
//加载jpeg到位图结构
void Load_Jpeg_File(BITMAP_IMG_PTR bitmap, const char* filename);
#endif