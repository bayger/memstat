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
// ChartParentWnd.cpp
// Code for parent window of chart window
//

#include "stdafx.h"
#include "memstat.h"
#include "res/resource.h"


//- GLOBAL VARIABLES
//- ================

HWND     g_hChartParentWnd;		// parent chart window handle


//- PRIVATE VARIABLES
//- =================


static BOOL				m_bDragging;			// chart-wnd dragging flag
static POINT			m_pntStart;				// starting draging point
static RECT				m_rectStart;			// and rect


//- PRIVATE FUNCTIONS
//- =================


/// Handles WM_PAINT message - paints window content
static BOOL OnPaint( HWND hWnd )
{
	PAINTSTRUCT     ps;
	HDC				hDC, hDC2;
	HBITMAP			hbmpDrawArea;
	RECT            rectClient, rectPaint;
	RECT			r;
	HFONT			hFont;
	static LOGFONT  lf = { 14, 0, 0, 0,  FW_BOLD, FALSE, FALSE, FALSE, 
										FONT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
										ANTIALIASED_QUALITY, DEFAULT_PITCH|FF_DONTCARE,
										STR_FONT };
	char			str[512];
	LOGBRUSH		lb;
	HBRUSH			hBr;
	INT				state;

	// get client and paint rect
	GetClientRect( hWnd, &rectClient );
	GetUpdateRect( hWnd, &rectPaint, FALSE );
	FormatRecentSample( g_Config.pszCaptionFormat, str );
	SendMessage( hWnd, WM_SETTEXT, 0, (LPARAM)str );

	// open device contexts
	hDC = BeginPaint( hWnd, &ps );

	hDC2 = hDC;
	hDC = CreateCompatibleDC( hDC2 );
	hbmpDrawArea = CreateCompatibleBitmap( hDC2, rectClient.right, 16 );

	if ( hDC != NULL )
	{
		state = SaveDC( hDC );
		SelectObject( hDC, hbmpDrawArea );

		// caption background
		lb.lbColor = g_Config.crCaptionBg;
		lb.lbStyle = BS_SOLID;
		lb.lbHatch = 0;
		hBr = CreateBrushIndirect( &lb );
		SetRect( &r, 0, 0, rectClient.right, 16 );
		FillRect( hDC, &r, hBr );
		
		// caption text
		hFont = CreateFontIndirect( &lf );
		SelectObject( hDC, hFont );
		SetBkMode( hDC, TRANSPARENT );
		SetTextColor( hDC, g_Config.crCaptionText );
		TextOut( hDC, 2, 1, str, strlen(str) );

		// blit to window
		BitBlt( hDC2, 0, 0,
			rectClient.right, 16,
			hDC, 0, 0,
			SRCCOPY );

		RestoreDC( hDC, state );
		DeleteObject( hFont );
		DeleteObject( hBr );
		DeleteObject( hbmpDrawArea );
		DeleteDC( hDC );
		
		EndPaint( hWnd, &ps );

		return TRUE;
	}	
	else
		return FALSE;
}


/// This function handles WM_CREATE message - initializes window
static DWORD OnCreate( HWND hWnd )
{
	g_hChartParentWnd = hWnd;

	POINT p,s;
	p.x = g_Config.posChartWnd.dwPositionX;
	p.y = g_Config.posChartWnd.dwPositionY;
	s.x = g_Config.posChartWnd.dwWidth;
	s.y = g_Config.posChartWnd.dwHeight;

	if ( g_Config.posChartWnd.dwHeight < 16 )
	{
		SetWindowPos( hWnd, 0, 0, 0, 320, 120, SWP_NOZORDER|SWP_NOMOVE );
		TrayPosWindow( hWnd );
	}
	else
	{
		RECT r;
		SetRect( &r, 0, 0, s.x, s.y );
		SetWindowPos( hWnd, 0, p.x, p.y, s.x, s.y, SWP_NOZORDER );
	}

	UpdateAOTMode();
	UpdateTransparency();

	return MSG_OK;
}


