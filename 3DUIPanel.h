#ifndef _3DUIPANEL_H
#define _3DUIPANEL_H

#include "3DLib2.h"

typedef struct UI_PAENL_TYP
{
	RECT rect; //panelλ��
	BITMAP_IMG_PTR texture; //����
	int visible; //����
	UINT zorder; //z����
}UI_PANEL, *UI_PANEL_PTR;

void CreateUIPanel(UI_PANEL_PTR uipanel, RECT rect, const char* respath, UINT zorder = 0);
void DrawUIPanel(UI_PANEL_PTR uipanel, unsigned char *_dest_buffer, int mempitch);
void LoadLoadingPanel(UI_PANEL_PTR uipanel);
void LoadSettingPanel(UI_PANEL_PTR uipanel);
void LoadBackPanel(UI_PANEL_PTR uipanel);
void ShowLoading(UI_PANEL_PTR uipanel);
#endif