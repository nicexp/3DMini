#include "3DUIPanel.h"
#include "3DBitmap.h"

void CreateUIPanel(UI_PANEL_PTR uipanel, RECT rect, const char* respath, UINT zorder)
{
	uipanel->rect = rect;
	uipanel->zorder = zorder;
	uipanel->visible = 1;

	const char *ext = NULL;
	const char ch = '.';
	char workpath[256];
	GetCurrentDirectory(256, workpath);
	//GetModuleFileName(NULL, exepath, 256);

	uipanel->texture = (BITMAP_IMG_PTR)malloc(sizeof(BITMAP_IMG));

	strcat(workpath, respath);
	ext = strrchr(workpath, ch);
	if (!strcmp(ext + 1, "bmp"))
		Load_Bitmap_File(uipanel->texture, workpath);
	else if (!strcmp(ext + 1, "jpg"))
		Load_Jpeg_File(uipanel->texture, workpath);
	else if (!strcmp(ext + 1, "png"))
		Load_Png_File(uipanel->texture, workpath);
	else
		DEBUG_LOG("uipanel texture is not bmp,jpg,png");
}

void DrawUIPanel(UI_PANEL_PTR uipanel, unsigned char *_dest_buffer, int mempitch)
{
	if (!uipanel->visible)
		return;

	BITMAP_IMG_PTR bitmap = uipanel->texture;
	PRECT rect = &uipanel->rect;

	unsigned int* dest_buffer = (unsigned int*)_dest_buffer;
	unsigned int* dest_addr = NULL;
	mempitch >>= 2;

	int ys, ye, xs, xe;
	ys = rect->top;
	ye = rect->bottom;
	xs = rect->left;
	xe = rect->right;

	if (ys < min_clip_y)
		ys = min_clip_y;
	if (ye > max_clip_y)
		ye = max_clip_y;
	if (xs < min_clip_x)
		xs = min_clip_x;
	if (xe > max_clip_x)
		xe = max_clip_x;

	dest_addr = dest_buffer + ys * mempitch;

	for (int yi = ys; yi <= ye; yi++, dest_addr += mempitch)
	{
		int dy = yi - rect->top;
		for (int xi = xs; xi <= xe; xi++)
		{
			int dx = xi - rect->left;
			*(dest_addr + xi) = ((UINT*)bitmap->buffer)[dy * bitmap->width + dx];
		}
	}
}

void LoadLoadingPanel(UI_PANEL_PTR uipanel)
{
	int panel_width = WINDOW_WIDTH;
	int panel_height = WINDOW_HEIGHT;
	RECT rt = { 0, 0, panel_width - 1, panel_height - 1 };
	CreateUIPanel(uipanel, rt, "//UI//loading.jpg");
}

void LoadSettingPanel(UI_PANEL_PTR uipanel)
{
	int panel_width = 400;
	int panel_height = 288;
	int left = (WINDOW_WIDTH - panel_width) / 2;
	int top = (WINDOW_HEIGHT - panel_height) / 2;
	RECT rt = { left, top, left + panel_width - 1, top + panel_height - 1 };
	CreateUIPanel(uipanel, rt, "//UI//setting.jpg");
}

void LoadBackPanel(UI_PANEL_PTR uipanel)
{
	int panel_width = WINDOW_WIDTH;
	int panel_height = WINDOW_HEIGHT;
	int left = (WINDOW_WIDTH - panel_width) / 2;
	int top = (WINDOW_HEIGHT - panel_height) / 2;
	RECT rt = { left, top, left + panel_width - 1, top + panel_height - 1 };
	CreateUIPanel(uipanel, rt, "//UI//background.bmp");
}

void ShowLoading(UI_PANEL_PTR uipanel)
{
	DDraw_Fill_Surface(lpddsback, 0);
	DDraw_Lock_Back_Surface();
	DrawUIPanel(uipanel, back_buffer, back_lpitch);
	DDraw_Unlock_Back_Surface();
	DDraw_Flip();
}