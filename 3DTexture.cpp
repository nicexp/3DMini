/*仿射纹理映射，包括固定着色，恒定着色，gouraud着色*/
#include "3DTexture.h"

#define TRI_LHS	0 //左转折
#define TRI_RHS 1 //右转折

#define TRIANGLE_TOP  0 //平顶三角形
#define TRIANGLE_BOTTOM 1 //平底三角形
#define TRIANGLE_GENERAL 2 //一般三角形

void DrawTextureConstant(POLYF4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch)
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

	//顶点变量与颜色
	int x0, y0, x1, y1, x2, y2;//顶点坐标
	int t0, s0, t1, s1, t2, s2;//纹理坐标

	x0 = int(face->tvlist[ver0].x + 0.5);
	y0 = int(face->tvlist[ver0].y + 0.5);
	t0 = int(face->tvlist[ver0].u0);
	s0 = int(face->tvlist[ver0].v0);
	x1 = int(face->tvlist[ver1].x + 0.5);
	y1 = int(face->tvlist[ver1].y + 0.5);
	t1 = int(face->tvlist[ver1].u0);
	s1 = int(face->tvlist[ver1].v0);
	x2 = int(face->tvlist[ver2].x + 0.5);
	y2 = int(face->tvlist[ver2].y + 0.5);
	t2 = int(face->tvlist[ver2].u0);
	s2 = int(face->tvlist[ver2].v0);
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dxdyr;
	int dtdyl, dsdyl, dtdyr, dsdyr;
	int xl, xr;
	int tl, sl, tr, sr;
	int ti, si;
	int dtdx, dsdx;
	int dyl, dyr;

	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t2 - t0) << FIXP16_SHIFT) / dy;
			dsdyl = ((s2 - s0) << FIXP16_SHIFT) / dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dy;
			dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				tl = (t0 << FIXP16_SHIFT) + (dy * dtdyl);
				sl = (s0 << FIXP16_SHIFT) + (dy * dsdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				tr = (t1 << FIXP16_SHIFT) + (dy * dtdyr);
				sr = (s1 << FIXP16_SHIFT) + (dy * dsdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0 << FIXP16_SHIFT);
				sl = (s0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);
				tr = (t1 << FIXP16_SHIFT);
				sr = (s1 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		if (tri_type == TRIANGLE_BOTTOM)
		{
			dy = y2 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dy;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dy;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				tl = (t0 << FIXP16_SHIFT) + (dtdyl * dy);
				sl = (s0 << FIXP16_SHIFT) + (dsdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				tr = (t0 << FIXP16_SHIFT) + (dtdyr * dy);
				sr = (s0 << FIXP16_SHIFT) + (dsdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0 << FIXP16_SHIFT);
				sl = (s0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);
				tr = (t0 << FIXP16_SHIFT);
				sr = (s0 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = (xe - xs);
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ti += dtdx * dx;
					si += dsdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
					xe = max_clip_x;

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
			}
		}
		else
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = tl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
			}
		}
	}
	else
	{
		ye = y2;
		if (ye > max_clip_y)
			ye = max_clip_y;

		if (y1 < min_clip_y) //垂直裁剪
		{
			dyl = y2 - y1;

			dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			tl = (t1 << FIXP16_SHIFT) + dyl*dtdyl;
			sl = (s1 << FIXP16_SHIFT) + dyl*dsdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			tr = (t0 << FIXP16_SHIFT) + dyr*dtdyr;
			sr = (s0 << FIXP16_SHIFT) + dyr*dsdyr;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if (y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			tl = (t0 << FIXP16_SHIFT) + dy * dtdyl;
			sl = (s0 << FIXP16_SHIFT) + dy * dsdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			tr = (t0 << FIXP16_SHIFT) + dy * dtdyr;
			sr = (s0 << FIXP16_SHIFT) + dy * dsdyr;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			tl = (t0 << FIXP16_SHIFT);
			sl = (s0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);
			tr = (t0 << FIXP16_SHIFT);
			sr = (s0 << FIXP16_SHIFT);

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = tr - tl;
					dsdx = sr - sl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ti += dtdx * dx;
					si += dsdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
						dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1 << FIXP16_SHIFT);
						sl = (s1 << FIXP16_SHIFT);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dyr;
						dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1 << FIXP16_SHIFT);
						sr = (s1 << FIXP16_SHIFT);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
					}
				}
			}
		}
		else
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
						dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1 << FIXP16_SHIFT);
						sl = (s1 << FIXP16_SHIFT);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dyr;
						dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1 << FIXP16_SHIFT);
						sr = (s1 << FIXP16_SHIFT);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
					}
				}

			}
		}
	}
}

