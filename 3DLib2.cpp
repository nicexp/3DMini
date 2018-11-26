#include "3DLib2.h"

#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

//ddraw 
LPDIRECTDRAW7 lpdd = NULL;
LPDIRECTDRAWSURFACE7 lpddsprimary = NULL;
LPDIRECTDRAWSURFACE7 lpddsback = NULL;
LPDIRECTDRAWCLIPPER  lpddclipper = NULL;
LPDIRECTDRAWCLIPPER  lpddclipperwin = NULL;
DDSURFACEDESC2 ddsd;
DDSCAPS2 ddscaps;

//dinput
LPDIRECTINPUT8       lpdi = NULL;
LPDIRECTINPUTDEVICE8 lpdikey = NULL;
LPDIRECTINPUTDEVICE8 lpdimouse = NULL;

UCHAR                *primary_buffer = NULL;
UCHAR                *back_buffer = NULL;

int                  primary_lpitch = 0;
int                  back_lpitch = 0;

int window_client_x0 = 0;
int window_client_y0 = 0;

int dd_pixel_format = DD_PIXEL_FORMAT565;

HWND main_window_handle = NULL;
HINSTANCE main_instance = NULL;

UCHAR keyboard_state[256];
DIMOUSESTATE mouse_state;

int min_clip_x = 0, min_clip_y = 0, max_clip_x = WINDOW_WIDTH - 1, max_clip_y = WINDOW_HEIGHT - 1;

static int start_clock_count = 0;

LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width,
	int height,
	int mem_flags,
	USHORT color_key_value)
{
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 lpdds;

	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

	if (FAILED(lpdd->CreateSurface(&ddsd, &lpdds, NULL)))
		return(NULL);

	DDCOLORKEY color_key;
	color_key.dwColorSpaceLowValue = color_key_value;
	color_key.dwColorSpaceHighValue = color_key_value;

	lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);

	return lpdds;
}

int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT *client)
{
	DDBLTFX ddbltfx; 
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor = color;

	lpdds->Blt(client,
		NULL,       
		NULL,
		DDBLT_COLORFILL | DDBLT_WAIT,            
		&ddbltfx); 

	return 1;
}

LPDIRECTDRAWCLIPPER DDraw_Attach_Clipper(LPDIRECTDRAWSURFACE7 lpdds,
	int num_rects,
	LPRECT clip_list)

