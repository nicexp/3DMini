///////////////////
//工具接口
//////////////////
#include "3DTool.h"

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