void DrawTextureGouraud(POLYF4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch)
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

	//顶点光照颜色
	int r_base0 = 0, g_base0 = 0, b_base0 = 0;
	int r_base1 = 0, g_base1 = 0, b_base1 = 0;
	int r_base2 = 0, g_base2 = 0, b_base2 = 0;
	//纹理颜色
	int ri = 0, gi = 0, bi = 0, textel = 0;

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
	int t0, s0, t1, s1, t2, s2;//纹理坐标

	x0 = int(face->tvlist[ver0].x + 0.5);
	y0 = int(face->tvlist[ver0].y + 0.5);
	u0 = r_base0; v0 = g_base0; w0 = b_base0;
	t0 = int(face->tvlist[ver0].u0);
	s0 = int(face->tvlist[ver0].v0);

	x1 = int(face->tvlist[ver1].x + 0.5);
	y1 = int(face->tvlist[ver1].y + 0.5);
	u1 = r_base1; v1 = g_base1; w1 = b_base1;
	t1 = int(face->tvlist[ver1].u0);
	s1 = int(face->tvlist[ver1].v0);

	x2 = int(face->tvlist[ver2].x + 0.5);
	y2 = int(face->tvlist[ver2].y + 0.5);
	u2 = r_base2; v2 = g_base2; w2 = b_base2;
	t2 = int(face->tvlist[ver2].u0);
	s2 = int(face->tvlist[ver2].v0);
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dudyl, dvdyl, dwdyl, dxdyr, dudyr, dvdyr, dwdyr;
	int dtdyl, dsdyl, dtdyr, dsdyr;
	int xl, ul, vl, wl, xr, ur, vr, wr;
	int tl, sl, tr, sr;
	int ui, vi, wi;
	int ti, si;
	int dudx, dvdx, dwdx;
	int dtdx, dsdx;
	int dyl, dyr;

	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dudyl = ((u2 - u0) << FIXP16_SHIFT) / dy;
			dvdyl = ((v2 - v0) << FIXP16_SHIFT) / dy;
			dwdyl = ((w2 - w0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t2 - t0) << FIXP16_SHIFT) / dy;
			dsdyl = ((s2 - s0) << FIXP16_SHIFT) / dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dy;
			dudyr = ((u2 - u1) << FIXP16_SHIFT) / dy;
			dvdyr = ((v2 - v1) << FIXP16_SHIFT) / dy;
			dwdyr = ((w2 - w1) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dy;
			dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				ul = (u0 << FIXP16_SHIFT) + (dy * dudyl);
				vl = (v0 << FIXP16_SHIFT) + (dy * dvdyl);
				wl = (w0 << FIXP16_SHIFT) + (dy * dwdyl);
				tl = (t0 << FIXP16_SHIFT) + (dy * dtdyl);
				sl = (s0 << FIXP16_SHIFT) + (dy * dsdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				ur = (u1 << FIXP16_SHIFT) + (dy * dudyr);
				vr = (v1 << FIXP16_SHIFT) + (dy * dvdyr);
				wr = (w1 << FIXP16_SHIFT) + (dy * dwdyr);
				tr = (t1 << FIXP16_SHIFT) + (dy * dtdyr);
				sr = (s1 << FIXP16_SHIFT) + (dy * dsdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				ul = (u0 << FIXP16_SHIFT);
				vl = (v0 << FIXP16_SHIFT);
				wl = (w0 << FIXP16_SHIFT);
				tl = (t0 << FIXP16_SHIFT);
				sl = (s0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);
				ur = (u1 << FIXP16_SHIFT);
				vr = (v1 << FIXP16_SHIFT);
				wr = (w1 << FIXP16_SHIFT);
				tr = (t1 << FIXP16_SHIFT);
				sr = (s1 << FIXP16_SHIFT);

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
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dy;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dy;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dy;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dy;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				ul = (u0 << FIXP16_SHIFT) + (dudyl * dy);
				vl = (v0 << FIXP16_SHIFT) + (dvdyl * dy);
				wl = (w0 << FIXP16_SHIFT) + (dwdyl * dy);
				tl = (t0 << FIXP16_SHIFT) + (dtdyl * dy);
				sl = (s0 << FIXP16_SHIFT) + (dsdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				ur = (u0 << FIXP16_SHIFT) + (dudyr * dy);
				vr = (v0 << FIXP16_SHIFT) + (dvdyr * dy);
				wr = (w0 << FIXP16_SHIFT) + (dwdyr * dy);
				tr = (t0 << FIXP16_SHIFT) + (dtdyr * dy);
				sr = (s0 << FIXP16_SHIFT) + (dsdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				ul = (u0 << FIXP16_SHIFT);
				vl = (v0 << FIXP16_SHIFT);
				wl = (w0 << FIXP16_SHIFT);
				tl = (t0 << FIXP16_SHIFT);
				sl = (s0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);
				ur = (u0 << FIXP16_SHIFT);
				vr = (v0 << FIXP16_SHIFT);
				wr = (w0 << FIXP16_SHIFT);
				tr = (t0 << FIXP16_SHIFT);
				sr = (s0 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul + FIXP16_ROUND_UP);
				vi = (vl + FIXP16_ROUND_UP);
				wi = (wl + FIXP16_ROUND_UP);
				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = (xe - xs);
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ui += dudx * dx;
					vi += dvdx * dx;
					wi += dwdx * dx;
					ti += dtdx * dx;
					si += dsdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
					xe = max_clip_x;

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textel, &ri, &gi, &bi);
					ri *= ui;
					gi *= vi;
					bi *= wi;
					textel = _RGBTOINT((ri >> (FIXP16_SHIFT + 8)), (gi >> (FIXP16_SHIFT + 8)), (bi >> (FIXP16_SHIFT + 8)));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textel;
#else
					*(dest_addr + xi) = (USHORT)textel;
#endif


					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;
			}
		}
		else
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;

				dx = xe - xs;
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textel, &ri, &gi, &bi);
					ri *= ui;
					gi *= vi;
					bi *= wi;
					textel = _RGBTOINT((ri >> (FIXP16_SHIFT + 8)), (gi >> (FIXP16_SHIFT + 8)), (bi >> (FIXP16_SHIFT + 8)));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textel;
#else
					*(dest_addr + xi) = (USHORT)textel;
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;
			}
		}
	}
	else
	{
		ye = y2;
		if (ye > max_clip_y)
			ye = max_clip_y;

		if (y1 < min_clip_y) //垂直裁剪
		{
			dyl = y2 - y1;

			dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
			dudyl = ((u2 - u1) << FIXP16_SHIFT) / dyl;
			dvdyl = ((v2 - v1) << FIXP16_SHIFT) / dyl;
			dwdyl = ((w2 - w1) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dyr;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dyr;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			ul = (u1 << FIXP16_SHIFT) + dyl*dudyl;
			vl = (v1 << FIXP16_SHIFT) + dyl*dvdyl;
			wl = (w1 << FIXP16_SHIFT) + dyl*dwdyl;
			tl = (t1 << FIXP16_SHIFT) + dyl*dtdyl;
			sl = (s1 << FIXP16_SHIFT) + dyl*dsdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			ur = (u0 << FIXP16_SHIFT) + dyr*dudyr;
			vr = (v0 << FIXP16_SHIFT) + dyr*dvdyr;
			wr = (w0 << FIXP16_SHIFT) + dyr*dwdyr;
			tr = (t0 << FIXP16_SHIFT) + dyr*dtdyr;
			sr = (s0 << FIXP16_SHIFT) + dyr*dsdyr;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if (y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dudyl = ((u1 - u0) << FIXP16_SHIFT) / dyl;
			dvdyl = ((v1 - v0) << FIXP16_SHIFT) / dyl;
			dwdyl = ((w1 - w0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dyr;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dyr;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			ul = (u0 << FIXP16_SHIFT) + dy * dudyl;
			vl = (v0 << FIXP16_SHIFT) + dy * dvdyl;
			wl = (w0 << FIXP16_SHIFT) + dy * dwdyl;
			tl = (t0 << FIXP16_SHIFT) + dy * dtdyl;
			sl = (s0 << FIXP16_SHIFT) + dy * dsdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			ur = (u0 << FIXP16_SHIFT) + dy * dudyr;
			vr = (v0 << FIXP16_SHIFT) + dy * dvdyr;
			wr = (w0 << FIXP16_SHIFT) + dy * dwdyr;
			tr = (t0 << FIXP16_SHIFT) + dy * dtdyr;
			sr = (s0 << FIXP16_SHIFT) + dy * dsdyr;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
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
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0) << FIXP16_SHIFT) / dyr;
			dvdyr = ((v2 - v0) << FIXP16_SHIFT) / dyr;
			dwdyr = ((w2 - w0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			ul = (u0 << FIXP16_SHIFT);
			vl = (v0 << FIXP16_SHIFT);
			wl = (w0 << FIXP16_SHIFT);
			tl = (t0 << FIXP16_SHIFT);
			sl = (s0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);
			ur = (u0 << FIXP16_SHIFT);
			vr = (v0 << FIXP16_SHIFT);
			wr = (w0 << FIXP16_SHIFT);
			tr = (t0 << FIXP16_SHIFT);
			sr = (s0 << FIXP16_SHIFT);

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul + FIXP16_ROUND_UP);
				vi = (vl + FIXP16_ROUND_UP);
				wi = (wl + FIXP16_ROUND_UP);
				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = tr - tl;
					dsdx = sr - sl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ui += dudx * dx;
					vi += dvdx * dx;
					wi += dwdx * dx;
					ti += dtdx * dx;
					si += dsdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textel, &ri, &gi, &bi);
					ri *= ui;
					gi *= vi;
					bi *= wi;
					textel = _RGBTOINT((ri >> (FIXP16_SHIFT + 8)), (gi >> (FIXP16_SHIFT + 8)), (bi >> (FIXP16_SHIFT + 8)));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textel;
#else
					*(dest_addr + xi) = (USHORT)textel;
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dudyl = ((u2 - u1) << FIXP16_SHIFT) / dyl;
						dvdyl = ((v2 - v1) << FIXP16_SHIFT) / dyl;
						dwdyl = ((w2 - w1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
						dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (u1 << FIXP16_SHIFT);
						vl = (v1 << FIXP16_SHIFT);
						wl = (w1 << FIXP16_SHIFT);
						tl = (t1 << FIXP16_SHIFT);
						sl = (s1 << FIXP16_SHIFT);

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
						tl += dtdyl;
						sl += dsdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dudyr = ((u2 - u1) << FIXP16_SHIFT) / dyr;
						dvdyr = ((v2 - v1) << FIXP16_SHIFT) / dyr;
						dwdyr = ((w2 - w1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dyr;
						dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						ur = (u1 << FIXP16_SHIFT);
						vr = (v1 << FIXP16_SHIFT);
						wr = (w1 << FIXP16_SHIFT);
						tr = (t1 << FIXP16_SHIFT);
						sr = (s1 << FIXP16_SHIFT);

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
						tr += dtdyr;
						sr += dsdyr;
					}
				}
			}
		}
		else
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul + FIXP16_ROUND_UP);
				vi = (vl + FIXP16_ROUND_UP);
				wi = (wl + FIXP16_ROUND_UP);
				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textel, &ri, &gi, &bi);
					ri *= ui;
					gi *= vi;
					bi *= wi;
					textel = _RGBTOINT((ri >> (FIXP16_SHIFT + 8)), (gi >> (FIXP16_SHIFT + 8)), (bi >> (FIXP16_SHIFT + 8)));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textel;
#else
					*(dest_addr + xi) = (USHORT)textel;
#endif
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dudyl = ((u2 - u1) << FIXP16_SHIFT) / dyl;
						dvdyl = ((v2 - v1) << FIXP16_SHIFT) / dyl;
						dwdyl = ((w2 - w1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
						dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (u1 << FIXP16_SHIFT);
						vl = (v1 << FIXP16_SHIFT);
						wl = (w1 << FIXP16_SHIFT);
						tl = (t1 << FIXP16_SHIFT);
						sl = (s1 << FIXP16_SHIFT);

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
						tl += dtdyl;
						sl += dsdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dudyr = ((u2 - u1) << FIXP16_SHIFT) / dyr;
						dvdyr = ((v2 - v1) << FIXP16_SHIFT) / dyr;
						dwdyr = ((w2 - w1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dyr;
						dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						ur = (u1 << FIXP16_SHIFT);
						vr = (v1 << FIXP16_SHIFT);
						wr = (w1 << FIXP16_SHIFT);
						tr = (t1 << FIXP16_SHIFT);
						sr = (s1 << FIXP16_SHIFT);

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
						tr += dtdyr;
						sr += dsdyr;
					}
				}

			}
		}
	}
}

