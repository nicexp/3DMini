#ifndef _3DTEXTURE2_H
#define _3DTEXTURE2_H

#include "3DLib2.h"

void DrawTextureConstantWithPerInvz(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
void DrawTextureGouraudWithPerInvz(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
void DrawTextureFlatWithPerInvz(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);

#endif