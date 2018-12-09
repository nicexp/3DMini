/*恒定着色，gouraud着色*/
#include "3DShader.h"
#include "3DLib2.h"

#define TRI_LHS	0 //左转折
#define TRI_RHS 1 //右转折

#define TRIANGLE_TOP  0 //平顶三角形
#define TRIANGLE_BOTTOM 1 //平底三角形
#define TRIANGLE_GENERAL 2 //一般三角形

//描绘平顶三角形
void ShaderFlatTriTop(float x1, float y1, float x2, float y2, float x3, float y3, int color, UCHAR *_dest_buffer, int mempitch)
{
	float dx_left, dx_right, xs, xe, left, right;
	float temp_x;
	int ys, ye;

#ifdef WINDDOW_BPP32
	UINT *dest_buffer = (UINT *)_dest_buffer;
	UINT  *dest_addr = NULL;
	mempitch = (mempitch >> 2);//mempitch是字节数量，需要转换为32位
#else

	USHORT *dest_buffer = (USHORT *)_dest_buffer;
	USHORT  *dest_addr = NULL;
	mempitch = (mempitch >> 1);//mempitch是字节数量，需要转换为16位
#endif
	//颜色
	int r = 0, g = 0, b = 0;
	_RGBFROMINT(color, &r, &g, &b);

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

	dest_addr = dest_buffer + ys * mempitch;
	//水平裁剪
	if (x1 > min_clip_x && x1 < max_clip_x && x2 > min_clip_x
		&& x2 < max_clip_x && x3 > min_clip_x && x3 < max_clip_x)
	{
		for (int yi = ys; yi <= ye; yi++,dest_addr += mempitch)
		{
#ifdef WINDDOW_BPP32
			Mem_Set_QUAD(dest_addr + (unsigned int)(xs), color, ((unsigned int)(xe)-(unsigned int)(xs)+1));
#else
			Mem_Set_WORD(dest_addr + (unsigned int)(xs), color, ((unsigned int)(xe)-(unsigned int)(xs)+1));
#endif
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
		{
			left = xs; right = xe;

			xs += dx_left;
			xe += dx_right;

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

#ifdef WINDDOW_BPP32
			Mem_Set_QUAD(dest_addr + (unsigned int)(left), color, ((unsigned int)(right)-(unsigned int)(left)+1));
#else
			Mem_Set_WORD(dest_addr + (unsigned int)(left), color, ((unsigned int)(right)-(unsigned int)(left)+1));
#endif
		}
	}
}

//描绘平底三角形
void ShaderFlatTriBottom(float x1, float y1, float x2, float y2, float x3, float y3, int color, UCHAR *_dest_buffer, int mempitch)
{
	float dx_left, dx_right, xs, xe, left, right;
	float temp_x;
	int ys, ye;

#ifdef WINDDOW_BPP32
	UINT *dest_buffer = (UINT *)_dest_buffer;
	UINT  *dest_addr = NULL;
	mempitch = (mempitch >> 2);//mempitch是字节数量，需要转换为32位
#else

	USHORT *dest_buffer = (USHORT *)_dest_buffer;
	USHORT  *dest_addr = NULL;
	mempitch = (mempitch >> 1);//mempitch是字节数量，需要转换为16位
#endif

	//颜色
	int r = 0, g = 0, b = 0;
	_RGBFROMINT(color, &r, &g, &b);

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
	dest_addr = dest_buffer + ys * mempitch;
	//水平裁剪
	if (x1 > min_clip_x && x1 < max_clip_x && x2 > min_clip_x
		&& x2 < max_clip_x && x3 > min_clip_x && x3 < max_clip_x)
	{
		for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
		{
#ifdef WINDDOW_BPP32
			Mem_Set_QUAD(dest_addr + (unsigned int)(xs), color, ((unsigned int)(xe)-(unsigned int)(xs)+1));
#else
			Mem_Set_WORD(dest_addr + (unsigned int)(xs), color, ((unsigned int)(xe)-(unsigned int)(xs)+1));
#endif
			xs += dx_left;
			xe += dx_right;
		}
	}
	else
	{
		for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
		{
			left = xs;
			right = xe;

			xs += dx_left;
			xe += dx_right;

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

#ifdef WINDDOW_BPP32
			Mem_Set_QUAD(dest_addr + (unsigned int)(left), color, ((unsigned int)(right)-(unsigned int)(left)+1));
#else
			Mem_Set_WORD(dest_addr + (unsigned int)(left), color, ((unsigned int)(right)-(unsigned int)(left)+1));
#endif
		}
	}
}

//恒定着色
void ShaderFlat(POLYF4DV2_PTR face, UCHAR *_dest_buffer, int mempitch)
{
	float temp_x, temp_y, new_x;

	//取顶点坐标
	float x[3] = { 0 };
	float y[3] = { 0 };
	for (int ver = 0; ver < 3; ver++)
	{
		x[ver] = face->tvlist[ver].x;
		y[ver] = face->tvlist[ver].y;
	}

	//判断三角形是否退化为一条直线
	if (((FCMP(x[0], x[1])) && (FCMP(x[1], x[2]))) ||
		((FCMP(x[0], x[1])) && (FCMP(x[1], x[2]))))
		return;
	//取颜色
	int r_base = 0, g_base = 0, b_base = 0;
	_RGBFROMINT(face->color, &r_base, &g_base, &b_base);
	//光照
	int ri = 0, gi = 0, bi = 0;
	_RGBFROMINT(face->lit_color[0], &ri, &gi, &bi);
	r_base = (r_base * ri) / 256;
	g_base = (g_base * gi) / 256;
	b_base = (b_base * bi) / 256;
	int color = _RGBTOINT(r_base, g_base, b_base);

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
		ShaderFlatTriTop(x[0], y[0], x[1], y[1], x[2], y[2], color, _dest_buffer, mempitch);
	}
	//平底三角形
	else if (FCMP(y[1], y[2]))
	{
		ShaderFlatTriBottom(x[0], y[0], x[1], y[1], x[2], y[2], color, _dest_buffer, mempitch);
	}
	//一般三角形
	else
	{
		new_x = x[0] + (y[1] - y[0]) * (x[2] - x[0]) / (y[2] - y[0]);
		ShaderFlatTriBottom(x[0], y[0], new_x, y[1], x[1], y[1], color, _dest_buffer, mempitch);
		ShaderFlatTriTop(x[1], y[1], new_x, y[1], x[2], y[2], color, _dest_buffer, mempitch);
	}
}