void DrawTextureFlat(POLYF4DV2_PTR face, BITMAP_FILE_PTR bitmap, unsigned char *_dest_buffer, int mempitch)
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

	//光照颜色
	int ri = 0, gi = 0, bi = 0, textiel = 0;
	int rbase = 0, gbase = 0, bbase = 0;

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

	//顶点变量与颜色
	int x0, y0, x1, y1, x2, y2;//顶点坐标
	int t0, s0, t1, s1, t2, s2;//纹理坐标

	x0 = int(face->tvlist[ver0].x + 0.5);
	y0 = int(face->tvlist[ver0].y + 0.5);
	t0 = int(face->tvlist[ver0].u0);
	s0 = int(face->tvlist[ver0].v0);
	x1 = int(face->tvlist[ver1].x + 0.5);
	y1 = int(face->tvlist[ver1].y + 0.5);
	t1 = int(face->tvlist[ver1].u0);
	s1 = int(face->tvlist[ver1].v0);
	x2 = int(face->tvlist[ver2].x + 0.5);
	y2 = int(face->tvlist[ver2].y + 0.5);
	t2 = int(face->tvlist[ver2].u0);
	s2 = int(face->tvlist[ver2].v0);
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dxdyr;
	int dtdyl, dsdyl, dtdyr, dsdyr;
	int xl, xr;
	int tl, sl, tr, sr;
	int ti, si;
	int dtdx, dsdx;
	int dyl, dyr;

	//取顶点颜色
	_RGBFROMINT(face->lit_color[0], &ri, &gi, &bi);

	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t2 - t0) << FIXP16_SHIFT) / dy;
			dsdyl = ((s2 - s0) << FIXP16_SHIFT) / dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dy;
			dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				tl = (t0 << FIXP16_SHIFT) + (dy * dtdyl);
				sl = (s0 << FIXP16_SHIFT) + (dy * dsdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				tr = (t1 << FIXP16_SHIFT) + (dy * dtdyr);
				sr = (s1 << FIXP16_SHIFT) + (dy * dsdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0 << FIXP16_SHIFT);
				sl = (s0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);
				tr = (t1 << FIXP16_SHIFT);
				sr = (s1 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		if (tri_type == TRIANGLE_BOTTOM)
		{
			dy = y2 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dy;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dy;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				tl = (t0 << FIXP16_SHIFT) + (dtdyl * dy);
				sl = (s0 << FIXP16_SHIFT) + (dsdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				tr = (t0 << FIXP16_SHIFT) + (dtdyr * dy);
				sr = (s0 << FIXP16_SHIFT) + (dsdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0 << FIXP16_SHIFT);
				sl = (s0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);
				tr = (t0 << FIXP16_SHIFT);
				sr = (s0 << FIXP16_SHIFT);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = (xe - xs);
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ti += dtdx * dx;
					si += dsdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
					xe = max_clip_x;

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textiel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textiel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textiel, &rbase, &gbase, &bbase);
					rbase *= ri;
					gbase *= gi;
					bbase *= bi;
					textiel = _RGBTOINT((rbase >> 8), (gbase >> 8), (bbase >> 8));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textiel;
#else
					*(dest_addr + xi) = (USHORT)textiel;
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
			}
		}
		else
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = tl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textiel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textiel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textiel, &rbase, &gbase, &bbase);
					rbase *= ri;
					gbase *= gi;
					bbase *= bi;
					textiel = _RGBTOINT((rbase >> 8), (gbase >> 8), (bbase >> 8));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textiel;
