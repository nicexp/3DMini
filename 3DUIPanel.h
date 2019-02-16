#ifndef _3DUIPANEL_H
#define _3DUIPANEL_H

#include "3DLib2.h"

typedef struct UI_PAENL_TYP
{
	RECT rect; //panelŒª÷√
	BITMAP_IMG_PTR texture; //Œ∆¿Ì
	int visible; //œ‘“˛
	UINT zorder; //z≈≈–Ú
}UI_PANEL, *UI_PANEL_PTR;

void CreateUIPanel(UI_PANEL_PTR uipanel, RECT rect, const char* respath, UINT zorder = 0);
void DrawUIPanel(UI_PANEL_PTR uipanel, unsigned char *_dest_buffer, int mempitch);
void LoadLoadingPanel(UI_PANEL_PTR uipanel);
void LoadSettingPanel(UI_PANEL_PTR uipanel);
void LoadBackPanel(UI_PANEL_PTR uipanel);
void ShowLoading(UI_PANEL_PTR uipanel);
#endif