/// Handles WM_LBUTTONDOWN/WM_LBUTTONUP messages - controls the window dragging
static void OnDrag( HWND hWnd, BOOL bStart )
{
	if ( bStart )
	{
		SetCapture( hWnd );
		GetCursorPos( &m_pntStart );
		GetWindowRect( hWnd, &m_rectStart );
		m_bDragging = TRUE;
	}
	else
	{
		m_bDragging = FALSE;
		ReleaseCapture();
	}
}


/// Handles WM_MOUSEMOVE message - drags window if LBM down
static void OnMouseMove( HWND hWnd )
{
	POINT pnt;

	if ( m_bDragging )
	{
		GetCursorPos( &pnt );
		SetWindowPos( hWnd, 0, 
			m_rectStart.left+(pnt.x-m_pntStart.x), m_rectStart.top+(pnt.y-m_pntStart.y),
			0, 0, SWP_NOSIZE | SWP_NOZORDER );
	}
}


/// Handles WM_RBUTTONUP message - toggles windowshade mode
static void OnRButtonUp()
{
	RECT r,q;

	if ( g_Config.dwChartState & CWS_WINDOWSHADE )
	{
		// restore to oryginal size; windowshade: off
		g_Config.dwChartState &= (0xffffffff-CWS_WINDOWSHADE);
		SetWindowLong( g_hChartParentWnd, GWL_STYLE, WS_POPUP|WS_THICKFRAME|WS_VISIBLE );
		SetWindowPos( g_hChartParentWnd, 0, g_Config.posChartWnd.dwPositionX, g_Config.posChartWnd.dwPositionY, g_Config.posChartWnd.dwWidth, g_Config.posChartWnd.dwHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_FRAMECHANGED );
	}
	else
	{
		// caption only; windowshade: on; no sizing
		g_Config.dwChartState |= CWS_WINDOWSHADE;
		GetWindowRect( g_hChartParentWnd, &q );
		SetRect( &r, 0, 0, g_Config.posChartWnd.dwWidth, 16 );
		AdjustWindowRect( &r, WS_POPUP|WS_DLGFRAME, FALSE );
		SetWindowLong( g_hChartParentWnd, GWL_STYLE, WS_POPUP|WS_DLGFRAME|WS_VISIBLE );
		SetWindowPos( g_hChartParentWnd, 0, g_Config.posChartWnd.dwPositionX, g_Config.posChartWnd.dwPositionY, q.right-q.left, r.bottom-r.top, SWP_NOZORDER|SWP_FRAMECHANGED );
		
	}
}