#else
					*(dest_addr + xi) = (USHORT)textiel;
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
			}
		}
	}
	else
	{
		ye = y2;
		if (ye > max_clip_y)
			ye = max_clip_y;

		if (y1 < min_clip_y) //垂直裁剪
		{
			dyl = y2 - y1;

			dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			tl = (t1 << FIXP16_SHIFT) + dyl*dtdyl;
			sl = (s1 << FIXP16_SHIFT) + dyl*dsdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			tr = (t0 << FIXP16_SHIFT) + dyr*dtdyr;
			sr = (s0 << FIXP16_SHIFT) + dyr*dsdyr;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if (y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			tl = (t0 << FIXP16_SHIFT) + dy * dtdyl;
			sl = (s0 << FIXP16_SHIFT) + dy * dsdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			tr = (t0 << FIXP16_SHIFT) + dy * dtdyr;
			sr = (s0 << FIXP16_SHIFT) + dy * dsdyr;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0) << FIXP16_SHIFT) / dyl;
			dsdyl = ((s1 - s0) << FIXP16_SHIFT) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0) << FIXP16_SHIFT) / dyr;
			dsdyr = ((s2 - s0) << FIXP16_SHIFT) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			tl = (t0 << FIXP16_SHIFT);
			sl = (s0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);
			tr = (t0 << FIXP16_SHIFT);
			sr = (s0 << FIXP16_SHIFT);

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = tr - tl;
					dsdx = sr - sl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ti += dtdx * dx;
					si += dsdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textiel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textiel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textiel, &rbase, &gbase, &bbase);
					rbase *= ri;
					gbase *= gi;
					bbase *= bi;
					textiel = _RGBTOINT((rbase >> 8), (gbase >> 8), (bbase >> 8));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textiel;
