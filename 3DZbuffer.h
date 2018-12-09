#ifndef _3DZBUFFER_H
#define _3DZBUFFER_H

typedef struct ZBUFFER_TYP
{
	int attr; //����
	unsigned char* zbuffer; //z����
	int width; //���
	int height; //�߶�
	int sizeq; //��С����λ4���ֽ�
}ZBUFFER, *ZBUFFER_PTR;

int CreateZbuffer(ZBUFFER_PTR zb, int width, int height, int attr);
void UpdateZbuffer(ZBUFFER_PTR zb, unsigned int data);
int DeleteZbuffer(ZBUFFER_PTR zb);

#endif