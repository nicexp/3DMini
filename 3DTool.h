#ifndef _3DTOOL_H
#define _3DTOOL_H

#include <windows.h>
#include <Mmsystem.h>
#include "3DLib1.h"

void WriteLine2D(int xa, int ya, int xb, int yb, COLORREF c, HDC hdc);//2d����
float GetFPS();//��ȡ֡��
//����λͼ����
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char* filename);
#endif