#else
					*(dest_addr + xi) = (USHORT)textiel;
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
						dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1 << FIXP16_SHIFT);
						sl = (s1 << FIXP16_SHIFT);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dyr;
						dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1 << FIXP16_SHIFT);
						sr = (s1 << FIXP16_SHIFT);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
					}
				}
			}
		}
		else
		{
			for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl + FIXP16_ROUND_UP);
				si = (sl + FIXP16_ROUND_UP);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
				}

				for (int xi = xs; xi <= xe; xi++)
				{
#ifdef WINDDOW_BPP32
					textiel = ((UINT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#else
					textiel = ((USHORT*)bitmap->buffer)[(si >> FIXP16_SHIFT) * bitmap->bitmapinfoheader.biWidth + (ti >> FIXP16_SHIFT)];
#endif
					_RGBFROMINT(textiel, &rbase, &gbase, &bbase);
					rbase *= ri;
					gbase *= gi;
					bbase *= bi;
					textiel = _RGBTOINT((rbase >> 8), (gbase >> 8), (bbase >> 8));
#ifdef WINDDOW_BPP32
					*(dest_addr + xi) = textiel;
#else
					*(dest_addr + xi) = (USHORT)textiel;
#endif
					ti += dtdx;
					si += dsdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1) << FIXP16_SHIFT) / dyl;
						dsdyl = ((s2 - s1) << FIXP16_SHIFT) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1 << FIXP16_SHIFT);
						sl = (s1 << FIXP16_SHIFT);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1) << FIXP16_SHIFT) / dyr;
						dsdyr = ((s2 - s1) << FIXP16_SHIFT) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1 << FIXP16_SHIFT);
						sr = (s1 << FIXP16_SHIFT);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
					}
				}

			}
		}
	}
}
