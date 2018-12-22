#ifndef _3DMIPMAP_H
#define _3DMIPMAP_H

#include "3DLib2.h"

void CreateMipmap(BITMAP_IMG_PTR source, BITMAP_IMG_PTR* mipmap, float param);
void OperWithMipmap(POLYF4DV2_PTR poly);
#endif