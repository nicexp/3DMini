#include <stdlib.h>
#include "3DLib2.h"
#include "3DZbuffer.h"

unsigned int(*afbuffer)[256];

//Zbuffer和alpha buffer在这里实现
int CreateZbuffer(ZBUFFER_PTR zb, int width, int height, int attr)
{
	if (!zb)
		return 0;

	if (zb->zbuffer)
		free(zb->zbuffer);

	zb->width = width;
	zb->height = height;
	zb->attr = attr;
	zb->sizeq = width * height;

	if ((zb->zbuffer = (unsigned char*)malloc(width*height*sizeof(int))))
		return 1;
	else
		return 0;
}

void UpdateZbuffer(ZBUFFER_PTR zb, unsigned int data)
{
	Mem_Set_QUAD(zb->zbuffer, data, zb->sizeq);
}

int DeleteZbuffer(ZBUFFER_PTR zb)
{
	if (zb)
	{
		if (zb->zbuffer)
			free(zb->zbuffer);

		memset(zb, 0, sizeof(ZBUFFER));
	}
	else
	{
		return 0;
	}

	return 1;
}

int CreateAlphaBuffer()
{
	if (afbuffer)
		free(afbuffer);

	if (!(afbuffer = (unsigned int(*)[256])malloc(256 * 256 * sizeof(unsigned int))))
		return 0;
	

	for (int alpha = 0; alpha < 256; alpha++)
	{
		for (int cl = 0; cl < 256; cl++)
		{
			afbuffer[alpha][cl] = (alpha * cl) >> 8;
		}
	}

	return 1;
}

int DeleteAlphabuffer()
{
	if (afbuffer)
	{
		free(afbuffer);
	}
	else
	{
		return 0;
	}

	return 1;
}