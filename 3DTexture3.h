#ifndef _3DTEXTURE3_H
#define _3DTEXTURE3_H

#include "3DLib2.h"

void DrawTextureConstantWithPerInvzBinfilter(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
void DrawTextureConstantWithPerInvzAlpha(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch);
#endif