#ifndef _3DSHADER_H
#define _3DSHADER_H

#include "3DLib1.h"
#include <Windows.h>

//ºã¶¨×ÅÉ«
void ShaderFlat(POLY4DV2_PTR face, int bLight, HDC hdc);
//Gauroud
void ShaderGouraud(POLY4DV2_PTR face, HDC hdc);

#endif