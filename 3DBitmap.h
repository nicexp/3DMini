#ifndef _3DBITMAP_H
#define _3DBITMAP_H

#include "3DLib1.h"

//����λͼ����
int Load_Bitmap_File(BITMAP_IMG_PTR bitmap, char* filename);
//����λͼ����
void CreateBitmap(BITMAP_IMG_PTR bitmap, int width, int height);
#endif