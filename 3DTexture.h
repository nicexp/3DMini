#ifndef _3DTEXTURE_H
#define _3DTEXTURE_H

#include "3DLib2.h"

void DrawTextureConstant(POLY4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch);
void DrawTextureGouraud(POLY4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch);
void DrawTextureFlat(POLY4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch);
#endif