{
	int index;
	LPDIRECTDRAWCLIPPER lpddclipper;
	LPRGNDATA region_data;

	if (FAILED(lpdd->CreateClipper(0, &lpddclipper, NULL)))
		return(NULL);

	region_data = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT));

	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);

	region_data->rdh.dwSize = sizeof(RGNDATAHEADER);
	region_data->rdh.iType = RDH_RECTANGLES;
	region_data->rdh.nCount = num_rects;
	region_data->rdh.nRgnSize = num_rects*sizeof(RECT);

	region_data->rdh.rcBound.left = 64000;
	region_data->rdh.rcBound.top = 64000;
	region_data->rdh.rcBound.right = -64000;
	region_data->rdh.rcBound.bottom = -64000;

	for (index = 0; index<num_rects; index++)
	{
		if (clip_list[index].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[index].left;

		if (clip_list[index].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[index].right;

		if (clip_list[index].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[index].top;

		if (clip_list[index].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[index].bottom;

	}

	if (FAILED(lpddclipper->SetClipList(region_data, 0)))
	{
		free(region_data);
		return(NULL);
	}

	if (FAILED(lpdds->SetClipper(lpddclipper)))
	{

		free(region_data);
		return(NULL);
	}

	free(region_data);
	return(lpddclipper);
}

int DDraw_Init(int width, int height, int bpp, int windowed)
{
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
	{
		DEBUG_LOG("DirectDrawCreateEx failed");
		return 0;
	}
	if (WINDOWED_APP)
	{
		if (FAILED(lpdd->SetCooperativeLevel(main_window_handle, DDSCL_NORMAL)))
		{
			DEBUG_LOG("SetCooperativeLevel failed");
			return 0;
		}	
	}
	else
	{
		if (FAILED(lpdd->SetCooperativeLevel(main_window_handle, DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN |
			DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT | DDSCL_MULTITHREADED)))
		{
			DEBUG_LOG("SetCooperativeLevel failed");
			return 0;
		}
		if (FAILED(lpdd->SetDisplayMode(width, height, bpp, 0, 0)))
		{
			DEBUG_LOG("SetDisplayMode failed");
			return 0;
		}
	}

	DDRAW_INIT_STRUCT(ddsd);

	if (!windowed) //全屏模式
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 0;
	}

	lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);

	DDPIXELFORMAT ddpf;
	DDRAW_INIT_STRUCT(ddpf);

	lpddsprimary->GetPixelFormat(&ddpf);

	dd_pixel_format = ddpf.dwRGBBitCount;

	DEBUG_LOG("%d", ddpf.dwRGBBitCount);
	
	if (!windowed)
	{
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		if (FAILED(lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback)))
		{
			DEBUG_LOG("GetAttachedSurface failed");
			return 0;
		}
	}
	else
	{
		lpddsback = DDraw_Create_Surface(width, height, DDSCAPS_SYSTEMMEMORY);
	}

	if (windowed)
	{
		DDraw_Fill_Surface(lpddsback, 0);
	}
	else
	{
		DDraw_Fill_Surface(lpddsprimary, 0);
		DDraw_Fill_Surface(lpddsback, 0);
	}

	RECT rect = { 0, 0, width, height };
	lpddclipper = DDraw_Attach_Clipper(lpddsback, 1, &rect);

	if (windowed)
	{
		if (FAILED(lpdd->CreateClipper(0, &lpddclipperwin, NULL)))
		{
			DEBUG_LOG("CreateClipper failed");
			return 0;
		}
		if (FAILED(lpddclipperwin->SetHWnd(0, main_window_handle)))
		{
			DEBUG_LOG("lpddclipperwin SetHWnd failed");
			return 0;
		}
		if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
		{
			DEBUG_LOG("lpddsprimary SetClipper failed");
			return 0;
		}
	}

	DEBUG_LOG("DDrawInit Sucess");
	return 1;
}

int DInput_Init()
{
	if (FAILED(DirectInput8Create(main_instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&lpdi, NULL)))
	{
		DEBUG_LOG("DirectInput8Create failed");
		return 0;
	}

	DEBUG_LOG("DInput_Init Success");
	return 1;

}

