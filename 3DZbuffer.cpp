#include <stdlib.h>
#include "3DLib2.h"
#include "3DZbuffer.h"

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
}