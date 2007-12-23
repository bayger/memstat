/*
** Copyright (C) 1999-2002 Patryk Bajer (a.k.a. Bayger)
**	
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// cfgappearance.cpp
// Code of 'Appearance' configuration page
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE VARIABLES
//- =================


static char*		m_pIconStyles[] = STR_ICON_STYLES }; // icon style names


//- PRIVATE FUNCTIONS
//- =================


/// Displays color dialog box
static void ChangeColor( LPCOLORREF lpColor, WORD wID, LPSTR lpszCaption )
{
	CHOOSERGB crgb = { CRGB_INITCOLOR | CRGB_PREVENTHEX | CRGB_REALTIME,
		lpszCaption,
		lpColor,
		0,
		0,
		g_hMainWnd };

	ChooseRGB( &crgb );
	RedrawDlgItem( g_hwcpAppearance, wID );
}


/// Updates hotkey that toggles the chart window
static void UpdateChartHotkey()
{
	WORD wVKey = LOBYTE(g_Config.dwChartHotkey);
	WORD wMods = HIBYTE(g_Config.dwChartHotkey);
	RegisterHotKey( g_hChartParentWnd, HK_TOGGLECHART, wMods, wVKey );
}


/// Refreshes dialog controls when config has changed
static void OnConfigChange( HWND hDlg )
{
	char str[256];
	WORD wID;

	// set up all dialog controls
	sprintf( str, "%u", g_Config.dwInterval );
	SDIM( hDlg, IDC_INTERVAL, WM_SETTEXT, 0, (LPARAM)str );
	SDIM( hDlg, IDC_AOT, BM_SETCHECK, g_Config.bAlwaysOnTop ? BST_CHECKED : BST_UNCHECKED, 0 );
	UpdateAOTMode();
	SDIM( hDlg, IDC_AUTOSCALE, BM_SETCHECK, g_Config.bAutoYScale ? BST_CHECKED : BST_UNCHECKED, 0 );
	
	// transparency stuff (chk if available)
	if ( SetLayeredWindowProperties_USER32 != NULL )
	{
		SDIM( hDlg, IDC_CW_BLEND, BM_SETCHECK, g_Config.bBlendChartWnd ? BST_CHECKED : BST_UNCHECKED, 0 );
		SDIM( hDlg, IDC_BLEND_FACTOR, TBM_SETRANGE, TRUE, MAKELONG(0,255) );
		SDIM( hDlg, IDC_BLEND_FACTOR, TBM_SETPOS, TRUE, g_Config.dwBlendFactor );
	}
	else
	{
		EnableWindow( GetDlgItem(hDlg,IDC_CW_BLEND), FALSE );
		EnableWindow( GetDlgItem(hDlg,IDC_BLEND_FACTOR), FALSE );
		EnableWindow( GetDlgItem(hDlg,IDC_BLEND_PERC), FALSE );
	}

	switch ( g_Config.dwTrayIconStyle )
	{
		case TI_FREEBAR:    wID = IDC_ICONBAR; break;
		case TI_MINICHART:  wID = IDC_ICONCHART; break;
		case TI_NUMERICAL:	wID = IDC_ICONNUM; break;
		case TI_NUMERICAL2:	wID = IDC_ICONNUM2; break;
		default:            wID = IDC_ICONSTD;
	}
	SDIM( hDlg, IDC_ICONSTYLE, CB_SETCURSEL, g_Config.dwTrayIconStyle, 0 );
	SDIM( hDlg, IDC_BUSY_ICON, BM_SETCHECK, g_Config.bBusyIcon ? BST_CHECKED : BST_UNCHECKED, 0 );

	// setup hotkey control
	SDIM( hDlg, IDC_HOTKEYCHART, HKM_SETRULES, 
		HKCOMB_NONE|HKCOMB_S|HKCOMB_A|HKCOMB_SA|HKCOMB_SCA|HKCOMB_CA|HKCOMB_SC,
		MAKELPARAM(HOTKEYF_CONTROL,0) );
	SDIM( hDlg, IDC_HOTKEYCHART, HKM_SETHOTKEY, g_Config.dwChartHotkey, 0 );
	UpdateChartHotkey();
}


/// Handle track-bar notification messages
static void OnHScroll( HWND hDlg, WORD wNotifyCode, WORD wPos, WORD wID )
{
	char str[256];
	DWORD dwBlendFactor;

	switch ( wID )
	{
		case IDC_BLEND_FACTOR:
		// 'blend-factor' tracking
			dwBlendFactor = SDIM( hDlg, IDC_BLEND_FACTOR, TBM_GETPOS, 0, 0 );
			g_Config.dwBlendFactor = dwBlendFactor;
			sprintf( str, "%.0f%%", 100.0f * dwBlendFactor / 255.0f );
			SDIM( hDlg, IDC_BLEND_PERC, WM_SETTEXT, 0, (LPARAM)str );
			UpdateTransparency();
			break;
	}
}


/// Handles WM_INITDIALOG message - initializes page
static void OnInitDialog( HWND hDlg )
{
	// icon styles
	for( int i=0; i<STR_ICON_STYLES_CNT; i++ )
		SDIM( hDlg, IDC_ICONSTYLE, CB_ADDSTRING, 0, (LPARAM)m_pIconStyles[i] );
	OnConfigChange( hDlg );
	OnHScroll( hDlg, 0, 0, IDC_BLEND_FACTOR );
}


/// Draws color-buttons
static void OnDrawItem( HWND hDlg, WORD wID, LPDRAWITEMSTRUCT lpDrawItem )
{
	HWND hwndCtl;
	HBRUSH hbrColor;
	LOGBRUSH lb;
	HPEN hpenHilite;
	HPEN hpenShadow;
	RECT rect;
	BOOL bPushed;
	int iSaved;

	hwndCtl = GetDlgItem( hDlg, wID );
	GetClientRect( hwndCtl, &rect );
	lb.lbStyle = BS_SOLID;
	lb.lbHatch = 0;
	switch ( wID )
	{
		case IDC_BGCOLOR:		lb.lbColor = g_Config.crBackground; break;
		case IDC_FGCOLOR:		lb.lbColor = g_Config.crChart; break;
		case IDC_LINECOLOR:		lb.lbColor = g_Config.crLines; break;
		case IDC_TEXTCOLOR:		lb.lbColor = g_Config.crText; break;
		case IDC_BGCAPTION:		lb.lbColor = g_Config.crCaptionBg; break;
		case IDC_TEXTCAPTION:	lb.lbColor = g_Config.crCaptionText; break;
		default:        return;
	}
	bPushed = ( lpDrawItem->itemState & ODS_SELECTED );

	// creating GDI resources
	LOGBRUSH lbr;
	lbr.lbColor = GetSysColor(COLOR_3DHILIGHT);
	lbr.lbStyle = BS_SOLID;
	lbr.lbHatch = 0;
	hbrColor = CreateBrushIndirect( &lb ); //  GetSysColor(COLOR_3DHILIGHT)
	hpenHilite = ExtCreatePen( PS_GEOMETRIC|PS_SOLID|PS_ENDCAP_SQUARE, 2, &lbr, 0, 0 );
	lbr.lbColor = GetSysColor(COLOR_3DSHADOW);
	hpenShadow = ExtCreatePen( PS_GEOMETRIC|PS_SOLID|PS_ENDCAP_SQUARE, 2, &lbr, 0, 0 );
	iSaved = SaveDC( lpDrawItem->hDC );

	// drawing colored button
	FillRect( lpDrawItem->hDC, &rect, hbrColor );

	if ( bPushed )
		SelectObject( lpDrawItem->hDC, hpenShadow );
	else
		SelectObject( lpDrawItem->hDC, hpenHilite );
	MoveToEx( lpDrawItem->hDC, rect.left+1, rect.bottom, NULL );
	LineTo( lpDrawItem->hDC, rect.left+1, rect.top+1 );
	LineTo( lpDrawItem->hDC, rect.right-1, rect.top+1 );

	if ( !bPushed )
		SelectObject( lpDrawItem->hDC, hpenShadow );
	else
		SelectObject( lpDrawItem->hDC, hpenHilite );
	MoveToEx( lpDrawItem->hDC, rect.right-1, rect.top, NULL );
	LineTo( lpDrawItem->hDC, rect.right-1, rect.bottom-1 );
	LineTo( lpDrawItem->hDC, rect.left, rect.bottom-1 );

	if ( lpDrawItem->itemState & ODS_FOCUS )
		FrameRect( lpDrawItem->hDC, &rect, (HBRUSH)GetSysColorBrush( COLOR_BTNTEXT) );

	// release GDI resources
	RestoreDC( lpDrawItem->hDC, iSaved );
	DeleteObject( hpenShadow );
	DeleteObject( hpenHilite );
	DeleteObject( hbrColor );
}


/// Handles all dialog controls commands sent by WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	WORD wVal;

	switch ( wID )
	{
		case IDC_CAPTIONFORMAT:
			if ( wNotifyCode == BN_CLICKED )
				if ( EditFormat( g_Config.pszCaptionFormat, STR_EDIT_CAPTION_FORMAT ) )
					UpdateMemStats();
			break;

		case IDC_TOOLTIPFORMAT:
			if ( wNotifyCode == BN_CLICKED )
				EditFormat( g_Config.pszTooltipFormat, STR_EDIT_TOOLTIP_FORMAT );
			break;

		case IDC_AOT:
			g_Config.bAlwaysOnTop = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			UpdateAOTMode();
			break;
			
		case IDC_AUTOSCALE:
			if ( wNotifyCode == BN_CLICKED )
				g_Config.bAutoYScale = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_BGCOLOR:
			if ( wNotifyCode == BN_CLICKED )
				ChangeColor( &g_Config.crBackground, wID, STR_SET_BG_COLOR );
			break;

		case IDC_FGCOLOR:
			if ( wNotifyCode == BN_CLICKED )
				ChangeColor( &g_Config.crChart, wID, STR_SET_CHART_COLOR );
			break;

		case IDC_LINECOLOR:
			if ( wNotifyCode == BN_CLICKED )
				ChangeColor( &g_Config.crLines, wID, STR_SET_LINE_COLOR );
			break;

		case IDC_TEXTCOLOR:
			if ( wNotifyCode == BN_CLICKED )
				ChangeColor( &g_Config.crText, wID, STR_SET_TEXT_COLOR );
			break;

		case IDC_BGCAPTION:
			if ( wNotifyCode == BN_CLICKED )
				ChangeColor( &g_Config.crCaptionBg, wID, STR_SET_BG_CAPTION );
			break;

		case IDC_TEXTCAPTION:
			if ( wNotifyCode == BN_CLICKED )
				ChangeColor( &g_Config.crCaptionText, wID, STR_SET_TEXT_CAPTION );
			break;

		case IDC_ICONSTYLE:
			if ( wNotifyCode == CBN_SELCHANGE )
			{
				g_Config.dwTrayIconStyle = SDIM( hDlg, IDC_ICONSTYLE, CB_GETCURSEL, 0, 0 );
				// repaint icon (static flag reset)
				g_dwLastIconValue = 0xffffffff;
				UpdateMemStats();
			}
			break;

		case IDC_HOTKEYCHART:
			if ( wNotifyCode == EN_CHANGE )
			{
				// set global hotkey
				wVal = (WORD)SDIM( hDlg, IDC_HOTKEYCHART, HKM_GETHOTKEY, 0, 0 );
				g_Config.dwChartHotkey = wVal;
				UpdateChartHotkey();
			}
			break;

		case IDC_CW_BLEND:
			if ( wNotifyCode == BN_CLICKED )
			{
				g_Config.bBlendChartWnd = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
				UpdateTransparency();
			}
			break;

		case IDC_BUSY_ICON:
			if ( wNotifyCode == BN_CLICKED )
				g_Config.bBusyIcon = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

	}

	// redraw chart window if needed
	if ( wNotifyCode == BN_CLICKED )
	{
		switch ( wID )
		{
			case IDC_AUTOSCALE:
			case IDC_BGCOLOR:
			case IDC_FGCOLOR:
			case IDC_LINECOLOR:
			case IDC_TEXTCOLOR:
			case IDC_BGCAPTION:
			case IDC_TEXTCAPTION:
			case IDC_RESTORE:
				RedrawChartWnd();
				break;
		}
	}

}


/// 'Appearance' page window procedure
static BOOL CALLBACK CfgAppearanceProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			OnInitDialog( hDlg );
			return TRUE;
	
		case WM_COMMAND:
			if ( lParam != NULL )
				OnCtrlCommand( hDlg, HIWORD(wParam), LOWORD(wParam), (HWND)lParam );
			break;

		case WM_DRAWITEM:
			OnDrawItem( hDlg, (WORD)wParam, (LPDRAWITEMSTRUCT)lParam );
			break;

		case WM_RBUTTONUP:
			HMENU hMenu;
			DWORD idCommand;
			POINT p;
			hMenu = LoadMenu( g_hInstance, MAKEINTRESOURCE(IDR_APPEARANCE) );
			GetCursorPos( &p );
			idCommand = TrackPopupMenu( GetSubMenu( hMenu, 0 ), TPM_NONOTIFY|TPM_RETURNCMD, p.x, p.y, 0, hDlg, NULL );
			if ( idCommand == ID_SYSCOLORS )
			{
				g_Config.crCaptionBg = GetSysColor( COLOR_BTNFACE );
				g_Config.crCaptionText = GetSysColor( COLOR_BTNTEXT );
				g_Config.crBackground = GetSysColor( COLOR_BTNSHADOW );
				g_Config.crChart = GetSysColor( COLOR_BTNTEXT );
				g_Config.crLines = GetSysColor( COLOR_BTNFACE );
				g_Config.crText = GetSysColor( COLOR_BTNHIGHLIGHT );
				RedrawDlgItem( hDlg, IDC_BGCOLOR );
				RedrawDlgItem( hDlg, IDC_TEXTCOLOR );
				RedrawDlgItem( hDlg, IDC_BGCAPTION );
				RedrawDlgItem( hDlg, IDC_TEXTCAPTION );
				RedrawDlgItem( hDlg, IDC_LINECOLOR );
				RedrawDlgItem( hDlg, IDC_COLOR );
				RedrawChartWnd();
			}
			break;

		case WM_CONFIGCHANGE:
			// repaint icon (static flag reset)
			g_dwLastIconValue = 0xffffffff;
			OnConfigChange( hDlg );
			break;

		case WM_HSCROLL:
			OnHScroll( hDlg, LOWORD(wParam), HIWORD(wParam), GetDlgCtrlID((HWND)lParam) );
			break;
	}
	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


// Creates 'Appearance' configuration page
HWND CreateCfgAppearance( HWND hwndTab, HWND hwndTabOwner )
{
	TCITEM tci;
	HWND hWnd;

	// create and position
	hWnd = CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_CFG_APPEARANCE), 
		hwndTabOwner, CfgAppearanceProc );

	if ( hWnd == NULL )
		return ExitError( STR_CANT_CREATE_DIALOG_PAGE ), NULL;

	// add tab control item
	memset( &tci, 0, sizeof(tci) );
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.pszText = STR_TAB_APPEARANCE;
	tci.lParam = (LPARAM)hWnd;
	SendMessage( hwndTab, TCM_INSERTITEM, 0, (LPARAM)&tci );
	
	return hWnd;
}