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
// chartwnd.cpp
// Code of window that shows chart
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- GLOBAL VARIABLES
//- ================


HWND					g_hChartWnd;			// mem-chart window handle



//- PRIVATE VARIABLES
//- =================


static BOOL				m_bDragging;			// chart-wnd dragging flag
static POINT			m_pntStart;				// starting draging point
static RECT				m_rectStart;			// and rect
static HMENU			m_hmenuPopup;			// pop-up menu handle
static POINT			m_pntSize;				// oryginal size of the window


//- PRIVATE FUNCTIONS
//- =================


/// Handles WM_PAINT message - draws memory usage chart
static BOOL OnPaint( HWND hWnd )
{
	char            str[32];
	PAINTSTRUCT     ps;
	RECT            rectClient, rectPaint;
	HDC             hDC, hDC2;
	HPEN            hpenHLines, hpenChart;
	LOGBRUSH        lb;
	HBRUSH          hbrBg;
	HBITMAP         hbmpDrawArea;
	HFONT           hFont;
	static LOGFONT  lf = { 12, 5, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, 
										ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
										ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE,
										"Small Fonts" };
	DWORD           minY, maxY;
	DWORD           dwWidth, dwHeight;
	MEMSTATITEM*	lpMemStats;
	LPMEMSTATITEM	pCurItem;
	DWORD           dwMemStatsCount, dwMemStatsCur, dwNext;
	float           scaleY;

	WINDOWPLACEMENT wpl;
	GetWindowPlacement( hWnd, &wpl );
	if ( wpl.showCmd == SW_HIDE || GetUpdateRect( hWnd, NULL, FALSE ) == 0 )
		return TRUE;

	GetClientRect( hWnd, &rectClient );
	GetUpdateRect( hWnd, &rectPaint, FALSE );

	// open device contexts
	hDC = BeginPaint( hWnd, &ps );

		// clean drawing always: ON
		hDC2 = hDC;
		hDC = CreateCompatibleDC( hDC2 );
		hbmpDrawArea = CreateCompatibleBitmap( hDC2, rectClient.right, rectClient.bottom );
		SelectObject( hDC, hbmpDrawArea );

	if ( hDC != NULL )
	{
		// lock mem-stat data before painting
		lpMemStats = LockMSData( &dwMemStatsCount, &dwMemStatsCur );
		if ( ( dwNext = dwMemStatsCur+1 ) >= dwMemStatsCount )
			dwNext = 0;

		// creating GDI resources GetVer
		hpenChart = CreatePen( PS_SOLID, 1, g_Config.crChart );
		hpenHLines = CreatePen( PS_SOLID, 1, g_Config.crLines );
		hFont = CreateFontIndirect( &lf );
		lb.lbColor = g_Config.crBackground;
		lb.lbStyle = BS_SOLID;
		lb.lbHatch = 0;
		hbrBg = CreateBrushIndirect( &lb );

		dwWidth = rectClient.right - rectClient.left;

		// scaning data & setting scaling factors
		int n = dwMemStatsCur;
		minY = 0;
		if ( g_Config.bAutoYScale )
		{
			maxY = 1;//g_msMemInfo.dwTotalPhys;

			for( int x = dwWidth-1; x >= 0; x-- )
			{
				pCurItem = &lpMemStats[n];
				if ( pCurItem->dwAvailPhys > maxY )
					maxY = pCurItem->dwAvailPhys;
				if (--n < 0) n = dwMemStatsCount-1;
			}
/*

			for ( i = 0; i < dwMemStatsCount; i++ )
				if ( lpMemStats[i].dwAvailPhys > maxY )
					maxY = lpMemStats[i].dwAvailPhys;
*/
			maxY = (DWORD)(maxY * 1.03f);
		}
		else
			maxY = g_msMemInfo.dwTotalPhys;

		// calc y-scale
		scaleY = (float)(dwHeight = rectClient.bottom - rectClient.top-1) / (maxY - minY);

		if ( dwWidth > dwMemStatsCount ) dwWidth = dwMemStatsCount;

		// clearing window
		FillRect( hDC, &rectPaint, hbrBg );

		// drawing lines
		SelectObject( hDC, hpenHLines );
		MoveToEx( hDC, 0, dwHeight / 2, NULL );
		LineTo( hDC, dwWidth, dwHeight / 2 );
		MoveToEx( hDC, 0, dwHeight / 4, NULL );
		LineTo( hDC, dwWidth, dwHeight / 4 );
		MoveToEx( hDC, 0, dwHeight*3 / 4, NULL );
		LineTo( hDC, dwWidth, dwHeight*3 / 4 );

		// drawing chart
		int y;
		SelectObject( hDC, hpenChart );
		n = dwMemStatsCur;
		MoveToEx( hDC, dwWidth, (int)(rectClient.bottom - lpMemStats[n].dwAvailPhys*scaleY - 1), NULL );
		for( int x = dwWidth-1; x >= 0; x-- )
		{
			pCurItem = &lpMemStats[n];
			y = (int)(rectClient.bottom - pCurItem->dwAvailPhys*scaleY - 1);
			LineTo( hDC, x, y );
			if (--n < 0) n = dwMemStatsCount-1;
		}
		LineTo( hDC, -1, y );
	
		// draw scale labels
		SelectObject( hDC, hFont );
		SetTextColor( hDC, g_Config.crText );
		SetBkMode( hDC, TRANSPARENT );
		sprintf( str, "%.2f MB", maxY / (float)MB );
		TextOut( hDC, 1, 0, str, strlen(str) );
		sprintf( str, "%.2f MB", maxY*0.75f / (float)MB );
		TextOut( hDC, 1, dwHeight / 4, str, strlen(str) );
		sprintf( str, "%.2f MB", maxY*0.5f / (float)MB );
		TextOut( hDC, 1, dwHeight / 2, str, strlen(str) );
		sprintf( str, "%.2f MB", maxY*0.25f / (float)MB );
		TextOut( hDC, 1, dwHeight*3 / 4, str, strlen(str) );

		// unlock mem-stat data and release GDI resources
		UnlockMSData();
		SelectObject( hDC, GetStockObject( BLACK_PEN ) );
		SelectObject( hDC, GetStockObject( SYSTEM_FONT ) );

		// clean drawing always: ON
			// oh and don't forget to blit it to window :)
			// (for Clean Drawing mode)
			BitBlt( hDC2, 0, 0,
				rectClient.right, rectClient.bottom,
				hDC, 0, 0,
				SRCCOPY );
			DeleteDC( hDC );
			DeleteObject( hbmpDrawArea );

		EndPaint( hWnd, &ps );
		DeleteObject( hbrBg );
		DeleteObject( hpenChart );
		DeleteObject( hpenHLines );
		DeleteObject( hFont );

		return TRUE;
	}
	else
		return FALSE;
}


