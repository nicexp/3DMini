#ifndef _3DZBUFFER_H
#define _3DZBUFFER_H

typedef struct ZBUFFER_TYP
{
	int attr; //属性
	unsigned char* zbuffer; //z缓存
	int width; //宽度
	int height; //高度
	int sizeq; //大小，单位4个字节
}ZBUFFER, *ZBUFFER_PTR;

int CreateZbuffer(ZBUFFER_PTR zb, int width, int height, int attr);
void UpdateZbuffer(ZBUFFER_PTR zb, unsigned int data);
int DeleteZbuffer(ZBUFFER_PTR zb);

#endif