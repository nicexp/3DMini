#ifndef _3DSHADER_H
#define _3DSHADER_H

#include "3DLib1.h"

//�㶨��ɫ
void ShaderFlat(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);
//Gauroud
void ShaderGouraud(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);

#endif