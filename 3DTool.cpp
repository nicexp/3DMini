///////////////////
//工具接口
//////////////////
#include "3DTool.h"
#include "3DLib1.h"
#include "3DLog.h"

void WriteLine2D(int xa, int ya, int xb, int yb, COLORREF c, HDC hdc)
{
	float deltax, deltay, x, y;
	int dx, dy, steps;

	dx = xb - xa;
	dy = yb - ya;

	if (abs(dx) > abs(dy))
		steps = abs(dx);
	else
		steps = abs(dy);

	deltax = (float)dx / (float)steps;
	deltay = (float)dy / (float)steps;

	x = xa;
	y = ya;

	SetPixel(hdc, x, y, c);
	for (int k = 1; k <= steps; k++)
	{
		x += deltax;
		y += deltay;
		SetPixel(hdc, x, y, c);
	}
}
//取得帧率
float GetFPS()
{
	static float  fps = 0;
	static int    frameCount = 0;
	static float  currentTime;
	static float  lastTime = timeGetTime()*0.001f;
	frameCount++;
	currentTime = timeGetTime()*0.001f;
	if (currentTime - lastTime >= 1.0f)
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime; 
		frameCount = 0;
	}
	return fps;
}

//加载位图数据
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char* filename)
{
	FILE* fp;
	UCHAR* temp_buffer = NULL;
	UINT red, green, blue;
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
	if (bitmap->bitmapinfoheader.biBitCount == 8 || bitmap->bitmapinfoheader.biBitCount == 16)
	{
		DEBUG_LOG("8/16 bit bitmap");
		if (bitmap->buffer)
			free(bitmap->buffer);

		if (!(bitmap->buffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}

		fread(bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);
	}
	if (bitmap->bitmapinfoheader.biBitCount == 24)
	{
		DEBUG_LOG("24 bit bitmap");
		if (!(temp_buffer = (UCHAR*)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			fclose(fp);
			return 0;
		}
#ifdef WINDDOW_BPP32
		if (!(bitmap->buffer = (UCHAR*)malloc(4 * bitmap->bitmapinfoheader.biWidth * bitmap->bitmapinfoheader.biHeight)))
		{
			fclose(fp);
			return 0;
		}
#else
		if (!(bitmap->buffer = (UCHAR*)malloc(2 * bitmap->bitmapinfoheader.biWidth * bitmap->bitmapinfoheader.biHeight)))
		{
			fclose(fp);
			free(temp_buffer);
			return 0;
		}
#endif

		fread(temp_buffer, bitmap->bitmapinfoheader.biSizeImage, 1, fp);

		for (index = 0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
		{
			//读取像素
			blue = (int)temp_buffer[index * 3 + 0];
			green = (int)temp_buffer[index * 3 + 1];
			red = (int)temp_buffer[index * 3 + 2];
#ifdef WINDDOW_BPP32
			((UINT*)bitmap->buffer)[index] = _RGBTOINT(red, green, blue);
#else
			((USHORT*)bitmap->buffer)[index] = _RGBTOINT(red, green, blue);
#endif
		}
		bitmap->bitmapinfoheader.biBitCount = 16;
		free(temp_buffer);
	}

	fclose(fp);
	return 1;
}