/// Handles all menu commands (tray-menu)
static BOOL OnMenuSelect( HWND hWnd, WORD wID )
{
	switch ( wID )
	{
		case ID_MOVE2TRAY:
			// move chart window near system tray
			TrayPosWindow( g_hChartParentWnd );
			break;

		case ID_AUTOYSCALE:
			// update auto-y-scale flag (also redraw config control)
			g_Config.bAutoYScale = !g_Config.bAutoYScale;
			SDIM( g_hwcpAppearance, IDC_AUTOSCALE, BM_SETCHECK, 
				g_Config.bAutoYScale ? BST_CHECKED : BST_UNCHECKED, 0 );
			RedrawWindow( g_hChartWnd, NULL, NULL, RDW_INVALIDATE );
			break;

		case ID_AOT:
			// toggle always-on-top mode
			g_Config.bAlwaysOnTop = !g_Config.bAlwaysOnTop;
			SDIM( g_hwcpAppearance, IDC_AOT, BM_SETCHECK, 
				g_Config.bAlwaysOnTop ? BST_CHECKED : BST_UNCHECKED, 0 );
			UpdateAOTMode();
			break;

		case ID_CONFIG:
			// show configuration window
			ShowWindow( g_hMainWnd, SW_SHOW );
			SetForegroundWindow( g_hMainWnd );
			break;

		case ID_HIDE:
			// hide chart-window
			ShowWindow( g_hChartParentWnd, SW_HIDE );
			break;

		case ID_EXIT:
			// exit application by destroying main wnd
			DestroyWindow( g_hMainWnd );
			break;

		default:
			return FALSE;
	}
	return TRUE;
}