/// Callback message procedure for chart parent window
LONG CALLBACK MemChartParentWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	RECT r;
	BOOL bProcessed;

	bProcessed = FALSE;
	switch ( msg )
	{
		// messages partialy handled here:
		case WM_PAINT:
			bProcessed = OnPaint( hWnd );
			break;

		case WM_CREATE:
			return OnCreate( hWnd );

		case WM_RBUTTONUP:
			OnRButtonUp();
			break;

		case WM_LBUTTONDOWN:
			OnDrag( hWnd, TRUE );
			break;

		case WM_LBUTTONUP:
			OnDrag( hWnd, FALSE );
			break;

		case WM_MOUSEMOVE:
			OnMouseMove( hWnd );
			break;

		case WM_MOVE:
			WINDOWPLACEMENT wpl;
			wpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement( hWnd, &wpl );
			g_Config.posChartWnd.dwPositionX = wpl.rcNormalPosition.left; 
			g_Config.posChartWnd.dwPositionY =  wpl.rcNormalPosition.top;
			break;

		case WM_SIZING:
			SetRect( &r, 0, 0, 64, 64 );
			AdjustWindowRect( &r, WS_POPUP|WS_THICKFRAME, FALSE );
			LONG dwMinWidth, dwMinHeight;
			LPRECT q;
			dwMinWidth = r.right-r.left;
			dwMinHeight = r.bottom-r.top;
			q = (LPRECT)lParam;
			if ( q->right-q->left < dwMinWidth )
			{
				if ( wParam == WMSZ_LEFT || wParam == WMSZ_TOPLEFT || wParam == WMSZ_BOTTOMLEFT )
					q->left = q->right-dwMinWidth;
				else
					q->right = q->left+dwMinWidth;
			}
			if ( q->bottom-q->top < dwMinHeight )
			{
				if ( wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT )
					q->top = q->bottom-dwMinHeight;
				else
					q->bottom = q->top+dwMinHeight;
			}
			bProcessed = TRUE;
			break;

		case WM_SIZE:
			GetWindowRect( hWnd, &r );
			if ( (g_Config.dwChartState & CWS_WINDOWSHADE) == 0 )
			{
				g_Config.posChartWnd.dwPositionX = r.left;
				g_Config.posChartWnd.dwPositionY = r.top;
				g_Config.posChartWnd.dwWidth = r.right-r.left;
				g_Config.posChartWnd.dwHeight = r.bottom-r.top;
				SetWindowPos( g_hChartWnd, 0, 0, 16, LOWORD(lParam), HIWORD(lParam)-16, SWP_NOZORDER );
			}
			else
			{
				LONG dwHeight;
				SetRect( &r, 0, 0, 16, 16 );
				AdjustWindowRect( &r, WS_DLGFRAME|WS_POPUP, FALSE );
				dwHeight = r.bottom-r.top;
				GetWindowRect( hWnd, &r );
				if ( r.bottom-r.top != dwHeight )
					SetWindowPos( g_hChartParentWnd, 0, 0, 0, r.right-r.left, dwHeight, SWP_NOZORDER|SWP_NOMOVE );
			}
			break;

		case WM_SHOWWINDOW:
			if ( wParam )
				g_Config.dwChartState |= CWS_VISIBLE;
			else
				g_Config.dwChartState &= (0xffffffff-CWS_VISIBLE);
			break;

		case WM_HOTKEY:
			if ( wParam == HK_TOGGLECHART )
			{
				ShowWindow( hWnd, IsWindowVisible( hWnd ) ? SW_HIDE : SW_SHOW );
				if ( IsWindowVisible( hWnd ) )
					SetForegroundWindow( hWnd );
			}
			break;

		case WM_CLOSE:
			DestroyWindow( g_hMainWnd );
			break;
	}

	if ( !bProcessed )
		return DefWindowProc( hWnd, msg, wParam, lParam );
	else
		return 0;
}


//- PUBLIC VARIABLES
//- ================


/// Updates always-on-top mode (from configuration)
void UpdateAOTMode()
{
	SetWindowPos( g_hChartParentWnd,
		g_Config.bAlwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
		0, 0, 0, 0,
		SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );
}


/// Changes chart-window transparency on/off and factor
void UpdateTransparency()
{
	if ( SetLayeredWindowProperties_USER32 != NULL )
	{
		DWORD newExStyle = GetWindowLong( g_hChartParentWnd, GWL_EXSTYLE);
		newExStyle = g_Config.bBlendChartWnd ? WS_EX_LAYERED|newExStyle : newExStyle&(0xffffffff-WS_EX_LAYERED);
		SetWindowLong( g_hChartParentWnd, GWL_EXSTYLE, newExStyle );
		SetLayeredWindowProperties_USER32( g_hChartParentWnd, 0, (BYTE)(255-g_Config.dwBlendFactor), LWA_ALPHA );
	}
}


/// Creates parent window for chart window
void CreateChartParentWnd()
{
	WNDCLASS wc;

	memset( &wc, 0, sizeof(wc) );
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MemChartParentWndProc;
	wc.hIcon = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_MEM) );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.lpszClassName = "MemoryChartParentWnd";
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	if ( !RegisterClass( &wc ) )
		ExitError( STR_CANT_CREATE_CHART );

	CreateWindowEx( WS_EX_TOOLWINDOW, "MemoryChartParentWnd",
		"MemStat XP Chart",
		WS_POPUP | ((g_Config.dwChartState & CWS_WINDOWSHADE)!=0 ? WS_DLGFRAME : WS_THICKFRAME),
		0,
		0,
		320, 16,
		g_hMainWnd,
		NULL,
		g_hInstance,
		NULL );

	if ( g_hChartParentWnd == NULL )
		ExitError( STR_CANT_CREATE_CHART );

	// create child windows - chart
	CreateChartWnd();

	ShowWindow( g_hChartParentWnd, (g_Config.dwChartState & CWS_VISIBLE) ? SW_SHOW : SW_HIDE );
	SetClassLong( g_hChartParentWnd, GCL_HICON, (LONG)LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_MEM) ) );
}