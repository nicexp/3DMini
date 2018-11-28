///////////////////
//���߽ӿ�
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
//ȡ��֡��
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

//����λͼ����
int Load_Bitmap_File(BITMAP_FILE_PTR bitmap, char* filename)
{
	FILE* fp;
	UCHAR* temp_buffer = NULL;
	UINT red, green, blue;
	int index;


	//��ȡ�ļ�
	fp = fopen(filename, "rb");
	if (!fp)
	{
		DEBUG_LOG("open file %s error", filename);
		return 0;
	}
	//��ȡ�ļ�ͷ
	fread(&bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	//�Ƿ�Ϊλͼ�ļ�
	if (bitmap->bitmapfileheader.bfType != 0x4D42)
	{
		DEBUG_LOG("file is not bitmap��");
		fclose(fp);
		return 0;
	}
	//��ȡ��Ϣͷ��
	fread(&bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);
	//�ж��Ƿ��ȡ��ɫ��,8λģʽ���е�ɫ������
	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		fread(bitmap->palette, sizeof(PALETTEENTRY), 256, fp);
		//��BGRת��ΪRGB
		for (index = 0; index < 256; index++)
		{
			unsigned char tempcolor = bitmap->palette[index].peBlue;
			bitmap->palette[index].peBlue = bitmap->palette[index].peRed;
			bitmap->palette[index].peRed = tempcolor;
			bitmap->palette[index].peFlags = PC_NOCOLLAPSE;
		}
	}
	//��ȡλͼ��ɫ����
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
			//��ȡ����
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