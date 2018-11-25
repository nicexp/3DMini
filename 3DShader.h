#ifndef _3DSHADER_H
#define _3DSHADER_H

#include "3DLib1.h"

//ºã¶¨×ÅÉ«
void ShaderFlat(POLY4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);
//Gauroud
void ShaderGouraud(POLY4DV2_PTR face, unsigned char *_dest_buffer, int mempitch);

#endif