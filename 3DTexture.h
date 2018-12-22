#ifndef _3DTEXTURE_H
#define _3DTEXTURE_H

#include "3DLib2.h"

void DrawTextureConstant(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);
void DrawTextureGouraud(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);
void DrawTextureFlat(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);
#endif