//高洛德着色
void ShaderGouraud(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch)
{
	int ver0 = 0, ver1 = 1, ver2 = 2, temp;

	int irestart = TRI_LHS;
	int tri_type = 0;

#ifdef WINDDOW_BPP32
	unsigned int* dest_buffer = (unsigned int*)_dest_buffer;
	unsigned int* dest_addr = NULL;
	mempitch >>= 2;
#else
	unsigned short* dest_buffer = (unsigned short*)_dest_buffer;
	unsigned short* dest_addr = NULL;
	mempitch >>= 1;
#endif

	//顶点颜色
	int r_base0 = 0, g_base0 = 0, b_base0 = 0;
	int r_base1 = 0, g_base1 = 0, b_base1 = 0;
	int r_base2 = 0, g_base2 = 0, b_base2 = 0;
	//光照颜色
	int ri0 = 0, gi0 = 0, bi0 = 0;
	int ri1 = 0, gi1 = 0, bi1 = 0;
	int ri2 = 0, gi2 = 0, bi2 = 0;

	//裁剪不在屏幕内的三角形
	if (((face->tvlist[ver0].y < min_clip_y) &&
		(face->tvlist[ver1].y < min_clip_y) &&
		(face->tvlist[ver2].y < min_clip_y)) ||
		((face->tvlist[ver0].y > max_clip_y) &&
		(face->tvlist[ver1].y > max_clip_y) &&
		(face->tvlist[ver2].y > max_clip_y)) ||
		((face->tvlist[ver0].x < min_clip_x) &&
		(face->tvlist[ver1].x < min_clip_x) &&
		(face->tvlist[ver2].x < min_clip_x)) ||
		((face->tvlist[ver0].x > max_clip_x) &&
		(face->tvlist[ver1].x > max_clip_x) &&
		(face->tvlist[ver2].x > max_clip_x)))
		return;
	//三角形退化成直线
	if (((int)(face->tvlist[ver0].y + 0.5) == (int)(face->tvlist[ver1].y + 0.5)) &&
		((int)(face->tvlist[ver1].y + 0.5) == (int)(face->tvlist[ver2].y + 0.5)) ||
		((int)(face->tvlist[ver0].x + 0.5) == (int)(face->tvlist[ver1].x + 0.5)) &&
		((int)(face->tvlist[ver1].x + 0.5) == (int)(face->tvlist[ver2].x + 0.5)))
		return;
	//调整三角形顶点顺序v0到v2从上到下
	if (face->tvlist[ver1].y < face->tvlist[ver0].y)
	{
		SWAP(ver0, ver1, temp);
	}
	if (face->tvlist[ver2].y < face->tvlist[ver0].y)
	{
		SWAP(ver0, ver2, temp);
	}
	if (face->tvlist[ver2].y < face->tvlist[ver1].y)
	{
		SWAP(ver1, ver2, temp);
	}
	//判断三角形类型
	if ((int)(face->tvlist[ver0].y + 0.5) == (int)(face->tvlist[ver1].y + 0.5))
	{
		tri_type = TRIANGLE_TOP;
		//顶点排序
		if (face->tvlist[ver1].x < face->tvlist[ver0].x)
		{
			SWAP(ver0, ver1, temp);
		}
	}
	//平底三角形
	else if ((int)(face->tvlist[ver1].y + 0.5) == (int)(face->tvlist[ver2].y + 0.5))
	{
		tri_type = TRIANGLE_BOTTOM;
		//顶点排序
		if (face->tvlist[ver2].x < face->tvlist[ver1].x)
		{
			SWAP(ver1, ver2, temp);
		}
	}
	else
	{
		tri_type = TRIANGLE_GENERAL;
	}

	//取顶点颜色
	_RGBFROMINT(face->lit_color[ver0], &r_base0, &g_base0, &b_base0);
	_RGBFROMINT(face->lit_color[ver1], &r_base1, &g_base1, &b_base1);
	_RGBFROMINT(face->lit_color[ver2], &r_base2, &g_base2, &b_base2);
	//顶点变量与颜色
	int x0, y0, x1, y1, x2, y2;
	int u0, u1, u2, v0, v1, v2, w0, w1, w2;

	x0 = int(face->tvlist[ver0].x + 0.5);
	y0 = int(face->tvlist[ver0].y + 0.5);
	u0 = r_base0; v0 = g_base0; w0 = b_base0;
	x1 = int(face->tvlist[ver1].x + 0.5);
	y1 = int(face->tvlist[ver1].y + 0.5);
	u1 = r_base1; v1 = g_base1; w1 = b_base1;
	x2 = int(face->tvlist[ver2].x + 0.5);
	y2 = int(face->tvlist[ver2].y + 0.5);
	u2 = r_base2; v2 = g_base2; w2 = b_base2;
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dudyl, dvdyl, dwdyl, dxdyr, dudyr, dvdyr, dwdyr;
	int xl, ul, vl, wl, xr, ur, vr, wr;
	int ui, vi, wi;
	int dudx, dvdx, dwdx;
	int dyl, dyr;
	
	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) /dy;
			dudyl = ((u2 - u0) << FIXP16_SHIFT) /dy;
			dvdyl = ((v2 - v0) << FIXP16_SHIFT) /dy;
			dwdyl = ((w2 - w0) << FIXP16_SHIFT) /dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) /dy;
			dudyr = ((u2 - u1) << FIXP16_SHIFT) /dy;
			dvdyr = ((v2 - v1) << FIXP16_SHIFT) /dy;
			dwdyr = ((w2 - w1) << FIXP16_SHIFT) /dy;

			if(y0 < min_clip_y) //垂直裁剪
			{	
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				ul = (u0 << FIXP16_SHIFT) + (dy * dudyl);
				vl = (v0 << FIXP16_SHIFT) + (dy * dvdyl);
				wl = (w0 << FIXP16_SHIFT) + (dy * dwdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				ur = (u1 << FIXP16_SHIFT) + (dy * dudyr);
				vr = (v1 << FIXP16_SHIFT) + (dy * dvdyr);
				wr = (w1 << FIXP16_SHIFT) + (dy * dwdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				ul = (u0 << FIXP16_SHIFT);
				vl = (v0 << FIXP16_SHIFT);
				wl = (w0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);
				ur = (u1 << FIXP16_SHIFT);
				vr = (v1 << FIXP16_SHIFT);
				wr = (w1 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		if (tri_type == TRIANGLE_BOTTOM)
		{
			dy = y2 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dy;
			dudyl = ((u1 - u0) << FIXP16_SHIFT) / dy;
			dvdyl = ((v1 - v0) << FIXP16_SHIFT) / dy;
			dwdyl = ((w1 - w0) << FIXP16_SHIFT) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dy;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dy;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				ul = (u0 << FIXP16_SHIFT) + (dudyl * dy);
				vl = (v0 << FIXP16_SHIFT) + (dvdyl * dy);
				wl = (w0 << FIXP16_SHIFT) + (dwdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				ur = (u0 << FIXP16_SHIFT) + (dudyr * dy);
				vr = (v0 << FIXP16_SHIFT) + (dvdyr * dy);
				wr = (w0 << FIXP16_SHIFT) + (dwdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				ul = (u0 << FIXP16_SHIFT);
				vl = (v0 << FIXP16_SHIFT);
				wl = (w0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);
				ur = (u0 << FIXP16_SHIFT);
				vr = (v0 << FIXP16_SHIFT);
				wr = (w0 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;

		//水平裁剪
		if((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for(int yi = ys; yi <= ye; yi ++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul + FIXP16_ROUND_UP);
				vi = (vl + FIXP16_ROUND_UP);
				wi = (wl + FIXP16_ROUND_UP);

				dx = (xe - xs);
				if(dx > 0)
				{
					dudx = (ur - ul) /dx;
					dvdx = (vr - vl) /dx;
					dwdx = (wr - wl) /dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
				}

				if(xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ui += dudx * dx;
					vi += dvdx * dx;
					wi += dwdx * dx;

					xs = min_clip_x;
				}

				if(xe > max_clip_x)
					xe = max_clip_x;

				for(int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = (unsigned int)(((ui >> (FIXP16_SHIFT)) << 16) + ((vi >> (FIXP16_SHIFT)) << 8) + ((wi >> (FIXP16_SHIFT))) + 0xff000000);
#else
					*(dest_addr + xi) = (unsigned short)(((ui >> (FIXP16_SHIFT + 3)) << 11) + ((vi >> (FIXP16_SHIFT + 2)) << 5) + (wi >> (FIXP16_SHIFT + 3)));
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
			}
		}
		else
		{
			for(int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;

				dx = xe - xs;
				if(dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
				}

				for(int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = (unsigned int)(((ui >> (FIXP16_SHIFT)) << 16) + ((vi >> (FIXP16_SHIFT)) << 8) + ((wi >> (FIXP16_SHIFT))) + 0xff000000);
#else
					*(dest_addr + xi) = (unsigned short)(((ui >> (FIXP16_SHIFT + 3)) << 11) + ((vi >> (FIXP16_SHIFT + 2)) << 5) + (wi >> (FIXP16_SHIFT + 3)));
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
			}
		}
	}
	else
	{
		ye = y2;
		if(ye > max_clip_y)
			ye = max_clip_y;

		if(y1 < min_clip_y) //垂直裁剪
		{
			dyl = y2 - y1;

			dxdyl = ((x2 - x1) << FIXP16_SHIFT)/ dyl;
			dudyl = ((u2 - u1) << FIXP16_SHIFT)/ dyl;
			dvdyl = ((v2 - v1) << FIXP16_SHIFT)/ dyl;
			dwdyl = ((w2 - w1) << FIXP16_SHIFT)/ dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT)/ dyr;
			dudyr = ((u2 - u0) << FIXP16_SHIFT)/ dyr;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT)/ dyr;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT)/ dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			ul = (u1 << FIXP16_SHIFT) + dyl*dudyl;
			vl = (v1 << FIXP16_SHIFT) + dyl*dvdyl;
			wl = (w1 << FIXP16_SHIFT) + dyl*dwdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			ur = (u0 << FIXP16_SHIFT) + dyr*dudyr;
			vr = (v0 << FIXP16_SHIFT) + dyr*dvdyr;
			wr = (w0 << FIXP16_SHIFT) + dyr*dwdyr;

			if(dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if(y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dudyl = ((u1 - u0) << FIXP16_SHIFT) / dyl;
			dvdyl = ((v1 - v0) << FIXP16_SHIFT) / dyl;
			dwdyl = ((w1 - w0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dyr;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dyr;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			ul = (u0 << FIXP16_SHIFT) + dy * dudyl;
			vl = (v0 << FIXP16_SHIFT) + dy * dvdyl;
			wl = (w0 << FIXP16_SHIFT) + dy * dwdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			ur = (u0 << FIXP16_SHIFT) + dy * dudyr;
			vr = (v0 << FIXP16_SHIFT) + dy * dvdyr;
			wr = (w0 << FIXP16_SHIFT) + dy * dwdyr;

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dudyl = ((u1 - u0) << FIXP16_SHIFT) / dyl;
			dvdyl = ((v1 - v0) << FIXP16_SHIFT) / dyl;
			dwdyl = ((w1 - w0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dyr;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dyr;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			ul = (u0 << FIXP16_SHIFT);
			vl = (v0 << FIXP16_SHIFT);
			wl = (w0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);
			ur = (u0 << FIXP16_SHIFT);
			vr = (v0 << FIXP16_SHIFT);
			wr = (w0 << FIXP16_SHIFT);

			if(dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;

		//水平裁剪
		if((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for(int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul + FIXP16_ROUND_UP);
				vi = (vl + FIXP16_ROUND_UP);
				wi = (wl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if(dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
				}

				if(xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ui += dudx * dx;
					vi += dvdx * dx;
					wi += dwdx * dx;

					xs = min_clip_x;
				}	

				if(xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for(int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = (unsigned int)(((ui >> (FIXP16_SHIFT)) << 16) + ((vi >> (FIXP16_SHIFT)) << 8) + ((wi >> (FIXP16_SHIFT))) + 0xff000000);
#else
					*(dest_addr + xi) = (unsigned short)(((ui >> (FIXP16_SHIFT + 3)) << 11) + ((vi >> (FIXP16_SHIFT + 2)) << 5) + (wi >> (FIXP16_SHIFT + 3)));
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				if(yi == yrestart)
				{
					if(irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((u2 - u1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((v2 - v1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((w2 - w1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (u1 << FIXP16_SHIFT);
						vl = (v1 << FIXP16_SHIFT);
						wl = (w1 << FIXP16_SHIFT);

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dyr;
						dudyr = ((u2 - u1) << FIXP16_SHIFT)/dyr;
						dvdyr = ((v2 - v1) << FIXP16_SHIFT)/dyr;
						dwdyr = ((w2 - w1) << FIXP16_SHIFT)/dyr;

						xr = (x1 << FIXP16_SHIFT);
						ur = (u1 << FIXP16_SHIFT);
						vr = (v1 << FIXP16_SHIFT);
						wr = (w1 << FIXP16_SHIFT);

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
					}
				}
			}
		}
		else
		{
			for(int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul + FIXP16_ROUND_UP);
				vi = (vl + FIXP16_ROUND_UP);
				wi = (wl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if(dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
				}

				for(int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = (unsigned int)(((ui >> (FIXP16_SHIFT)) << 16) + ((vi >> (FIXP16_SHIFT)) << 8) + ((wi >> (FIXP16_SHIFT))) + 0xff000000);
#else
					*(dest_addr + xi) = (unsigned short)(((ui >> (FIXP16_SHIFT + 3)) << 11) + ((vi >> (FIXP16_SHIFT + 2)) << 5) + (wi >> (FIXP16_SHIFT + 3)));
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;

				if(yi == yrestart)
				{
					if(irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT)/dyl;
						dudyl = ((u2 - u1) << FIXP16_SHIFT)/dyl;
						dvdyl = ((v2 - v1) << FIXP16_SHIFT)/dyl;
						dwdyl = ((w2 - w1) << FIXP16_SHIFT)/dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (u1 << FIXP16_SHIFT);
						vl = (v1 << FIXP16_SHIFT);
						wl = (w1 << FIXP16_SHIFT);

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT)/dyr;
						dudyr = ((u2 - u1) << FIXP16_SHIFT)/dyr;
						dvdyr = ((v2 - v1) << FIXP16_SHIFT)/dyr;
						dwdyr = ((w2 - w1) << FIXP16_SHIFT)/dyr;

						xr = (x1 << FIXP16_SHIFT);
						ur = (u1 << FIXP16_SHIFT);
						vr = (v1 << FIXP16_SHIFT);
						wr = (w1 << FIXP16_SHIFT);

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
					}
				}
				
			}
		}
	}
}