int DInput_Init_Mouse()
{
	if (lpdi->CreateDevice(GUID_SysMouse, &lpdimouse, NULL) != DI_OK)
		return 0;

	if (lpdimouse->SetCooperativeLevel(main_window_handle,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return 0;

	if (lpdimouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return 0;

	if (lpdimouse->Acquire() != DI_OK)
		return 0;

	return 1;
}

void DInput_Release_Mouse()
{
	if (lpdimouse)
	{
		lpdimouse->Unacquire();
		lpdimouse->Release();
	}
}

int DInput_Init_Keyboard()
{
	if (lpdi->CreateDevice(GUID_SysKeyboard, &lpdikey, NULL) != DI_OK)
	{
		return 0;
	}
		

	if (lpdikey->SetCooperativeLevel(main_window_handle,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
	{
		DEBUG_LOG("keyboard SetCooperativeLevel failed");
		return 0;
	}
	if (lpdikey->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
	{
		DEBUG_LOG("SetDataFormat failed");
		return 0;
	}
	if (lpdikey->Acquire() != DI_OK)
	{
		DEBUG_LOG("Acquire failed");
		return 0;
	}

	DEBUG_LOG("DInput_Init_Keyboard Success");
	return 1;

}

void DInput_Release_Keyboard()
{
	if (lpdikey)
	{
		lpdikey->Unacquire();
		lpdikey->Release();
	}
}

void DInput_Shutdown()
{
	if (lpdi)
		lpdi->Release();
}

int DDraw_Shutdown()
{
	if (lpddclipper)
		lpddclipper->Release();
	if (lpddclipperwin)
		lpddclipperwin->Release();
	if (lpddsback)
		lpddsback->Release();
	if (lpddsprimary)
		lpddsprimary->Release();
	if (lpdd)
		lpdd->Release();

	return 1;
}

int Draw_Text_GDI(char *text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds)
{
	HDC xdc;
	if (FAILED(lpdds->GetDC(&xdc)))
	{
		DEBUG_LOG("GetDC failed");
		return 0;
	}
		
	SetTextColor(xdc, color);
	SetBkMode(xdc, TRANSPARENT);

	TextOut(xdc, x, y, text, strlen(text));

	lpdds->ReleaseDC(xdc);

	return 1;
}

UCHAR *DDraw_Lock_Back_Surface()
{
	if (back_buffer)
	{
		return(back_buffer);
	}

	DDRAW_INIT_STRUCT(ddsd);
	lpddsback->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, NULL);

	back_buffer = (UCHAR *)ddsd.lpSurface;
	back_lpitch = ddsd.lPitch;

	return(back_buffer);

}

int DDraw_Unlock_Back_Surface()
{
	if (!back_buffer)
	{
		DEBUG_LOG("back_buff null");
		return 0;
	}

	lpddsback->Unlock(NULL);

	back_buffer = NULL;
	back_lpitch = 0;

	return 1;
}

int DDraw_Flip(void)
{
	if (primary_buffer || back_buffer)
	{
		DEBUG_LOG("primary_buffer or back_buffer null");
		return 0;
	}

	if (!WINDOWED_APP)
		while (FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT)));
	else
	{
		RECT    dest_rect;

		GetWindowRect(main_window_handle, &dest_rect);

		dest_rect.left += window_client_x0;
		dest_rect.top += window_client_y0;

		dest_rect.right = dest_rect.left + WINDOW_WIDTH - 1;
		dest_rect.bottom = dest_rect.top + WINDOW_HEIGHT - 1;


		if (FAILED(lpddsprimary->Blt(&dest_rect, lpddsback, NULL, DDBLT_WAIT, NULL)))
			return 0;
	}

	return 1;
}

int Draw_Clip_Line(int x0, int y0, int x1, int y1, int color,
	UCHAR *dest_buffer, int lpitch)
{
	int cxs, cys,
		cxe, cye;

	cxs = x0;
	cys = y0;
	cxe = x1;
	cye = y1;

	if (Clip_Line(cxs, cys, cxe, cye))
		Draw_Line(cxs, cys, cxe, cye, color, dest_buffer, lpitch);

	return 1;
}

int Clip_Line(int &x1, int &y1, int &x2, int &y2)
{
	int xc1 = x1,
		yc1 = y1,
		xc2 = x2,
		yc2 = y2;

	int p1_code = 0,
		p2_code = 0;

	if (y1 < min_clip_y)
		p1_code |= CLIP_CODE_N;
	else
	if (y1 > max_clip_y)
		p1_code |= CLIP_CODE_S;

	if (x1 < min_clip_x)
		p1_code |= CLIP_CODE_W;
	else
	if (x1 > max_clip_x)
		p1_code |= CLIP_CODE_E;

	if (y2 < min_clip_y)
		p2_code |= CLIP_CODE_N;
	else
	if (y2 > max_clip_y)
		p2_code |= CLIP_CODE_S;

	if (x2 < min_clip_x)
		p2_code |= CLIP_CODE_W;
	else
	if (x2 > max_clip_x)
		p2_code |= CLIP_CODE_E;

	if ((p1_code & p2_code))
		return 0;

	if (p1_code == 0 && p2_code == 0)
		return 1;

	switch (p1_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
	{
						yc1 = min_clip_y;
						xc1 = x1 + 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1);
	} break;
	case CLIP_CODE_S:
	{
						yc1 = max_clip_y;
						xc1 = x1 + 0.5 + (max_clip_y - y1)*(x2 - x1) / (y2 - y1);
	} break;

	case CLIP_CODE_W:
	{
						xc1 = min_clip_x;
						yc1 = y1 + 0.5 + (min_clip_x - x1)*(y2 - y1) / (x2 - x1);
	} break;

	case CLIP_CODE_E:
	{
						xc1 = max_clip_x;
						yc1 = y1 + 0.5 + (max_clip_x - x1)*(y2 - y1) / (x2 - x1);
	} break;

	case CLIP_CODE_NE:
	{
						 yc1 = min_clip_y;
						 xc1 = x1 + 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1);

						 if (xc1 < min_clip_x || xc1 > max_clip_x)
						 {
							 xc1 = max_clip_x;
							 yc1 = y1 + 0.5 + (max_clip_x - x1)*(y2 - y1) / (x2 - x1);
						 }

	} break;

	case CLIP_CODE_SE:
	{
						 yc1 = max_clip_y;
						 xc1 = x1 + 0.5 + (max_clip_y - y1)*(x2 - x1) / (y2 - y1);

						 if (xc1 < min_clip_x || xc1 > max_clip_x)
						 {
							 xc1 = max_clip_x;
							 yc1 = y1 + 0.5 + (max_clip_x - x1)*(y2 - y1) / (x2 - x1);
						 }

	} break;

	case CLIP_CODE_NW:
	{
						 yc1 = min_clip_y;
						 xc1 = x1 + 0.5 + (min_clip_y - y1)*(x2 - x1) / (y2 - y1);

						 if (xc1 < min_clip_x || xc1 > max_clip_x)
						 {
							 xc1 = min_clip_x;
							 yc1 = y1 + 0.5 + (min_clip_x - x1)*(y2 - y1) / (x2 - x1);
						 }

	} break;

	case CLIP_CODE_SW:
	{
						 yc1 = max_clip_y;
						 xc1 = x1 + 0.5 + (max_clip_y - y1)*(x2 - x1) / (y2 - y1);

						 if (xc1 < min_clip_x || xc1 > max_clip_x)
						 {
							 xc1 = min_clip_x;
							 yc1 = y1 + 0.5 + (min_clip_x - x1)*(y2 - y1) / (x2 - x1);
						 }

	} break;

	default:break;

	}
	switch (p2_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
	{
						yc2 = min_clip_y;
						xc2 = x2 + (min_clip_y - y2)*(x1 - x2) / (y1 - y2);
	} break;

	case CLIP_CODE_S:
	{
						yc2 = max_clip_y;
						xc2 = x2 + (max_clip_y - y2)*(x1 - x2) / (y1 - y2);
	} break;

	case CLIP_CODE_W:
	{
						xc2 = min_clip_x;
						yc2 = y2 + (min_clip_x - x2)*(y1 - y2) / (x1 - x2);
	} break;

	case CLIP_CODE_E:
	{
						xc2 = max_clip_x;
						yc2 = y2 + (max_clip_x - x2)*(y1 - y2) / (x1 - x2);
	} break;

	case CLIP_CODE_NE:
	{
						 yc2 = min_clip_y;
						 xc2 = x2 + 0.5 + (min_clip_y - y2)*(x1 - x2) / (y1 - y2);

						 if (xc2 < min_clip_x || xc2 > max_clip_x)
						 {
							 xc2 = max_clip_x;
							 yc2 = y2 + 0.5 + (max_clip_x - x2)*(y1 - y2) / (x1 - x2);
						 }

	} break;

	case CLIP_CODE_SE:
	{
						 yc2 = max_clip_y;
						 xc2 = x2 + 0.5 + (max_clip_y - y2)*(x1 - x2) / (y1 - y2);

						 if (xc2 < min_clip_x || xc2 > max_clip_x)
						 {
							 xc2 = max_clip_x;
							 yc2 = y2 + 0.5 + (max_clip_x - x2)*(y1 - y2) / (x1 - x2);
						 }

	} break;

	case CLIP_CODE_NW:
	{
						 yc2 = min_clip_y;
						 xc2 = x2 + 0.5 + (min_clip_y - y2)*(x1 - x2) / (y1 - y2);

						 if (xc2 < min_clip_x || xc2 > max_clip_x)
						 {
							 xc2 = min_clip_x;
							 yc2 = y2 + 0.5 + (min_clip_x - x2)*(y1 - y2) / (x1 - x2);
						 }

	} break;

	case CLIP_CODE_SW:
	{
						 yc2 = max_clip_y;
						 xc2 = x2 + 0.5 + (max_clip_y - y2)*(x1 - x2) / (y1 - y2);

						 if (xc2 < min_clip_x || xc2 > max_clip_x)
						 {
							 xc2 = min_clip_x;
							 yc2 = y2 + 0.5 + (min_clip_x - x2)*(y1 - y2) / (x1 - x2);
						 }

	} break;

	default:break;

	}
	if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
		(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
		(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
		(yc2 < min_clip_y) || (yc2 > max_clip_y))
	{
		return 0;
	}

	x1 = xc1;
	y1 = yc1;
	x2 = xc2;
	y2 = yc2;

	return 1;
}

//使用的是Bresenham画点法(使用加减法代替乘除法避免了浮点数占用cpu周期)
int Draw_Line(int x0, int y0,
	int x1, int y1,
	int color,
	UCHAR *vb_start, int lpitch)
{
	int dx,
		dy,
		dx2,
		dy2,
		x_inc,
		y_inc,
		error,
		index;

#ifdef WINDDOW_BPP32
	int lpitch_2 = lpitch >> 2;
	UINT *vb_start2 = (UINT *)vb_start + x0 + y0*lpitch_2;
#else
	int lpitch_2 = lpitch >> 1;
	USHORT *vb_start2 = (USHORT *)vb_start + x0 + y0*lpitch_2;
#endif

	dx = x1 - x0;
	dy = y1 - y0;

	if (dx >= 0)
	{
		x_inc = 1;

	}
	else
	{
		x_inc = -1;
		dx = -dx;

	}
	if (dy >= 0)
	{
		y_inc = lpitch_2;
	}
	else
	{
		y_inc = -lpitch_2;
		dy = -dy;

	}
	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)
	{
		error = dy2 - dx;

		for (index = 0; index <= dx; index++)
		{
#ifdef WINDDOW_BPP32
			*vb_start2 = (UINT)color;
#else
			*vb_start2 = (USHORT)color;
#endif

			if (error >= 0)
			{
				error -= dx2;
				vb_start2 += y_inc;

			}

			error += dy2;
			vb_start2 += x_inc;

		}
	}
	else
	{
		error = dx2 - dy;

		for (index = 0; index <= dy; index++)
		{
#ifdef WINDDOW_BPP32
			*vb_start2 = (UINT)color;
#else
			*vb_start2 = (USHORT)color;
#endif
			if (error >= 0)
			{
				error -= dy2;
				vb_start2 += x_inc;

			}
			error += dx2;

			vb_start2 += y_inc;

		}
	}

	return 1;
}
//读取键盘
int DInput_Read_Keyboard()
{
	if (lpdikey)
	{
		if (lpdikey->GetDeviceState(256, (LPVOID)keyboard_state) != DI_OK)
			return 0;
	}
	else
	{
		memset(keyboard_state, 0, sizeof(keyboard_state));

		return 0;
	}
	return 1;
}
//读取鼠标
int DInput_Read_Mouse(void)
{
	if (lpdimouse)
	{
		if (lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state) != DI_OK)
			return(0);
	}
	else
	{
		memset(&mouse_state, 0, sizeof(mouse_state));
		return 0;
	}

	return 1;
}

//时钟控制
DWORD Get_Clock(void)
{
	return(GetTickCount());

}

DWORD Start_Clock(void)
{
	return(start_clock_count = Get_Clock());
}

DWORD Wait_Clock(DWORD count)
{
	while ((Get_Clock() - start_clock_count) < count);
	return(Get_Clock());
}