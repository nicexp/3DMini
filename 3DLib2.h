#ifndef _3DLIB2_H
#define _3DLIB2_H

#include <ddraw.h>
#include <dinput.h>
#include "3DLog.h"
#include "3DLib1.h"

#define DD_PIXEL_FORMAT565      16

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

extern LPDIRECTDRAW7 lpdd;
extern LPDIRECTDRAWSURFACE7 lpddsprimary;
extern LPDIRECTDRAWSURFACE7 lpddsback;
extern LPDIRECTDRAWCLIPPER  lpddclipper;
extern LPDIRECTDRAWCLIPPER  lpddclipperwin;
extern DDSURFACEDESC2 ddsd;
extern DDSCAPS2 ddscaps;

//dinput
extern LPDIRECTINPUT8       lpdi;
extern LPDIRECTINPUTDEVICE8 lpdikey;
extern LPDIRECTINPUTDEVICE8 lpdimouse;

extern UCHAR                *primary_buffer;
extern UCHAR                *back_buffer;

extern int                  primary_lpitch;
extern int                  back_lpitch;

extern int window_client_x0;
extern int window_client_y0;

extern int dd_pixel_format;

extern HWND main_window_handle;
extern HINSTANCE main_instance;

extern UCHAR keyboard_state[256];
extern DIMOUSESTATE mouse_state;

extern int min_clip_x, min_clip_y, max_clip_x, max_clip_y;

LPDIRECTDRAWSURFACE7 DDraw_Create_Surface(int width, int height, int mem_flags = 0, USHORT color_key_value = 0);
int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds, USHORT color, RECT *client = NULL);
int DDraw_Init(int width, int height, int bpp, int windowed); //初始化ddraw
int DInput_Init(); //初始化dinput
int DInput_Init_Keyboard(); //初始化keyboard
int DInput_Init_Mouse();//初始化鼠标

void DInput_Release_Mouse();//释放鼠标
void DInput_Release_Keyboard(); //释放键盘
void DInput_Shutdown();//释放输入
int DDraw_Shutdown();//释放ddraw

int Draw_Text_GDI(char *text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);
int DDraw_Unlock_Back_Surface();
UCHAR *DDraw_Lock_Back_Surface();
int DDraw_Flip(void);
int DInput_Read_Keyboard();
int DInput_Read_Mouse(void);
int Clip_Line(int &x1, int &y1, int &x2, int &y2);
int Draw_Clip_Line(int x0, int y0, int x1, int y1, int color,UCHAR *dest_buffer, int lpitch);
int Draw_Line(int x0, int y0, int x1, int y1, int color, UCHAR *vb_start, int lpitch);
DWORD Get_Clock(void);
DWORD Start_Clock(void);
DWORD Wait_Clock(DWORD count);
//16位内存填充
inline void Mem_Set_WORD(void *dest, USHORT data, int count)
{
	_asm
	{
		mov edi, dest;
		mov ecx, count;
		mov ax, data;
		rep stosw;
	}
}
//32位内存填充
inline void Mem_Set_QUAD(void *dest, UINT data, int count)
{
	_asm
	{
		mov edi, dest;
		mov ecx, count;
		mov eax, data;
		rep stosd;
	}
}

#endif