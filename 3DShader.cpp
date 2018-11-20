#include "3DShader.h"
#include <Windows.h>

//描绘平顶三角形
void ShaderFlatTriTop(float x1, float y1, float x2, float y2, float x3, float y3, int color, HDC hdc)
{
	float dx_left, dx_right, xs, xe, left, right;
	float temp_x;
	int ys, ye;

	//裁剪范围
	int min_clip_x = 0, min_clip_y = 0;
	int max_clip_x = WINDOW_WIDTH - 1, max_clip_y = WINDOW_HEIGHT - 1;
	//颜色
	int r = 0, g = 0, b = 0;
	_RGB565FROM16BIT(color, &r, &g, &b);
	r << 3; g << 2; b << 3;

	//保证顶点顺序
	if (x2 < x1)
		SWAP(x2, x1, temp_x);

	dx_left = (x3 - x1) / (y3 - y1);
	dx_right = (x3 - x2) / (y3 - y1);
	xs = x1; xe = x2;
	//垂直裁剪
	if (y1 < min_clip_y)
	{
		xs = xs + (min_clip_y - y1)*dx_left;
		xe = xe + (min_clip_y - y1)*dx_right;

		ys = min_clip_y;
	}
	else
	{
		ys = ceil(y1);

		xs = xs + (ys - y1)*dx_left;
		xe = xe + (ys - y1)*dx_right;
	}
	if (y3 > max_clip_y)
		ye = max_clip_y - 1;
	else
		ye = ceil(y3) - 1;
	//水平裁剪
	if (x1 > min_clip_x && x1 < max_clip_x && x2 > min_clip_x
		&& x2 < max_clip_x && x3 > min_clip_x && x3 < max_clip_x)
	{
		for (int yi = ys; yi <= ye; yi++)
		{
			for (int xi = ceil(xs); xi < ceil(xe) - 1; xi++)
			{
				SetPixel(hdc, xi, yi, RGB(r, g, b));
			}

			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (int yi = ys; yi <= ye; yi++)
		{
			left = xs; right = xe;
			if (left < min_clip_x)
			{
				left = min_clip_x;
				if (right < min_clip_x)
					continue;
			}

			if (right > max_clip_x)
			{
				right = max_clip_x;
				if (left > max_clip_x)
					continue;
			}

			for (int xi = ceil(left); xi < ceil(right) - 1; xi++)
			{
				SetPixel(hdc, xi, yi, RGB(r, g, b));
			}

			xs += dx_left;
			xe += dx_right;
		}
	}
}

//描绘平底三角形
void ShaderFlatTriBottom(float x1, float y1, float x2, float y2, float x3, float y3, int color, HDC hdc)
{
	float dx_left, dx_right, xs, xe, left, right;
	float temp_x;
	int ys, ye;

	//裁剪范围
	int min_clip_x = 0, min_clip_y = 0;
	int max_clip_x = WINDOW_WIDTH - 1, max_clip_y = WINDOW_HEIGHT - 1;
	//颜色
	int r = 0, g = 0, b = 0;
	_RGB565FROM16BIT(color, &r, &g, &b);
	r << 3; g << 2; b << 3;

	//保证顶点顺序
	if (x3 < x2)
		SWAP(x3, x2, temp_x);

	dx_left = (x2 - x1) / (y2 - y1);
	dx_right = (x3 - x1) / (y2 - y1);
	xs = x1; xe = x1;
	//垂直裁剪
	if (y1 < min_clip_y)
	{
		xs = xs + (min_clip_y - y1)*dx_left;
		xe = xe + (min_clip_y - y1)*dx_right;

		ys = min_clip_y;
	}
	else
	{
		ys = ceil(y1);

		xs = xs + (ys - y1)*dx_left;
		xe = xe + (ys - y1)*dx_right;
	}

	if (y2 > max_clip_y)
	{
		ye = max_clip_y - 1;
	}
	else
	{
		ye = ceil(y2) - 1;
	}
	//水平裁剪
	if (x1 > min_clip_x && x1 < max_clip_x && x2 > min_clip_x
		&& x2 < max_clip_x && x3 > min_clip_x && x3 < max_clip_x)
	{
		for (int yi = ys; yi <= ye; yi++)
		{
			for (int xi = ceil(xs); xi < ceil(xe) - 1; xi++)
			{
				SetPixel(hdc, xi, yi, RGB(r, g, b));
			}

			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (int yi = ys; yi <= ye; yi++)
		{
			left = xs;
			right = xe;

			if (left < min_clip_x)
			{
				left = min_clip_x;
				if (right < min_clip_x)
					continue;
			}
			if (right > max_clip_x)
			{
				right = max_clip_x;
				if (left > max_clip_x)
					continue;
			}
			for (int xi = ceil(left); xi < ceil(right) - 1; xi++)
			{
				SetPixel(hdc, xi, yi, RGB(r, g, b));
			}

			xs += dx_left;
			xe += dx_right;
		}
	}
}

//恒定着色
void ShaderFlat(POLY4DV2_PTR face, int bLight, HDC hdc)
{
	float temp_x, temp_y, new_x;

	//取顶点坐标
	float x[3] = { 0 };
	float y[3] = { 0 };
	for (int ver = 0; ver < 3; ver++)
	{
		x[ver] = face->vlist[face->vert[ver]].x;
		y[ver] = face->vlist[face->vert[ver]].y;
	}
	//裁剪范围
	int min_clip_x = 0, min_clip_y = 0;
	int max_clip_x = WINDOW_WIDTH - 1, max_clip_y = WINDOW_HEIGHT - 1;
	//
	//判断三角形是否退化为一条直线
	if (((FCMP(x[0], x[1])) && (FCMP(x[1], x[2]))) ||
		((FCMP(x[0], x[1])) && (FCMP(x[1], x[2]))))
		return;
	//取颜色
	int r_base = 0, g_base = 0, b_base = 0;
	_RGB565FROM16BIT(face->color, &r_base, &g_base, &b_base);
	r_base << 3; g_base << 2; b_base << 3;
	//是否光照调制
	if (bLight)
	{
		int ri = 0, gi = 0, bi = 0;
		_RGB565FROM16BIT(face->lit_color[0], &ri, &bi, &gi);
		ri << 3; gi << 2; bi << 3;
		r_base = (r_base * ri) / 256;
		g_base = (g_base * gi) / 256;
		b_base = (b_base * bi) / 256;
	}
	int color = RGB16BIT565(r_base, g_base, b_base);

	//将顶点进行排序保持y1<y2<y3
	if (y[1] < y[0])
	{
		SWAP(y[1], y[0], temp_y);
		SWAP(x[1], x[0], temp_x);
	}
	if (y[2] < y[0])
	{
		SWAP(y[2], y[0], temp_y);
		SWAP(x[2], x[0], temp_x);
	}
	if (y[2] < y[1])
	{
		SWAP(y[2], y[1], temp_y);
		SWAP(x[2], x[1], temp_x);
	}
	//裁剪判断三角形是否在屏幕内
	if (y[2] < min_clip_y || y[0] > max_clip_y ||
		(x[0] < min_clip_x && x[1] < min_clip_x && x[2] < min_clip_x) ||
		(x[0] > max_clip_x && x[1] > max_clip_x && x[2] > max_clip_x))
		return;
	//判断三角形类型
	//平顶三角形
	if (FCMP(y[0], y[1]))
	{
		ShaderFlatTriTop(x[0], y[0], x[1], y[1], x[2], y[2], color, hdc);
	}
	//平底三角形
	else if (FCMP(y[1], y[2]))
	{
		ShaderFlatTriBottom(x[0], y[0], x[1], y[1], x[2], y[2], color, hdc);
	}
	//一般三角形
	else
	{
		new_x = x[0] + (y[1] - y[0]) * (x[2] - x[0]) / (y[2] - y[0]);
		ShaderFlatTriBottom(x[0], y[0], new_x, y[1], x[1], y[1], color, hdc);
		ShaderFlatTriTop(x[1], y[1], new_x, y[1], x[2], y[2], color, hdc);
	}
}
