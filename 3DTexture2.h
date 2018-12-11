#ifndef _3DTEXTURE2_H
#define _3DTEXTURE2_H

#include "3DLib2.h"

void DrawTextureConstant2(POLYF4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
void DrawTextureGouraud2(POLYF4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
void DrawTextureFlat2(POLYF4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
#endif