/// Updates pop-up menu before showing
static void UpdatePopupMenu()
{
	MENUITEMINFO mmi;

	memset( &mmi, 0, sizeof(mmi) );
	mmi.cbSize = sizeof(mmi);
	mmi.fMask = MIIM_STATE;
	mmi.fState = MFS_ENABLED | (g_Config.bAlwaysOnTop ? MFS_CHECKED : MFS_UNCHECKED);
	SetMenuItemInfo( m_hmenuPopup, ID_AOT, FALSE, &mmi );
	mmi.fState = MFS_ENABLED | (g_Config.bAutoYScale ? MFS_CHECKED : MFS_UNCHECKED);
	SetMenuItemInfo( m_hmenuPopup, ID_AUTOYSCALE, FALSE, &mmi );
}


/// Handles WM_CREATE message - chart window creation process
static DWORD OnCreate( HWND hWnd )
{
	RECT r;
	g_hChartWnd = hWnd;

	GetClientRect( g_hChartParentWnd, &r );
	SetWindowPos( g_hChartWnd, 0, 0, 16, r.right, r.bottom-16, SWP_NOZORDER );

	m_hmenuPopup = LoadMenu( g_hInstance, MAKEINTRESOURCE(IDR_CHARTMENU) );
	return MSG_OK;
}


/// Memory chart window procedure - message handler
static LONG CALLBACK MemChartWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	POINT posCursor;
	BOOL bProcessed;

	bProcessed = FALSE;
	switch ( msg )
	{
		// messages partialy handled here:
		case WM_PAINT:
			bProcessed = OnPaint( hWnd );
			break;

		case WM_COMMAND:
			if ( HIWORD(wParam) == 0 )
				return OnMenuSelect( hWnd, LOWORD(wParam) );
			break;

		case WM_CREATE:
			return OnCreate( hWnd );

		// messages fully handled here:
		case WM_KEYUP:
			if ( wParam == VK_APPS )
			{
				RECT r;
				GetWindowRect( hWnd, &r );
				UpdatePopupMenu();
				posCursor.x = r.left + (r.right-r.left)/2;
				posCursor.y = r.top + (r.bottom-r.top)/2;
				TrackPopupMenuEx( GetSubMenu( m_hmenuPopup, 0 ), 
					TPM_CENTERALIGN|TPM_VCENTERALIGN, 
					posCursor.x, posCursor.y, hWnd, NULL );
				bProcessed = TRUE;
			}
			break;

		case WM_RBUTTONUP:
			GetCursorPos( &posCursor );
			UpdatePopupMenu();
			TrackPopupMenuEx( GetSubMenu( m_hmenuPopup, 0 ), 0, posCursor.x, posCursor.y, hWnd, NULL );
			bProcessed = TRUE;
			break;

		case WM_CLOSE:
			bProcessed = ShowWindow( hWnd, SW_HIDE );
			break;
	}

	if ( !bProcessed )
		return DefWindowProc( hWnd, msg, wParam, lParam );
	else
		return 0;
}


//- PUBLIC FUNCTIONS
//- ================


// Creates window that shows chart
void CreateChartWnd()
{
	WNDCLASS wc;

	memset( &wc, 0, sizeof(wc) );
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MemChartWndProc;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.lpszClassName = "MemoryChartWnd";
	if ( !RegisterClass( &wc ) )
		ExitError( STR_CANT_CREATE_CHART );

	CreateWindowEx( WS_EX_CLIENTEDGE, "MemoryChartWnd",
		NULL,
		WS_CHILD,//WS_POPUPWINDOW | WS_THICKFRAME,
		0,
		16,
		160, 120,
		g_hChartParentWnd,
		NULL,
		g_hInstance,
		NULL );

	ShowWindow( g_hChartWnd, SW_SHOW );

	if ( g_hChartWnd == NULL )
		ExitError( STR_CANT_CREATE_CHART );
}
