/*支持透视纹理映射，1/z缓存，光栅化填充模式遵循左上原则*/
#include "3DTexture2.h"

#define TRI_LHS	0 //左转折
#define TRI_RHS 1 //右转折

#define TRIANGLE_TOP  0 //平顶三角形
#define TRIANGLE_BOTTOM 1 //平底三角形
#define TRIANGLE_GENERAL 2 //一般三角形

//支持透视矫正和1/z缓存的固定纹理映射
void DrawTextureConstantWithPerInvz(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch)
{
	BITMAP_IMG_PTR bitmap = face->texture;
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
	//1/z缓存
	unsigned int* z_buffer = (unsigned int*)zbuffer;
	unsigned int* z_addr = NULL;

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
	if (((int)(face->tvlist[ver0].y + 0.0) == (int)(face->tvlist[ver1].y + 0.0)) &&
		((int)(face->tvlist[ver1].y + 0.0) == (int)(face->tvlist[ver2].y + 0.0)) ||
		((int)(face->tvlist[ver0].x + 0.0) == (int)(face->tvlist[ver1].x + 0.0)) &&
		((int)(face->tvlist[ver1].x + 0.0) == (int)(face->tvlist[ver2].x + 0.0)))
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
	if ((int)(face->tvlist[ver0].y + 0.0) == (int)(face->tvlist[ver1].y + 0.0))
	{
		tri_type = TRIANGLE_TOP;
		//顶点排序
		if (face->tvlist[ver1].x < face->tvlist[ver0].x)
		{
			SWAP(ver0, ver1, temp);
		}
	}
	//平底三角形
	else if ((int)(face->tvlist[ver1].y + 0.0) == (int)(face->tvlist[ver2].y + 0.0))
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
	int z0, z1, z2; //透视映射，1/z缓存

	x0 = int(face->tvlist[ver0].x + 0.0);
	y0 = int(face->tvlist[ver0].y + 0.0);
	z0 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver0].z + 0.5));
	t0 = ((int)(face->tvlist[ver0].u0 + 0.5) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z + 0.5));
	s0 = ((int)(face->tvlist[ver0].v0 + 0.5) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z + 0.5));
	
	x1 = int(face->tvlist[ver1].x + 0.0);
	y1 = int(face->tvlist[ver1].y + 0.0);
	z1 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver1].z + 0.5));
	t1 = ((int)(face->tvlist[ver1].u0 + 0.5) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z + 0.5));
	s1 = ((int)(face->tvlist[ver1].v0 + 0.5) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z + 0.5));
	
	x2 = int(face->tvlist[ver2].x + 0.0);
	y2 = int(face->tvlist[ver2].y + 0.0);
	z2 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver2].z + 0.5));
	t2 = ((int)(face->tvlist[ver2].u0 + 0.5) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z + 0.5));
	s2 = ((int)(face->tvlist[ver2].v0 + 0.5) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z + 0.5));
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dxdyr;
	int dtdyl, dsdyl, dtdyr, dsdyr;
	int dzdyl, dzdyr;
	int xl, xr;
	int tl, sl, tr, sr;
	int zl, zr;
	int ti, si;
	int zi;
	int dtdx, dsdx;
	int dzdx;
	int dyl, dyr;
	//避免光栅化时执行除法
	int z_1 = 1 << ZBUFFER_SHIFT;
	int z_temp; 
	int z_shift = ZBUFFER_SHIFT - ZCORRECT_SHIFT;

	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t2 - t0)) / dy;
			dsdyl = ((s2 - s0)) / dy;
			dzdyl = ((z2 - z0)) / dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t1)) / dy;
			dsdyr = ((s2 - s1)) / dy;
			dzdyr = ((z2 - z1)) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				tl = (t0) + (dy * dtdyl);
				sl = (s0) + (dy * dsdyl);
				zl = (z0) + (dy * dzdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				tr = (t1) + (dy * dtdyr);
				sr = (s1) + (dy * dsdyr);
				zr = (z1) + (dy * dzdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0);
				sl = (s0);
				zl = (z0);
				xr = (x1 << FIXP16_SHIFT);
				tr = (t1);
				sr = (s1);
				zr = (z1);

				ys = y0;
			}
		}
		if (tri_type == TRIANGLE_BOTTOM)
		{
			dy = y2 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t1 - t0)) / dy;
			dsdyl = ((s1 - s0)) / dy;
			dzdyl = ((z1 - z0)) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t0)) / dy;
			dsdyr = ((s2 - s0)) / dy;
			dzdyr = ((z2 - z0)) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				tl = (t0) + (dtdyl * dy);
				sl = (s0) + (dsdyl * dy);
				zl = (z0) + (dzdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				tr = (t0) + (dtdyr * dy);
				sr = (s0) + (dsdyr * dy);
				zr = (z0) + (dzdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0);
				sl = (s0);
				zl = (z0);
				xr = (x0 << FIXP16_SHIFT);
				tr = (t0);
				sr = (s0);
				zr = (z0);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;
		z_addr = z_buffer + ys * zpitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = (xe - xs);
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ti += dtdx * dx;
					si += dsdx * dx;
					zi += dzdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
					xe = max_clip_x;

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}

					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;
			}
		}
		else
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = tl;
				si = sl;
				zi = zl;

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}

					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;
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
			dtdyl = ((t2 - t1)) / dyl;
			dsdyl = ((s2 - s1)) / dyl;
			dzdyl = ((z2 - z1)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			tl = (t1) + dyl*dtdyl;
			sl = (s1) + dyl*dsdyl;
			zl = (z1) + dyl*dzdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			tr = (t0) + dyr*dtdyr;
			sr = (s0) + dyr*dsdyr;
			zr = (z0) + dyr*dzdyr;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if (y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0)) / dyl;
			dsdyl = ((s1 - s0)) / dyl;
			dzdyl = ((z1 - z0)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			tl = (t0) + dy * dtdyl;
			sl = (s0) + dy * dsdyl;
			zl = (z0) + dy * dzdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			tr = (t0) + dy * dtdyr;
			sr = (s0) + dy * dsdyr;
			zr = (z0) + dy * dzdyr;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0)) / dyl;
			dsdyl = ((s1 - s0)) / dyl;
			dzdyl = ((z1 - z0)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			tl = (t0);
			sl = (s0);
			zl = (z0);
			xr = (x0 << FIXP16_SHIFT);
			tr = (t0);
			sr = (s0);
			zr = (z0);

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;
		z_addr = z_buffer + ys*zpitch;
		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = tr - tl;
					dsdx = sr - sl;
					dzdx = zr - zl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ti += dtdx * dx;
					si += dsdx * dx;
					zi += dzdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1)) / dyl;
						dsdyl = ((s2 - s1)) / dyl;
						dzdyl = ((z2 - z1)) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1);
						sl = (s1);
						zl = (z1);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
						zl += dzdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1)) / dyr;
						dsdyr = ((s2 - s1)) / dyr;
						dzdyr = ((z2 - z1)) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1);
						sr = (s1);
						zr = (z1);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
						zr += dzdyr;
					}
				}
			}
		}
		else
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						*(dest_addr + xi) = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1)) / dyl;
						dsdyl = ((s2 - s1)) / dyl;
						dzdyl = ((z2 - z1)) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1);
						sl = (s1);
						zl = (z1);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
						zl += dzdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1)) / dyr;
						dsdyr = ((s2 - s1)) / dyr;
						dzdyr = ((z2 - z1)) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1);
						sr = (s1);
						zr = (z1);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
						zr += dzdyr;
					}
				}

			}
		}
	}
}
//支持透视矫正和1/z缓存的gouraud纹理映射
void DrawTextureGouraudWithPerInvz(POLYF4DV2_PTR face,unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch)
{
	BITMAP_IMG_PTR bitmap = face->texture;
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
	//1/z缓存
	unsigned int* z_buffer = (unsigned int*)zbuffer;
	unsigned int* z_addr = NULL;

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
	if (((int)(face->tvlist[ver0].y + 0.0) == (int)(face->tvlist[ver1].y + 0.0)) &&
		((int)(face->tvlist[ver1].y + 0.0) == (int)(face->tvlist[ver2].y + 0.0)) ||
		((int)(face->tvlist[ver0].x + 0.0) == (int)(face->tvlist[ver1].x + 0.0)) &&
		((int)(face->tvlist[ver1].x + 0.0) == (int)(face->tvlist[ver2].x + 0.0)))
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
	if ((int)(face->tvlist[ver0].y + 0.0) == (int)(face->tvlist[ver1].y + 0.0))
	{
		tri_type = TRIANGLE_TOP;
		//顶点排序
		if (face->tvlist[ver1].x < face->tvlist[ver0].x)
		{
			SWAP(ver0, ver1, temp);
		}
	}
	//平底三角形
	else if ((int)(face->tvlist[ver1].y + 0.0) == (int)(face->tvlist[ver2].y + 0.0))
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
	int z0, z1, z2; //透视映射，1/z缓存

	x0 = int(face->tvlist[ver0].x + 0.0);
	y0 = int(face->tvlist[ver0].y + 0.0);
	z0 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver0].z));
	t0 = ((int)(face->tvlist[ver0].u0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));
	s0 = ((int)(face->tvlist[ver0].v0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));
	u0 = (r_base0 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));
	v0 = (g_base0 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));
	w0 = (b_base0 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));


	x1 = int(face->tvlist[ver1].x + 0.0);
	y1 = int(face->tvlist[ver1].y + 0.0);
	z1 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver1].z));
	t1 = ((int)(face->tvlist[ver1].u0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));
	s1 = ((int)(face->tvlist[ver1].v0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));
	u1 = (r_base1 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));
	v1 = (g_base1 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));
	w1 = (b_base1 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));

	x2 = int(face->tvlist[ver2].x + 0.0);
	y2 = int(face->tvlist[ver2].y + 0.0);
	z2 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver2].z));
	t2 = ((int)(face->tvlist[ver2].u0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	s2 = ((int)(face->tvlist[ver2].v0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	u2 = (r_base2 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	v2 = (g_base2 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	w2 = (b_base2 << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dudyl, dvdyl, dwdyl, dxdyr, dudyr, dvdyr, dwdyr;
	int dtdyl, dsdyl, dtdyr, dsdyr;
	int dzdyl, dzdyr;
	int xl, ul, vl, wl, xr, ur, vr, wr;
	int tl, sl, tr, sr;
	int zl, zr;
	int ui, vi, wi;
	int ti, si;
	int zi;
	int dudx, dvdx, dwdx;
	int dtdx, dsdx;
	int dzdx;
	int dyl, dyr;
	//避免光栅化时执行除法
	int z_1 = 1 << ZBUFFER_SHIFT;
	int z_temp;
	int z_shift = ZBUFFER_SHIFT - ZCORRECT_SHIFT;

	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dudyl = ((u2 - u0)) / dy;
			dvdyl = ((v2 - v0)) / dy;
			dwdyl = ((w2 - w0)) / dy;
			dtdyl = ((t2 - t0)) / dy;
			dsdyl = ((s2 - s0)) / dy;
			dzdyl = ((z2 - z0)) / dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dy;
			dudyr = ((u2 - u1)) / dy;
			dvdyr = ((v2 - v1)) / dy;
			dwdyr = ((w2 - w1)) / dy;
			dtdyr = ((t2 - t1)) / dy;
			dsdyr = ((s2 - s1)) / dy;
			dzdyr = ((z2 - z1)) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				ul = (u0) + (dy * dudyl);
				vl = (v0) + (dy * dvdyl);
				wl = (w0) + (dy * dwdyl);
				tl = (t0) + (dy * dtdyl);
				sl = (s0) + (dy * dsdyl);
				zl = (z0) + (dy * dzdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				ur = (u1) + (dy * dudyr);
				vr = (v1) + (dy * dvdyr);
				wr = (w1) + (dy * dwdyr);
				tr = (t1) + (dy * dtdyr);
				sr = (s1) + (dy * dsdyr);
				zr = (z1) + (dy * dzdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				ul = (u0);
				vl = (v0);
				wl = (w0);
				tl = (t0);
				sl = (s0);
				zl = (z0);
				xr = (x1 << FIXP16_SHIFT);
				ur = (u1);
				vr = (v1);
				wr = (w1);
				tr = (t1);
				sr = (s1);
				zr = (z1);

				ys = y0;
			}
		}
		if (tri_type == TRIANGLE_BOTTOM)
		{
			dy = y2 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dy;
			dudyl = ((u1 - u0)) / dy;
			dvdyl = ((v1 - v0)) / dy;
			dwdyl = ((w1 - w0)) / dy;
			dtdyl = ((t1 - t0)) / dy;
			dsdyl = ((s1 - s0)) / dy;
			dzdyl = ((z1 - z0)) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dudyr = ((u2 - u0)) / dy;
			dvdyr = ((v2 - v0)) / dy;
			dwdyr = ((w2 - w0)) / dy;
			dtdyr = ((t2 - t0)) / dy;
			dsdyr = ((s2 - s0)) / dy;
			dzdyr = ((z2 - z0)) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				ul = (u0) + (dudyl * dy);
				vl = (v0) + (dvdyl * dy);
				wl = (w0) + (dwdyl * dy);
				tl = (t0) + (dtdyl * dy);
				sl = (s0) + (dsdyl * dy);
				zl = (z0) + (dzdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				ur = (u0) + (dudyr * dy);
				vr = (v0) + (dvdyr * dy);
				wr = (w0) + (dwdyr * dy);
				tr = (t0) + (dtdyr * dy);
				sr = (s0) + (dsdyr * dy);
				zr = (z0) + (dzdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				ul = (u0);
				vl = (v0);
				wl = (w0);
				tl = (t0);
				sl = (s0);
				zl = (z0);
				xr = (x0 << FIXP16_SHIFT);
				ur = (u0);
				vr = (v0);
				wr = (w0);
				tr = (t0);
				sr = (s0);
				zr = (z0);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;
		z_addr = z_buffer + ys * zpitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul);
				vi = (vl);
				wi = (wl);
				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = (xe - xs);
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = tr - tl;
					dsdx = sr - sl;
					dzdx = zr - zl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ui += dudx * dx;
					vi += dvdx * dx;
					wi += dwdx * dx;
					ti += dtdx * dx;
					si += dsdx * dx;
					zi += dzdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
					xe = max_clip_x;

				for (int xi = xs; xi < xe; xi++)
				{

					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						_RGBFROMINT(textel, &ri, &gi, &bi); //取纹素rgb分量
						ri *= (ui) / (zi >> z_shift);
						gi *= (vi) / (zi >> z_shift);
						bi *= (wi) / (zi >> z_shift);
						textel = _RGBTOINT((ri >> (8)), (gi >> (8)), (bi >> (8)));
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = textel;
#else
						*(dest_addr + xi) = (USHORT)textel;
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}

					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;
			}
		}
		else
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = ul;
				vi = vl;
				wi = wl;
				ti = tl;
				si = sl;
				zi = zl;

				dx = xe - xs;
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						_RGBFROMINT(textel, &ri, &gi, &bi); //取纹素rgb分量
						ri *= (ui) / (zi >> z_shift);
						gi *= (vi) / (zi >> z_shift);
						bi *= (wi) / (zi >> z_shift);
						textel = _RGBTOINT((ri >> (8)), (gi >> (8)), (bi >> (8)));
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = textel;
#else
						*(dest_addr + xi) = (USHORT)textel;
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;
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
			dudyl = ((u2 - u1)) / dyl;
			dvdyl = ((v2 - v1)) / dyl;
			dwdyl = ((w2 - w1)) / dyl;
			dtdyl = ((t2 - t1)) / dyl;
			dsdyl = ((s2 - s1)) / dyl;
			dzdyl = ((z2 - z1)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0)) / dyr;
			dvdyr = ((v2 - v0)) / dyr;
			dwdyr = ((w2 - w0)) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			ul = (u1) + dyl*dudyl;
			vl = (v1) + dyl*dvdyl;
			wl = (w1) + dyl*dwdyl;
			tl = (t1) + dyl*dtdyl;
			sl = (s1) + dyl*dsdyl;
			zl = (z1) + dyl*dzdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			ur = (u0) + dyr*dudyr;
			vr = (v0) + dyr*dvdyr;
			wr = (w0) + dyr*dwdyr;
			tr = (t0) + dyr*dtdyr;
			sr = (s0) + dyr*dsdyr;
			zr = (z0) + dyr*dzdyr;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if (y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dudyl = ((u1 - u0)) / dyl;
			dvdyl = ((v1 - v0)) / dyl;
			dwdyl = ((w1 - w0)) / dyl;
			dtdyl = ((t1 - t0)) / dyl;
			dsdyl = ((s1 - s0)) / dyl;
			dzdyl = ((z1 - z0)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0)) / dyr;
			dvdyr = ((v2 - v0)) / dyr;
			dwdyr = ((w2 - w0)) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			ul = (u0) + dy * dudyl;
			vl = (v0) + dy * dvdyl;
			wl = (w0) + dy * dwdyl;
			tl = (t0) + dy * dtdyl;
			sl = (s0) + dy * dsdyl;
			zl = (z0) + dy * dzdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			ur = (u0) + dy * dudyr;
			vr = (v0) + dy * dvdyr;
			wr = (w0) + dy * dwdyr;
			tr = (t0) + dy * dtdyr;
			sr = (s0) + dy * dsdyr;
			zr = (z0) + dy * dzdyr;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dudyl = ((u1 - u0)) / dyl;
			dvdyl = ((v1 - v0)) / dyl;
			dwdyl = ((w1 - w0)) / dyl;
			dtdyl = ((t1 - t0)) / dyl;
			dsdyl = ((s1 - s0)) / dyl;
			dzdyl = ((z1 - z0)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dudyr = ((u2 - u0)) / dyr;
			dvdyr = ((v2 - v0)) / dyr;
			dwdyr = ((w2 - w0)) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			ul = (u0);
			vl = (v0);
			wl = (w0);
			tl = (t0);
			sl = (s0);
			zl = (z0);
			xr = (x0 << FIXP16_SHIFT);
			ur = (u0);
			vr = (v0);
			wr = (w0);
			tr = (t0);
			sr = (s0);
			zr = (z0);

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dudyl, dudyr, temp);
				SWAP(dvdyl, dvdyr, temp);
				SWAP(dwdyl, dwdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(ul, ur, temp);
				SWAP(vl, vr, temp);
				SWAP(wl, wr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;
		z_addr = z_buffer + ys*zpitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul);
				vi = (vl);
				wi = (wl);
				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = xe - xs;
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = tr - tl;
					dsdx = sr - sl;
					dzdx = zr - zl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ui += dudx * dx;
					vi += dvdx * dx;
					wi += dwdx * dx;
					ti += dtdx * dx;
					si += dsdx * dx;
					zi += dzdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						_RGBFROMINT(textel, &ri, &gi, &bi); //取纹素rgb分量
						ri *= (ui) / (zi >> z_shift);
						gi *= (vi) / (zi >> z_shift);
						bi *= (wi) / (zi >> z_shift);
						textel = _RGBTOINT((ri >> (8)), (gi >> (8)), (bi >> (8)));
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = textel;
#else
						*(dest_addr + xi) = (USHORT)textel;
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dudyl = ((u2 - u1)) / dyl;
						dvdyl = ((v2 - v1)) / dyl;
						dwdyl = ((w2 - w1)) / dyl;
						dtdyl = ((t2 - t1)) / dyl;
						dsdyl = ((s2 - s1)) / dyl;
						dzdyl = ((z2 - z1)) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (u1);
						vl = (v1);
						wl = (w1);
						tl = (t1);
						sl = (s1);
						zl = (z1);

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
						tl += dtdyl;
						sl += dsdyl;
						zl += dzdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dudyr = ((u2 - u1)) / dyr;
						dvdyr = ((v2 - v1)) / dyr;
						dwdyr = ((w2 - w1)) / dyr;
						dtdyr = ((t2 - t1)) / dyr;
						dsdyr = ((s2 - s1)) / dyr;
						dzdyr = ((z2 - z1)) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						ur = (u1);
						vr = (v1);
						wr = (w1);
						tr = (t1);
						sr = (s1);
						zr = (z1);

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
						tr += dtdyr;
						sr += dsdyr;
						zr += dzdyr;
					}
				}
			}
		}
		else
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ui = (ul);
				vi = (vl);
				wi = (wl);
				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = xe - xs;
				if (dx > 0)
				{
					dudx = (ur - ul) / dx;
					dvdx = (vr - vl) / dx;
					dwdx = (wr - wl) / dx;
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dudx = ur - ul;
					dvdx = vr - vl;
					dwdx = wr - wl;
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#endif
						_RGBFROMINT(textel, &ri, &gi, &bi); //取纹素rgb分量
						ri *= (ui) / (zi >> z_shift);
						gi *= (vi) / (zi >> z_shift);
						bi *= (wi) / (zi >> z_shift);
						textel = _RGBTOINT((ri >> (8)), (gi >> (8)), (bi >> (8)));
#ifdef WINDDOW_BPP32
						*(dest_addr + xi) = textel;
#else
						*(dest_addr + xi) = (USHORT)textel;
#endif
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ui += dudx;
					vi += dvdx;
					wi += dwdx;
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				ul += dudyl;
				vl += dvdyl;
				wl += dwdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				ur += dudyr;
				vr += dvdyr;
				wr += dwdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dudyl = ((u2 - u1)) / dyl;
						dvdyl = ((v2 - v1)) / dyl;
						dwdyl = ((w2 - w1)) / dyl;
						dtdyl = ((t2 - t1)) / dyl;
						dsdyl = ((s2 - s1)) / dyl;
						dzdyl = ((z2 - z1)) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						ul = (u1);
						vl = (v1);
						wl = (w1);
						tl = (t1);
						sl = (s1);
						zl = (z1);

						xl += dxdyl;
						ul += dudyl;
						vl += dvdyl;
						wl += dwdyl;
						tl += dtdyl;
						sl += dsdyl;
						zl += dzdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dudyr = ((u2 - u1)) / dyr;
						dvdyr = ((v2 - v1)) / dyr;
						dwdyr = ((w2 - w1)) / dyr;
						dtdyr = ((t2 - t1)) / dyr;
						dsdyr = ((s2 - s1)) / dyr;
						dzdyr = ((z2 - z1)) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						ur = (u1);
						vr = (v1);
						wr = (w1);
						tr = (t1);
						sr = (s1);
						zr = (z1);

						xr += dxdyr;
						ur += dudyr;
						vr += dvdyr;
						wr += dwdyr;
						tr += dtdyr;
						sr += dsdyr;
						zr += dzdyr;
					}
				}

			}
		}
	}
}
//支持透视矫正和1/z缓存的恒定纹理映射
void DrawTextureFlatWithPerInvz(POLYF4DV2_PTR face, unsigned char *_dest_buffer, int mempitch, UCHAR* zbuffer, int zpitch)
{
	BITMAP_IMG_PTR bitmap = face->texture;
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
	//1/z缓存
	unsigned int* z_buffer = (unsigned int*)zbuffer;
	unsigned int* z_addr = NULL;

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
	if (((int)(face->tvlist[ver0].y + 0.0) == (int)(face->tvlist[ver1].y + 0.0)) &&
		((int)(face->tvlist[ver1].y + 0.0) == (int)(face->tvlist[ver2].y + 0.0)) ||
		((int)(face->tvlist[ver0].x + 0.0) == (int)(face->tvlist[ver1].x + 0.0)) &&
		((int)(face->tvlist[ver1].x + 0.0) == (int)(face->tvlist[ver2].x + 0.0)))
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
	if ((int)(face->tvlist[ver0].y + 0.0) == (int)(face->tvlist[ver1].y + 0.0))
	{
		tri_type = TRIANGLE_TOP;
		//顶点排序
		if (face->tvlist[ver1].x < face->tvlist[ver0].x)
		{
			SWAP(ver0, ver1, temp);
		}
	}
	//平底三角形
	else if ((int)(face->tvlist[ver1].y + 0.0) == (int)(face->tvlist[ver2].y + 0.0))
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
	int z0, z1, z2; //透视映射，1/z缓存

	x0 = int(face->tvlist[ver0].x + 0.0);
	y0 = int(face->tvlist[ver0].y + 0.0);
	z0 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver0].z));
	t0 = ((int)(face->tvlist[ver0].u0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));
	s0 = ((int)(face->tvlist[ver0].v0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver0].z));
	x1 = int(face->tvlist[ver1].x + 0.0);
	y1 = int(face->tvlist[ver1].y + 0.0);
	z1 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver1].z));
	t1 = ((int)(face->tvlist[ver1].u0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));
	s1 = ((int)(face->tvlist[ver1].v0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver1].z));
	x2 = int(face->tvlist[ver2].x + 0.0);
	y2 = int(face->tvlist[ver2].y + 0.0);
	z2 = (1 << ZBUFFER_SHIFT) / ((int)(face->tvlist[ver2].z));
	t2 = ((int)(face->tvlist[ver2].u0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	s2 = ((int)(face->tvlist[ver2].v0) << ZCORRECT_SHIFT) / ((int)(face->tvlist[ver2].z));
	//设置转折点
	int yrestart = y1;
	//起点
	int ys, ye, xs, xe;
	//增量变量
	int dy, dx;
	int dxdyl, dxdyr;
	int dtdyl, dsdyl, dtdyr, dsdyr;
	int dzdyl, dzdyr;
	int xl, xr;
	int tl, sl, tr, sr;
	int zl, zr;
	int ti, si;
	int zi;
	int dtdx, dsdx;
	int dzdx;
	int dyl, dyr;
	//避免光栅化时执行除法
	int z_1 = 1 << ZBUFFER_SHIFT;
	int z_temp;
	int z_shift = ZBUFFER_SHIFT - ZCORRECT_SHIFT;

	//取顶点颜色
	_RGBFROMINT(face->lit_color[0], &ri, &gi, &bi);

	if (tri_type == TRIANGLE_TOP || tri_type == TRIANGLE_BOTTOM) //平顶三角形
	{
		if (tri_type == TRIANGLE_TOP)
		{

			dy = y2 - y0;
			dxdyl = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t2 - t0)) / dy;
			dsdyl = ((s2 - s0)) / dy;
			dzdyl = ((z2 - z0)) / dy;
			dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t1)) / dy;
			dsdyr = ((s2 - s1)) / dy;
			dzdyr = ((z2 - z1)) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;
				xl = (x0 << FIXP16_SHIFT) + (dy * dxdyl);
				tl = (t0)+(dy * dtdyl);
				sl = (s0)+(dy * dsdyl);
				zl = (z0)+(dy * dzdyl);
				xr = (x1 << FIXP16_SHIFT) + (dy * dxdyr);
				tr = (t1)+(dy * dtdyr);
				sr = (s1)+(dy * dsdyr);
				zr = (z1)+(dy * dzdyr);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0);
				sl = (s0);
				zl = (z0);
				xr = (x1 << FIXP16_SHIFT);
				tr = (t1);
				sr = (s1);
				zr = (z1);

				ys = y0;
			}
		}
		if (tri_type == TRIANGLE_BOTTOM)
		{
			dy = y2 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dy;
			dtdyl = ((t1 - t0)) / dy;
			dsdyl = ((s1 - s0)) / dy;
			dzdyl = ((z1 - z0)) / dy;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dy;
			dtdyr = ((t2 - t0)) / dy;
			dsdyr = ((s2 - s0)) / dy;
			dzdyr = ((z2 - z0)) / dy;

			if (y0 < min_clip_y) //垂直裁剪
			{
				dy = min_clip_y - y0;

				xl = (x0 << FIXP16_SHIFT) + (dxdyl * dy);
				tl = (t0)+(dtdyl * dy);
				sl = (s0)+(dsdyl * dy);
				zl = (z0)+(dzdyl * dy);
				xr = (x0 << FIXP16_SHIFT) + (dxdyr * dy);
				tr = (t0)+(dtdyr * dy);
				sr = (s0)+(dsdyr * dy);
				zr = (z0)+(dzdyr * dy);

				ys = min_clip_y;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				tl = (t0);
				sl = (s0);
				zl = (z0);
				xr = (x0 << FIXP16_SHIFT);
				tr = (t0);
				sr = (s0);
				zr = (z0);

				ys = y0;
			}
		}
		//设置ye
		ye = y2;
		if (y2 > max_clip_y)
			ye = max_clip_y;

		dest_addr = dest_buffer + ys * mempitch;
		z_addr = z_buffer + ys * zpitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = (xe - xs);
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;

					ti += dtdx * dx;
					si += dsdx * dx;
					zi += dzdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
					xe = max_clip_x;

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textiel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textiel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
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
						//更新1/z缓存
						z_addr[xi] = zi;
					}

					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;
			}
		}
		else
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = tl;
				si = sl;
				zi = zl;

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textiel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textiel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
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
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;
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
			dtdyl = ((t2 - t1)) / dyl;
			dsdyl = ((s2 - s1)) / dyl;
			dzdyl = ((z2 - z1)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			dyl = min_clip_y - y1;
			xl = (x1 << FIXP16_SHIFT) + dyl*dxdyl;
			tl = (t1)+dyl*dtdyl;
			sl = (s1)+dyl*dsdyl;
			zl = (z1)+dyl*dzdyl;

			dyr = min_clip_y - y0;
			xr = (x0 << FIXP16_SHIFT) + dyr*dxdyr;
			tr = (t0)+dyr*dtdyr;
			sr = (s0)+dyr*dsdyr;
			zr = (z0)+dyr*dzdyr;

			if (dxdyr > dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else if (y0 < min_clip_y)
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0)) / dyl;
			dsdyl = ((s1 - s0)) / dyl;
			dzdyl = ((z1 - z0)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			dy = min_clip_y - y0;

			xl = (x0 << FIXP16_SHIFT) + dy * dxdyl;
			tl = (t0)+dy * dtdyl;
			sl = (s0)+dy * dsdyl;
			zl = (z0)+dy * dzdyl;
			xr = (x0 << FIXP16_SHIFT) + dy * dxdyr;
			tr = (t0)+dy * dtdyr;
			sr = (s0)+dy * dsdyr;
			zr = (z0)+dy * dzdyr;

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = min_clip_y;
		}
		else
		{
			dyl = y1 - y0;

			dxdyl = ((x1 - x0) << FIXP16_SHIFT) / dyl;
			dtdyl = ((t1 - t0)) / dyl;
			dsdyl = ((s1 - s0)) / dyl;
			dzdyl = ((z1 - z0)) / dyl;

			dyr = y2 - y0;

			dxdyr = ((x2 - x0) << FIXP16_SHIFT) / dyr;
			dtdyr = ((t2 - t0)) / dyr;
			dsdyr = ((s2 - s0)) / dyr;
			dzdyr = ((z2 - z0)) / dyr;

			xl = (x0 << FIXP16_SHIFT);
			tl = (t0);
			sl = (s0);
			zl = (z0);
			xr = (x0 << FIXP16_SHIFT);
			tr = (t0);
			sr = (s0);
			zr = (z0);

			if (dxdyr < dxdyl)
			{
				SWAP(dxdyl, dxdyr, temp);
				SWAP(dtdyl, dtdyr, temp);
				SWAP(dsdyl, dsdyr, temp);
				SWAP(dzdyl, dzdyr, temp);
				SWAP(xl, xr, temp);
				SWAP(tl, tr, temp);
				SWAP(sl, sr, temp);
				SWAP(zl, zr, temp);
				irestart = TRI_RHS;
			}

			ys = y0;
		}

		dest_addr = dest_buffer + ys*mempitch;
		z_addr = z_buffer + ys*zpitch;

		//水平裁剪
		if ((x0 < min_clip_x) || (x0 > max_clip_x)
			|| (x1 < min_clip_x) || (x1 > max_clip_x)
			|| (x2 < min_clip_x) || (x2 > max_clip_x))
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = tr - tl;
					dsdx = sr - sl;
					dzdx = zr - zl;
				}

				if (xs < min_clip_x)
				{
					dx = min_clip_x - xs;
					ti += dtdx * dx;
					si += dsdx * dx;
					zi += dzdx * dx;

					xs = min_clip_x;
				}

				if (xe > max_clip_x)
				{
					xe = max_clip_x;
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textiel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textiel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
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
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1)) / dyl;
						dsdyl = ((s2 - s1)) / dyl;
						dzdyl = ((z2 - z1)) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1);
						sl = (s1);
						zl = (z1);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
						zl += dzdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1)) / dyr;
						dsdyr = ((s2 - s1)) / dyr;
						dzdyr = ((z2 - z1)) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1);
						sr = (s1);
						zr = (z1);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
						zr += dzdyr;
					}
				}
			}
		}
		else
		{
			for (int yi = ys; yi < ye; yi++, dest_addr += mempitch, z_addr += zpitch)
			{
				xs = (xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT;
				xe = (xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT;

				ti = (tl);
				si = (sl);
				zi = (zl);

				dx = xe - xs;
				if (dx > 0)
				{
					dtdx = (tr - tl) / dx;
					dsdx = (sr - sl) / dx;
					dzdx = (zr - zl) / dx;
				}
				else
				{
					dtdx = (tr - tl);
					dsdx = (sr - sl);
					dzdx = (zr - zl);
				}

				for (int xi = xs; xi < xe; xi++)
				{
					if (zi > z_addr[xi])
					{
#ifdef WINDDOW_BPP32
						textiel = ((UINT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
#else
						textiel = ((USHORT*)bitmap->buffer)[((si) / (zi >> z_shift)) * bitmap->width + ((ti) / (zi >> z_shift))];
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
						//更新1/z缓存
						z_addr[xi] = zi;
					}
					ti += dtdx;
					si += dsdx;
					zi += dzdx;
				}

				xl += dxdyl;
				tl += dtdyl;
				sl += dsdyl;
				zl += dzdyl;
				xr += dxdyr;
				tr += dtdyr;
				sr += dsdyr;
				zr += dzdyr;

				if (yi == yrestart)
				{
					if (irestart == TRI_LHS)
					{
						dyl = y2 - y1;

						dxdyl = ((x2 - x1) << FIXP16_SHIFT) / dyl;
						dtdyl = ((t2 - t1)) / dyl;
						dsdyl = ((s2 - s1)) / dyl;
						dzdyl = ((z2 - z1)) / dyl;

						xl = (x1 << FIXP16_SHIFT);
						tl = (t1);
						sl = (s1);
						zl = (z1);

						xl += dxdyl;
						tl += dtdyl;
						sl += dsdyl;
						zl += dzdyl;
					}
					else
					{
						dyr = y2 - y1;

						dxdyr = ((x2 - x1) << FIXP16_SHIFT) / dyr;
						dtdyr = ((t2 - t1)) / dyr;
						dsdyr = ((s2 - s1)) / dyr;
						dzdyr = ((z2 - z1)) / dyr;

						xr = (x1 << FIXP16_SHIFT);
						tr = (t1);
						sr = (s1);
						zr = (z1);

						xr += dxdyr;
						tr += dtdyr;
						sr += dsdyr;
						zr += dzdyr;
					}
				}

			}
		}
	}
}
