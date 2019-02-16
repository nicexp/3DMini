#ifndef _3DBITMAP_H
#define _3DBITMAP_H

#include "3DLib2.h"

//����λͼ����
void CreateBitmap(BITMAP_IMG_PTR bitmap, int width, int height);
//����λͼ����
void DestroyBitmap(BITMAP_IMG_PTR bitmap);
//�ƶ�λͼ����
void ScrollBitmap(BITMAP_IMG_PTR image, int dx);
//����λͼ����
void CopyBitmap(BITMAP_IMG_PTR dest_bitmap, int dest_x, int dest_y,BITMAP_IMG_PTR source_bitmap, int source_x, int source_y,int width, int height);
//����λͼ����
int Load_Bitmap_File(BITMAP_IMG_PTR bitmap, const char* filename);
//����png��λͼ�ṹ
void Load_Png_File(BITMAP_IMG_PTR bitmap, const char* filename);
//����jpeg��λͼ�ṹ
void Load_Jpeg_File(BITMAP_IMG_PTR bitmap, const char* filename);
#endif