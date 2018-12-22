#include "3DBitmap.h"

//创建位图
void CreateBitmap(BITMAP_IMG_PTR bitmap, int width, int height)
{
	bitmap->attr = 0;
	bitmap->state = 0;
	bitmap->width = width;
	bitmap->height = height;
#ifdef WINDDOW_BPP32
	bitmap->buffer = (UCHAR*)malloc(width * height*sizeof(UCHAR)* 4);
	bitmap->num_bytes = width * height*sizeof(UCHAR)* 4;
#else
	(*bitmap)->buffer = (UCHAR*)malloc(width * height*sizeof(UCHAR)* 2);
	(*bitmap)->num_bytes = width * height*sizeof(UCHAR)* 2;
#endif

	
	memset(bitmap->buffer, 0, bitmap->num_bytes);
}

//加载位图数据
int Load_Bitmap_File(BITMAP_IMG_PTR texture, char* filename)
{
	FILE* fp;
	BITMAP_FILE bp;
	BITMAP_FILE_PTR bitmap = &bp;
	UCHAR* tempbuffer = NULL;
	UINT red = 0, green = 0, blue = 0, alpha = 0;
	int index;

	//读取文件
	fp = fopen(filename, "rb");
	if (!fp)
	{
		DEBUG_LOG("open file %s error", filename);
		return 0;
	}
	//读取文件头
	fread(&bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	//是否为位图文件
	if (bitmap->bitmapfileheader.bfType != 0x4D42)
	{
		DEBUG_LOG("file is not bitmap！");
		fclose(fp);
		return 0;
	}
	//读取信息头部
	fread(&bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);
	DEBUG_LOG("bitcount:%d", bitmap->bitmapinfoheader.biBitCount);
	//为纹理创建位图
	CreateBitmap(texture, bitmap->bitmapinfoheader.biWidth, bitmap->bitmapinfoheader.biHeight);
	//判断是否读取调色板,8位模式下有调色板数据
	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		fread(bitmap->palette, sizeof(PALETTEENTRY), 256, fp);
		//将BGR转换为RGB
		for (index = 0; index < 256; index++)
		{
			unsigned char tempcolor = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed = tempcolor;
			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		}
	}
	//读取位图颜色数据
	fseek(fp, -(int)(bitmap->bitmapinfoheader.biSizeImage), SEEK_END);
	if (bitmap->bitmapinfoheader.biBitCount == 16)
	{
		DEBUG_LOG("16 bit bitmap");
		if (!(tempbuffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);
		//将位图颜色数据写入纹理数据
		for (int index = 0; index < bitmap->bitmapinfoheader.biWidth * bitmap->bitmapinfoheader.biHeight; index++)
		{
			UINT color = (UINT)((USHORT*)tempbuffer)[index];
			_RGB565FROM16BIT(color, &red, &green, &blue);
#ifdef WINDDOW_BPP32
			((UINT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		free(tempbuffer);
	}
	if (bitmap->bitmapinfoheader.biBitCount == 24)
	{
		DEBUG_LOG("24 bit bitmap");
		if (!(tempbuffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(tempbuffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);

		for (index = 0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
		{
			//读取像素
			blue = (int)tempbuffer[index * 3 + 0];
			green = (int)tempbuffer[index * 3 + 1];
			red = (int)tempbuffer[index * 3 + 2];
#ifdef WINDDOW_BPP32
			((UINT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		free(tempbuffer);
	}
	if (bitmap->bitmapinfoheader.biBitCount == 32)
	{
		DEBUG_LOG("32 bit bitmap");
		if (!(tempbuffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(tempbuffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);

		for (index = 0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
		{
			//读取像素
			blue = (int)tempbuffer[index * 4 + 0];
			green = (int)tempbuffer[index * 4 + 1];
			red = (int)tempbuffer[index * 4 + 2];
			alpha = (int)tempbuffer[index * 4 + 3];
#ifdef WINDDOW_BPP32
			((UINT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)texture->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		free(tempbuffer);
	}

	fclose(fp);
	return 1;
}