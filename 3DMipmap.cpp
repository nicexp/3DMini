#include "3DMipmap.h"
#include "3DBitmap.h"
//经过mipmap缩小后的图会变暗，通过param来进行增强 //暂时不支持16位位深
void CreateMipmap(BITMAP_IMG_PTR source, BITMAP_IMG_PTR* mipmap, float param)
{
	int r00, r01, r10, r11, rres;
	int g00, g01, g10, g11, gres;
	int b00, b01, b10, b11, bres;
	UCHAR *srcbuffer, *desbuffer;

	int mip_level = (int)(log2((float)source->width)) + 1;

	BITMAP_IMG_PTR* tmpmap = (BITMAP_IMG_PTR*)malloc(mip_level*sizeof(BITMAP_IMG_PTR*));
	tmpmap[0] = source;

	for (int level = 1; level < mip_level; level++)
	{
		int width = tmpmap[level - 1]->width * 0.5;
		int height = tmpmap[level - 1]->height * 0.5;

		tmpmap[level] = (BITMAP_IMG_PTR)malloc(sizeof(BITMAP_IMG));
		CreateBitmap(tmpmap[level], width, height);

		srcbuffer = tmpmap[level - 1]->buffer;
		desbuffer = tmpmap[level]->buffer;

		//读取数据
		for (int hi = 0; hi < height; hi++)
		{
			for (int  wi= 0; wi < width; wi++)
			{
				_RGBFROMINT(((UINT*)srcbuffer)[(wi * 2 + 0) + (hi * 2 + 0) * width * 2], &r00, &g00, &b00);
				_RGBFROMINT(((UINT*)srcbuffer)[(wi * 2 + 1) + (hi * 2 + 0) * width * 2], &r10, &g10, &b10);
				_RGBFROMINT(((UINT*)srcbuffer)[(wi * 2 + 0) + (hi * 2 + 1) * width * 2], &r01, &g01, &b01);
				_RGBFROMINT(((UINT*)srcbuffer)[(wi * 2 + 1) + (hi * 2 + 1) * width * 2], &r11, &g11, &b11);

				rres = (int)((r00 + r01 + r10 + r11)* 0.25 * param + 0.5);
				gres = (int)((g00 + g01 + g10 + g11)* 0.25 * param + 0.5);
				bres = (int)((b00 + b01 + b10 + b11)* 0.25 * param + 0.5);
				//考虑到param过大要截取
				if (rres > 255) { rres = 255; }
				if (gres > 255) { gres = 255; }
				if (bres > 255) { bres = 255; }

				((UINT*)desbuffer)[wi + hi * width] = _RGBTOINT(rres, gres, bres);
			}
		}
	}

	(*mipmap) = (BITMAP_IMG_PTR)tmpmap;
}

//根据映射到屏幕的平面选择对应的mipmap纹理
void OperWithMipmap(POLYF4DV2_PTR poly)
{
	int miplevel = 0;
	VECTOR4D va, vb, vc;
	BITMAP_IMG_PTR* mipmap = (BITMAP_IMG_PTR*)poly->texture;
	//纹理面积
	int texture_area = mipmap[0]->width * mipmap[0]->height;
	//屏幕上的面积
	VECTOR4D_SUB(&poly->tvlist[1].v, &poly->tvlist[0].v, &va);
	VECTOR4D_SUB(&poly->tvlist[2].v, &poly->tvlist[0].v, &vb);
	VECTOR4D_CROSS(&va, &vb, &vc);
	int screen_area = (int)(VECTOR4D_Length(&vc) * 0.5);
	if (screen_area < texture_area)
	{
		float ratio = texture_area / screen_area;
		miplevel = (int)(log(ratio) / log(4.0));
	}
	
	for (int i = 0; i < miplevel; i++)
	{
		poly->tvlist[0].u0 *= 0.5;
		poly->tvlist[0].v0 *= 0.5;
		poly->tvlist[1].u0 *= 0.5;
		poly->tvlist[1].v0 *= 0.5;
		poly->tvlist[2].u0 *= 0.5;
		poly->tvlist[2].v0 *= 0.5;
	}
	for (int i = 0; i < 3; i++)
	{
		if (poly->tvlist[i].u0 >(mipmap[miplevel]->width - 1))
		{
			poly->tvlist[i].u0 = mipmap[miplevel]->width - 1;
		}
		if (poly->tvlist[i].v0 >(mipmap[miplevel]->width - 1))
		{
			poly->tvlist[i].v0 = mipmap[miplevel]->width - 1;
		}
	}

	poly->texture = (BITMAP_IMG_PTR)(mipmap[miplevel]);
}