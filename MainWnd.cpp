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
// mainwnd.cpp 
// Memory Status main window code
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE VARIABLES
//- =================


static NOTIFYICONDATA		m_nidTrayIcon;			// notification tray icon struct
static HMENU				m_hTrayMenu;			// tray icon popup menu
static HMENU				m_hTrayMenu2;			// tray icon popup menu 2
static HWND					m_hwndTab;				// tab control handle
static HICON				m_hLastMadeIcon;		// last made icon handle
static TOOLINFO				m_tiChart;				// tooltip info for chart
static HBITMAP				m_bmFont = NULL;		// font used in icon
static BOOL					m_bLastState = FALSE;	// last free mem state


//- GLOBAL VARIABLES
//- ================


HWND						g_hMainWnd;             // main window handle
HWND						g_hwndTooltip;          // tooltip control for chart
HWND						g_hwcpFreeing;          // configuration pages
HWND						g_hwcpMonitoring;
HWND						g_hwcpAppearance;
HWND						g_hwcpAdvanced;
HWND						g_hwcpAbout;
HWND						g_hwcpCurrent;          // current page (visible)
DWORD						g_dwLastIconValue = 0xffffffff; // last icon value


//- PRIVATE FUNCTIONS
//- =================


/// Sends message WM_CONFIGCHANGE to specified window
static BOOL CALLBACK Resender( HWND hWnd, LPARAM lParam )
{
	PostMessage( hWnd, WM_CONFIGCHANGE, 0, 0 );
	return TRUE;
}


/// Checks all windows to see if they are inside of desktop boundaries
static void CheckWindows()
{
	RECT rWnd,rDesktop;

	GetWindowRect( GetDesktopWindow(), &rDesktop );
	GetWindowRect( g_hMainWnd, &rWnd );
	if ( (rWnd.bottom < rDesktop.top) || (rWnd.top > rDesktop.bottom ) ||
		(rWnd.left > rDesktop.right) || (rWnd.right < rDesktop.left ) )
		TrayPosWindow( g_hMainWnd );
	GetWindowRect( g_hChartWnd, &rWnd );
	if ( (rWnd.bottom < rDesktop.top) || (rWnd.top > rDesktop.bottom ) ||
		(rWnd.left > rDesktop.right) || (rWnd.right < rDesktop.left ) )
		TrayPosWindow( g_hChartWnd );
}


/// Moves specified window to the tab control
static void MoveToTabArea( HWND hWnd )
{
	// adjusting size and position
	DWORD dwDlgBase = GetDialogBaseUnits();
	int cxMargin = LOWORD(dwDlgBase) / 4; 
	int cyMargin = HIWORD(dwDlgBase) / 8; 
	RECT rc;
	HWND hTabs = GetDlgItem( g_hMainWnd, IDC_CFGTAB );
	GetWindowRect( hTabs, &rc );
	ScreenToClient( g_hMainWnd, (LPPOINT)&rc.left );
	ScreenToClient( g_hMainWnd, (LPPOINT)&rc.right );
	TabCtrl_AdjustRect( hTabs, FALSE, &rc );
	rc.left--;
	SetWindowPos( hWnd, HWND_TOP, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 0 ); 
}


/// Changes current configuration page using HWND
static void SetTabPage( HWND hPageWnd )
{
	int i;
	TCITEM tci = { 0 };

	int cnt = TabCtrl_GetItemCount( m_hwndTab );
	for ( i=0; i<cnt; i++ )
	{
		tci.mask = TCIF_PARAM;
		TabCtrl_GetItem( m_hwndTab, i, &tci );
		if ( hPageWnd == (HWND)tci.lParam )
		{
			TabCtrl_SetCurFocus( m_hwndTab, i );
			break;
		}
	}
}


/// Creates tray icon, style: 'Four Bars'
static BOOL MakeIcon_FourBars()
{
	LPMEMSTATITEM lpMemStat;
	DWORD dwCnt, dwCur;
	ICONINFO ii;
	HBITMAP hbmColor;
	HBITMAP hbmMask;
	HDC hTempDC, hDC;
	LOGBRUSH lb;
	HBRUSH hbrColor;
	RECT r;
	DWORD dwWidth;
	int iState;

	// all init stuff
	BOOL bMemState;
	lpMemStat = LockMSData( &dwCnt, &dwCur );
	dwWidth = (DWORD)(64.0f*lpMemStat[dwCur].dwAvailPhys/lpMemStat[dwCur].dwTotalPhys);
	bMemState = (g_Config.dwMinFree > lpMemStat[dwCur].dwAvailPhys);
	UnlockMSData();

	// do we have to write a new icon?
	if ( g_dwLastIconValue == dwWidth && m_bLastState == bMemState )
		return FALSE;

	// static info update
	g_dwLastIconValue = dwWidth;
	m_bLastState = bMemState;

	// we destroy last made icon first
	if ( m_hLastMadeIcon )
		DestroyIcon( m_hLastMadeIcon );


	hTempDC = CreateDC( "DISPLAY", NULL, NULL, NULL );
	hDC = CreateCompatibleDC( hTempDC );
	hbmColor = CreateCompatibleBitmap( hTempDC, 16, 16 );
	hbmMask = CreateBitmap( 16, 16, 1, 1, NULL );
	lb.lbColor = bMemState ? RGB(255,0,0) : RGB(0,255,0);
	lb.lbStyle = BS_SOLID;
	lb.lbHatch = 0;
	hbrColor = CreateBrushIndirect( &lb );
	iState = SaveDC( hDC );

	// drawing color bitmap
	SelectObject( hDC, hbmColor );
	SetRect( &r, 0, 0, 16, 16 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
	SetRect( &r, 0, 0, dwWidth > 16 ? 16 : dwWidth, 3 );
	FillRect( hDC, &r, hbrColor );
	if ( dwWidth > 16 )
	{
		SetRect( &r, 0, 4, dwWidth > 32 ? 16 : dwWidth-16, 7 );
		FillRect( hDC, &r, hbrColor );
	}
	if ( dwWidth > 32 )
	{
		SetRect( &r, 0, 8, dwWidth > 48 ? 16 : dwWidth-32, 11 );
		FillRect( hDC, &r, hbrColor );
	}
	if ( dwWidth > 48 )
	{
		SetRect( &r, 0, 12, dwWidth-48, 15 );
		FillRect( hDC, &r, hbrColor );
	}

	// drawing mask bitmap
	SelectObject( hDC, hbmMask );
	SetRect( &r, 0, 0, 16, 16 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( WHITE_BRUSH ) );
	SetRect( &r, 0, 0, 16, 3 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
	SetRect( &r, 0, 4, 16, 7 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
	SetRect( &r, 0, 8, 16, 11 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
	SetRect( &r, 0, 12, 16, 15 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	RestoreDC( hDC, iState );
	DeleteObject( hbrColor );

	// making new icon
	ii.fIcon = TRUE;
	ii.hbmColor = hbmColor;
	ii.hbmMask = hbmMask;
	m_hLastMadeIcon = m_nidTrayIcon.hIcon = CreateIconIndirect( &ii );
	DeleteObject( hbmMask );
	DeleteObject( hbmColor );

	// deinit GDI stuff
	DeleteDC( hDC );
	DeleteDC( hTempDC );

	return TRUE;
}


/// Creates tray icon, style: 'Numerical' or 'Digital'
static BOOL MakeIcon_Numerical( UINT glifOffset )
{
	// load font for icon (if haven't already done it)
	if ( m_bmFont == NULL )
		m_bmFont = (HBITMAP)LoadImage( g_hInstance, MAKEINTRESOURCE(IDB_ICONFONT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR );

	// get memory info
	LPMEMSTATITEM lpMemStat;
	BOOL bMemState;
	DWORD dwCnt, dwCur;
	DWORD dwFreeMemory, dwAllMemory;
	char szFree[32];

	lpMemStat = LockMSData( &dwCnt, &dwCur );
	dwFreeMemory = lpMemStat[dwCur].dwAvailPhys/MB;
	dwAllMemory = lpMemStat[dwCur].dwTotalPhys;
	bMemState = g_Config.dwMinFree > lpMemStat[dwCur].dwAvailPhys;
	sprintf( szFree, "%i", dwFreeMemory );
	UnlockMSData();

	// do we have to write a new icon?
	if ( g_dwLastIconValue == dwFreeMemory && m_bLastState == bMemState )
		return FALSE;

	// static info update
	g_dwLastIconValue = dwFreeMemory;
	m_bLastState = bMemState;

	// we destroy last made icon first
	if ( m_hLastMadeIcon )
		DestroyIcon( m_hLastMadeIcon );

	// creating DC for painting icon
	HDC hDC, hDC2, hTempDC;
	HBITMAP hbmColor;
	HBITMAP hbmMask;
	HBRUSH hbrColor;
	UINT iState;
	LOGBRUSH logbr;
	hTempDC = CreateDC( "DISPLAY", NULL, NULL, NULL );
	hDC = CreateCompatibleDC( hTempDC );
	hDC2 = CreateCompatibleDC( hTempDC );
	hbmColor = CreateCompatibleBitmap( hTempDC, 16, 16 );
	hbmMask = CreateBitmap(  16, 16, 1, 1, NULL );
	iState = SaveDC( hDC );
	logbr.lbColor = bMemState ? RGB(255,0,0) : RGB(0,255,0);
	logbr.lbStyle = BS_SOLID;
	hbrColor = CreateBrushIndirect( &logbr );
	
	// paint icon
	RECT r;
	UINT ox,sl;
	SelectObject( hDC, hbmColor );
	SelectObject( hDC2, m_bmFont );
	SetRect( &r, 0, 0, 16, 16 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( WHITE_BRUSH ) );

	if ( glifOffset > 0 )
	{
		SetRect( &r, 0, 0, 16, 10 );
		FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
	}

	if ( strlen(szFree) == 4 )
	{
		sl = strlen( szFree )*4;
		ox = (16-sl)/2;
		for( UINT i=0; i<4; i++ )
		{
			UINT ofs;
			ofs = szFree[i]-0x30;
			BitBlt( hDC, ox+(i*4), 1, 4, 8, hDC2, ofs*4, 0+glifOffset, SRCCOPY );
		}
	}
	else
	{
		sl = strlen( szFree )*5;
		ox = (16-sl)/2;
		for( UINT i=0; i<3; i++ )
		{
			UINT ofs;
			ofs = szFree[i]-0x30;
			BitBlt( hDC, ox+(i*5), 1, 5, 8, hDC2, ofs*5, 18+glifOffset, SRCCOPY );
		}
	}

	SetRect( &r, 0, 10, 16, 15 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );
	SetRect( &r, 1, 11, 1+(UINT)(14.0*(((float)dwFreeMemory*MB)/dwAllMemory)), 14 );
	FillRect( hDC, &r, hbrColor );

	SelectObject( hDC, hbmMask );
	SelectObject( hDC2, hbmColor );
	BitBlt( hDC, 0, 0, 16, 16, hDC2, 0, 0, SRCCOPY );

	RestoreDC( hDC, iState );

	// finally make trayicon
	ICONINFO ii;
	ii.fIcon = TRUE;
	ii.hbmColor = hbmColor;
	ii.hbmMask = hbmMask;
	m_hLastMadeIcon = m_nidTrayIcon.hIcon = CreateIconIndirect( &ii );
	DeleteObject( hbmMask );
	DeleteObject( hbmColor );

	// releasing GDI objects
	DeleteDC( hDC2 );
	DeleteDC( hDC );
	DeleteDC( hTempDC );
	DeleteObject( hbmColor );
	DeleteObject( hbmMask );
	DeleteObject( hbrColor );

	return TRUE;
}


/// Makes tray icon, style: 'Minichart'
static BOOL MakeIcon_MiniChart()
{
	LPMEMSTATITEM lpMemStat;
	DWORD dwCnt, dwCur;
	int i,x,y;
	ICONINFO ii;
	HBITMAP hbmColor;
	HBITMAP hbmMask;
	HDC hTempDC, hDC;
	HPEN hpenColor;
	RECT r;
	int iState;

	// we destroy last made icon
	if ( m_hLastMadeIcon )
		DestroyIcon( m_hLastMadeIcon );

	// all init stuff
	lpMemStat = LockMSData( &dwCnt, &dwCur );
	hTempDC = CreateDC( "DISPLAY", NULL, NULL, NULL );
	hDC = CreateCompatibleDC( hTempDC );
	hbmColor = CreateCompatibleBitmap( hTempDC, 16, 16 );
	hbmMask = CreateBitmap( 16, 16, 1, 1, NULL );
	hpenColor = CreatePen( PS_SOLID, 0, RGB(0,255,0) );
	iState = SaveDC( hDC );

	// drawing color bitmap (mini chart)
	float vScaleX = 16.0f / dwCnt;
	float vScaleY = 16.0f / lpMemStat[dwCur].dwTotalPhys;
	SelectObject( hDC, hbmColor );
	SetRect( &r, 0, 0, 16, 16 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	MoveToEx( hDC, 16, 16 - (DWORD)(vScaleY * lpMemStat[dwCur].dwAvailPhys), NULL );
	SelectObject( hDC, hpenColor );
	for ( i=dwCur; i>=0; i-- )
	{
		x = (DWORD)(vScaleX * (i + (dwCnt-dwCur)));
		y = 16 - (DWORD)(vScaleY * lpMemStat[i].dwAvailPhys);
		LineTo( hDC, x, y );
	}
	
	for ( i+=dwCnt; i>(int)dwCur; i-- )
	{
		x = (DWORD)(vScaleX * (i-dwCur));
		y = 16 - (DWORD)(vScaleY * lpMemStat[i].dwAvailPhys);
		LineTo( hDC, x, y );
	}
	LineTo( hDC, x-1, y );

	// drawing mask bitmap
	SelectObject( hDC, hbmMask );
	SetRect( &r, 0, 0, 16, 16 );
	FillRect( hDC, &r, (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	RestoreDC( hDC, iState );
	DeleteObject( hpenColor );

	// making new icon
	ii.fIcon = TRUE;
	ii.hbmColor = hbmColor;
	ii.hbmMask = hbmMask;
	m_hLastMadeIcon = m_nidTrayIcon.hIcon = CreateIconIndirect( &ii );
	DeleteObject( hbmMask );
	DeleteObject( hbmColor );

	// deinit GDI stuff
	UnlockMSData();
	DeleteDC( hDC );
	DeleteDC( hTempDC );

	return TRUE;
}


/// Handles WM_INITDIALOG message - creates all pages in dialog box and shows first
static void OnInitDialog( HWND hDlg )
{
	MENUITEMINFO mmi;
	
	// save window handle and setup main window
	g_hMainWnd = hDlg;
	LoadConfig( NULL );

	SendMessage( hDlg, WM_SETTEXT, 0, (LPARAM)STR_MEMSTAT_CAPTION );

	POINT p;
	p.x = g_Config.posMainWnd.dwPositionX;
	p.y = g_Config.posMainWnd.dwPositionY;

	if ( g_Config.posMainWnd.dwHeight == 0 )
		CenterWindow( hDlg, TRUE );
	else
		SetWindowPos( hDlg, 0, p.x, p.y, 0, 0, SWP_NOSIZE|SWP_NOZORDER );

	g_Config.posMainWnd.dwHeight = 1;

	SetWindowPos( hDlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE );

	// *** create threads, non-child wnds and enabling startup features ***
	// order of doing it is _critical_ !!!

		// freeing thread
		CreateFreeThread();

		// monitoring thread
		CreateMonitorThread();

		// create chart parent window
		CreateChartParentWnd();

		// and at least enable MSL at startup (option)
		if ( g_Config.bAutoMSL )
			EnableMSL();

	// *** end of creation ***

	// load tray menu & setup it
	m_hTrayMenu = LoadMenu( g_hInstance, MAKEINTRESOURCE(IDR_TRAYMENU) );
	m_hTrayMenu2 = LoadMenu( g_hInstance, MAKEINTRESOURCE(IDR_TRAYMENU2) );
	memset( &mmi, 0, sizeof(mmi) );
	mmi.cbSize = sizeof(mmi);
	mmi.fMask = MIIM_STATE;
	mmi.fState = MFS_ENABLED | MFS_DEFAULT;
	SetMenuItemInfo( m_hTrayMenu, ID_MEMCHART, FALSE, &mmi );
	SetMenuItemInfo( m_hTrayMenu2, ID_MEMCHART, FALSE, &mmi );

	// add notification icon to the system tray
	memset( &m_nidTrayIcon, 0, sizeof(m_nidTrayIcon) );
	m_nidTrayIcon.cbSize = sizeof(m_nidTrayIcon);
	m_nidTrayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nidTrayIcon.uCallbackMessage = WM_TRAYICON;
	m_nidTrayIcon.hIcon = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_TRAY) );
	m_nidTrayIcon.hWnd = hDlg;
	sprintf( m_nidTrayIcon.szTip, "MemStat XP" );
	Shell_NotifyIcon( NIM_ADD, &m_nidTrayIcon );

	// create config pages
	m_hwndTab = GetDlgItem( hDlg, IDC_CFGTAB );
	g_hwcpAbout = CreateCfgAbout( m_hwndTab, hDlg );
	g_hwcpAdvanced = CreateCfgAdvanced( m_hwndTab, hDlg );
	g_hwcpAppearance = CreateCfgAppearance( m_hwndTab, hDlg );
	g_hwcpMonitoring = CreateCfgMonitoring( m_hwndTab, hDlg );
	g_hwcpFreeing = CreateCfgFreeing( m_hwndTab, hDlg );

	// setup them
	MoveToTabArea( g_hwcpFreeing );
	MoveToTabArea( g_hwcpMonitoring );
	MoveToTabArea( g_hwcpAppearance );
	MoveToTabArea( g_hwcpAdvanced );
	MoveToTabArea( g_hwcpAbout );
	SetTabPage( g_hwcpFreeing );

	// create displaying timer (for updating infos)
	SetTimer( hDlg, 1, g_Config.dwInterval, NULL );

	// free p.m. at startup (if enabled)
	if ( g_Config.bStartupFree )
		TryFree( g_Config.dwStartupFree, 2 );
}


/// Handles WM_DESTROY message - destroys all main window data
static void OnDestroy( HWND hDlg )
{
	WinHelp( g_hMainWnd, g_lpszHelp, HELP_QUIT, 0 );
	DisableMSL();
	if ( g_Config.bAutoSave )
		SaveConfig( NULL );
	Shell_NotifyIcon( NIM_DELETE, &m_nidTrayIcon );
	PostQuitMessage(0);
}


/// Handles all dialog controls commands sent by WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	switch ( wID )
	{
		case IDCLOSE:
			ShowWindow( hDlg, SW_HIDE );
			break;
	}
}


/// Handles all menu commands (tray-menu)
static void OnMenuSelect( HWND hDlg, WORD wID )
{
	static bIsBenchmarking = FALSE;
	BOOL bVis;

	if ( (wID >= 40100) && (wID < 40199) )
	{
		DWORD dwTargetSize = ((1024*1024) << (wID-40100));
		TryFreeUp( dwTargetSize, 3 );
		return;
	}

	switch ( wID )
	{
		case ID_EXIT:
			DestroyWindow( g_hMainWnd );
			break;

		case ID_OPENCONFIG:
			ShowWindow( hDlg, SW_SHOW );
			SetForegroundWindow( hDlg );
			break;

		case ID_ABOUT:
			ShowWindow( hDlg, SW_SHOW );
			SetForegroundWindow( hDlg );
			SetTabPage( g_hwcpAbout );
			break;

		case ID_CFG_FREEING:
			ShowWindow( hDlg, SW_SHOW );
			SetForegroundWindow( hDlg );
			SetTabPage( g_hwcpFreeing );
			break;

		case ID_CFG_MONITORING:
			ShowWindow( hDlg, SW_SHOW );
			SetForegroundWindow( hDlg );
			SetTabPage( g_hwcpMonitoring );
			break;

		case ID_CFG_APPEARANCE:
			ShowWindow( hDlg, SW_SHOW );
			SetForegroundWindow( hDlg );
			SetTabPage( g_hwcpAppearance );
			break;

		case ID_CFG_ADVANCED:
			ShowWindow( hDlg, SW_SHOW );
			SetForegroundWindow( hDlg );
			SetTabPage( g_hwcpAdvanced );
			break;

		case ID_WINHELP:
			WinHelp( g_hMainWnd, g_lpszHelp, HELP_FINDER, 0 );
			break;

		case ID_MEMCHART:
			bVis = IsWindowVisible( g_hChartWnd);
			ShowWindow( g_hChartParentWnd, bVis ? SW_HIDE : SW_SHOW );
			if ( !bVis )
				SetForegroundWindow( g_hChartParentWnd );
			break;

		case ID_AUTOFREE:
			g_Config.bAutoDisabled = !g_Config.bAutoDisabled;
			SendMessage( g_hwcpFreeing, WM_CONFIGCHANGE, 0, 0 );
			break;

		case ID_FREENOW:
			// free by manual free button :)
			CLICKBTN( g_hwcpFreeing, IDC_FREENOW );
			break;

		case ID_CANCELFREE:
			CancelFreeing();
			break;

		case ID_SSAVER:
			Sleep( 500 );
			SendMessage( GetDesktopWindow(), WM_SYSCOMMAND, SC_SCREENSAVE, 0 );
			break;

		case IDC_IS_CHIP:
			g_Config.dwTrayIconStyle = TI_STANDARD;
			SendMessage( g_hwcpAppearance, WM_CONFIGCHANGE, 0, 0 );
			UpdateMemStats();
			break;

		case IDC_IS_FOURBARS:
			g_Config.dwTrayIconStyle = TI_FREEBAR;
			SendMessage( g_hwcpAppearance, WM_CONFIGCHANGE, 0, 0 );
			UpdateMemStats();
			break;

		case IDC_IS_MINICHART:
			g_Config.dwTrayIconStyle = TI_MINICHART;
			SendMessage( g_hwcpAppearance, WM_CONFIGCHANGE, 0, 0 );
			UpdateMemStats();
			break;

		case IDC_IS_NUMERICAL:
			g_Config.dwTrayIconStyle = TI_NUMERICAL;
			SendMessage( g_hwcpAppearance, WM_CONFIGCHANGE, 0, 0 );
			UpdateMemStats();
			break;

		case IDC_IS_NUMERICAL2:
			g_Config.dwTrayIconStyle = TI_NUMERICAL2;
			SendMessage( g_hwcpAppearance, WM_CONFIGCHANGE, 0, 0 );
			UpdateMemStats();
			break;

		case ID_MEM_BENCH:
			if ( bIsBenchmarking )
				break;
			bIsBenchmarking = TRUE;
			DialogBox( g_hInstance, MAKEINTRESOURCE(IDD_BENCHMARK), NULL, MemBenchDlgProc );
			bIsBenchmarking = FALSE;
			break;

		case ID_FREEUP_ALLPOSSIBLE:
			TryFree( g_msMemInfo.dwTotalPhys, 3 );
			break;

		case ID_CLEARCLIPBOARD:
			if ( OpenClipboard( g_hMainWnd ) )
			{
				EmptyClipboard();
				CloseClipboard();
			}
			break;
	}
}


/// Updates tray menu before showing
static void UpdateTrayMenu()
{
	MENUITEMINFO mmi;
	char txt[256];

	DWORD dwCnt, dwCur, dwCurFree;
	LPMEMSTATITEM lpMemStat, lpAll;

	lpAll = LockMSData( &dwCnt, &dwCur );
	lpMemStat = &lpAll[dwCur];
	dwCurFree = lpMemStat->dwAvailPhys;
	UnlockMSData();
	
	memset( &mmi, 0, sizeof(mmi) );
	mmi.cbSize = sizeof(mmi);
	mmi.fMask = MIIM_STATE;
	mmi.fState = MFS_ENABLED | (IsWindowVisible( g_hChartWnd ) ? MFS_CHECKED : MFS_UNCHECKED);
	SetMenuItemInfo( m_hTrayMenu, ID_MEMCHART, FALSE, &mmi );
	mmi.fState = MFS_ENABLED | (!g_Config.bAutoDisabled ? MFS_CHECKED : MFS_UNCHECKED);
	SetMenuItemInfo( m_hTrayMenu, ID_AUTOFREE, FALSE, &mmi );
	sprintf( txt, STR_TRAYMENU_FREEUP, g_Config.dwManualFree/(float)MB, (g_Config.dwManualFree+dwCurFree)/(float)MB );
	mmi.fMask = MIIM_TYPE;;
	mmi.fType = MFT_STRING;
	mmi.dwTypeData = txt;
	mmi.cch = strlen(txt);
	SetMenuItemInfo( m_hTrayMenu, ID_FREENOW, FALSE, &mmi );

	// check icon style
	WORD wID = 0;
	switch ( g_Config.dwTrayIconStyle )
	{
	case TI_STANDARD:	wID = IDC_IS_CHIP;			break;
	case TI_FREEBAR:	wID = IDC_IS_FOURBARS;		break;
	case TI_MINICHART:	wID = IDC_IS_MINICHART;		break;
	case TI_NUMERICAL:	wID = IDC_IS_NUMERICAL;		break;
	case TI_NUMERICAL2:	wID = IDC_IS_NUMERICAL2;	break;
	}

	mmi.fMask = MIIM_STATE;
	mmi.fState = MFS_ENABLED | ( wID == IDC_IS_CHIP ? MFS_CHECKED : MFS_UNCHECKED );
	SetMenuItemInfo( m_hTrayMenu, IDC_IS_CHIP, FALSE, &mmi );
	mmi.fState = MFS_ENABLED | ( wID == IDC_IS_FOURBARS ? MFS_CHECKED : MFS_UNCHECKED );
	SetMenuItemInfo( m_hTrayMenu, IDC_IS_FOURBARS, FALSE, &mmi );
	mmi.fState = MFS_ENABLED | ( wID == IDC_IS_MINICHART ? MFS_CHECKED : MFS_UNCHECKED );
	SetMenuItemInfo( m_hTrayMenu, IDC_IS_MINICHART, FALSE, &mmi );
	mmi.fState = MFS_ENABLED | ( wID == IDC_IS_NUMERICAL ? MFS_CHECKED : MFS_UNCHECKED );
	SetMenuItemInfo( m_hTrayMenu, IDC_IS_NUMERICAL, FALSE, &mmi );
	mmi.fState = MFS_ENABLED | ( wID == IDC_IS_NUMERICAL2 ? MFS_CHECKED : MFS_UNCHECKED );
	SetMenuItemInfo( m_hTrayMenu, IDC_IS_NUMERICAL2, FALSE, &mmi );

	UINT dwClipFormats;
	OpenClipboard( g_hMainWnd );
	dwClipFormats = CountClipboardFormats();
	CloseClipboard();

	mmi.fMask = MIIM_STATE;
	mmi.fState = dwClipFormats > 0 ? MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo( m_hTrayMenu, ID_CLEARCLIPBOARD, FALSE, &mmi );
}


/// Handles all messages from tray-icon
static void OnTrayIcon( HWND hDlg, UINT msg )
{
	POINT posCursor;

	switch ( msg )
	{
		case WM_LBUTTONUP:
			OnMenuSelect( hDlg, ID_MEMCHART );
			break;

		case WM_RBUTTONUP:
			// update & show pop-up menu
			BOOL bIsFreeing;
			bIsFreeing = ( TryFree(0,0) == FALSE );
			GetCursorPos( &posCursor );
			if ( !bIsFreeing )
				UpdateTrayMenu();
			SetForegroundWindow( hDlg );
			TrackPopupMenuEx( GetSubMenu( bIsFreeing ? m_hTrayMenu2 : m_hTrayMenu, 0 ), 0, posCursor.x, posCursor.y, hDlg, NULL );
			PostMessage(hDlg, WM_NULL, 0, 0);
			break;
	}
}


/// Handles WM_NOTIFY notification messages (from tab control)
static void OnNotify( HWND hDlg, LPNMHDR lpNM )
{
	int iNew;
	TCITEM tci;

	switch ( lpNM->idFrom )
	{
		case IDC_CFGTAB:
			if ( lpNM->code == TCN_SELCHANGE )
			{
				if ( g_hwcpCurrent )
					ShowWindow( g_hwcpCurrent, SW_HIDE );
				iNew = TabCtrl_GetCurSel( lpNM->hwndFrom );
				tci.mask = TCIF_PARAM;
				TabCtrl_GetItem( lpNM->hwndFrom, iNew, &tci );
				g_hwcpCurrent = (HWND)tci.lParam;
				ShowWindow( g_hwcpCurrent, SW_SHOW );
			}
			break;
	}
}


/// Main window message handler
static BOOL CALLBACK MainWndProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			OnInitDialog( hDlg );
			return TRUE;

		case WM_DESTROY:
			OnDestroy( hDlg );
			break;

		case WM_TRAYICON:
			OnTrayIcon( hDlg, lParam );
			break;

		case WM_COMMAND:
			if ( lParam != NULL )
				OnCtrlCommand( hDlg, HIWORD(wParam), LOWORD(wParam), (HWND)lParam );
			else if ( HIWORD(wParam) == 0 )
				OnMenuSelect( hDlg, LOWORD(wParam) );
			break;

		case WM_NOTIFY:
			OnNotify( hDlg, (LPNMHDR)lParam );
			break;

		case WM_MOVE:
			WINDOWPLACEMENT wpl;
			wpl.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement( hDlg, &wpl );
			g_Config.posMainWnd.dwPositionX = wpl.rcNormalPosition.left;
			g_Config.posMainWnd.dwPositionY = wpl.rcNormalPosition.top;
			break;

		case WM_CLOSE:
			ShowWindow( hDlg, SW_HIDE );
			return TRUE;

		case WM_INTERVALCHANGED:
			KillTimer( hDlg, 1 );
			SetTimer( hDlg, 1, g_Config.dwInterval, NULL );
			break;

		case WM_TIMER:
			if ( wParam == 1 )
			{
				UpdateMemStats();
				RedrawWindow( g_hChartWnd, NULL, NULL, RDW_INVALIDATE );
			}
			break;

		case WM_CONFIGCHANGE:
			EnumChildWindows( hDlg, Resender, NULL );
			break;

		case WM_ENDSESSION:
			DestroyWindow( hDlg );
			break;

		case WM_DISPLAYCHANGE:
			CheckWindows();
			break;
	}

	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


/// Updates all real-time memory indicators
void UpdateMemStats()
{
	char str[256];
	DWORD dwCurFree;
	MEMSTATITEM curSample;

	CheckFreeingTime();

	// update indicator in config dialog box
	GetRecentSample( &curSample );
	dwCurFree = curSample.dwAvailPhys;
	WORD wMax = (WORD)(curSample.dwTotalPhys / KB64);
	SDIM( g_hMainWnd, IDC_FREEBAR, PBM_SETRANGE, 0, MAKELONG(0,wMax) );
	SDIM( g_hMainWnd, IDC_FREEBAR, PBM_SETPOS, dwCurFree / KB64, 0 );
	BOOL isBusy = (TryFree(0,0) == FALSE);

	EnableWindow( GetDlgItem( g_hwcpFreeing, IDC_FREENOW ), isBusy ? FALSE : TRUE );
	if ( isBusy )
		sprintf( str, STR_FREEING, g_dwAmountDone/10.0f );
	else
		sprintf( str, STR_FREEING_FREEUP, 
			g_Config.dwManualFree / (float)MB, (g_Config.dwManualFree+dwCurFree)/(float)MB );
	SDIM( g_hwcpFreeing, IDC_FREENOW, WM_SETTEXT, 0, (LPARAM)str );

	if ( g_dwAmountDone > 0 )
	{
		ShowWindow( GetDlgItem(g_hwcpFreeing,IDC_FREEING), SW_SHOW );
		SDIM( g_hwcpFreeing, IDC_FREEING, PBM_SETRANGE, 0, MAKELONG(0,1000) );
		SDIM( g_hwcpFreeing, IDC_FREEING, PBM_SETPOS, g_dwAmountDone, 0 );
	}
	else
	{
		ShowWindow( GetDlgItem(g_hwcpFreeing,IDC_FREEING), SW_HIDE );
		SDIM( g_hwcpFreeing, IDC_FREEING, PBM_SETPOS, 0, 0 );
	}


	sprintf( str, "%.2f MB", dwCurFree / (float)MB );
	SDIM( g_hMainWnd, IDC_FREETXT, WM_SETTEXT, 0, (LPARAM)str );

	// run auto-free thread if needed and allowed
	if ( !g_Config.bAutoDisabled )
		if ( dwCurFree < g_Config.dwMinFree )
			TryFree( g_Config.dwFreeTo, 2 );
	RedrawWindow( g_hChartParentWnd, NULL, NULL, RDW_INVALIDATE|RDW_NOCHILDREN );

	// update tray icon and tip-text
	FormatRecentSample( g_Config.pszTooltipFormat, str );

	BOOL bReqUpdate;
	
	if ( TryFree(0,0) == FALSE && g_Config.bBusyIcon )
	{
		bReqUpdate = g_dwLastIconValue==0xfffffffe ? FALSE : TRUE;
		if ( m_hLastMadeIcon )
			DestroyIcon( m_hLastMadeIcon );
		m_nidTrayIcon.hIcon = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_WORK) );
		g_dwLastIconValue = 0xfffffffe;
	}
	else
	switch ( g_Config.dwTrayIconStyle )
	{
		case TI_FREEBAR:
			bReqUpdate = MakeIcon_FourBars();
			break;

		case TI_MINICHART:
			bReqUpdate = MakeIcon_MiniChart();
			break;

		case TI_NUMERICAL:
			bReqUpdate = MakeIcon_Numerical( 0 );
			break;

		case TI_NUMERICAL2:
			bReqUpdate = MakeIcon_Numerical( 9 );
			break;

		default:
			bReqUpdate = TRUE;
			if ( curSample.dwAvailPhys < g_Config.dwMinFree )
				m_nidTrayIcon.hIcon = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_TRAYLOW) );
			else
				m_nidTrayIcon.hIcon = LoadIcon( g_hInstance, MAKEINTRESOURCE(IDI_TRAYOK) );
			break;
	}
	if ( strcmp( m_nidTrayIcon.szTip, str ) != 0 || bReqUpdate )
	{
		strncpy( m_nidTrayIcon.szTip, str, 64 );
		Shell_NotifyIcon( NIM_MODIFY, &m_nidTrayIcon );
	}
	
}


/// Creates main window from dialog box template (IDD_MAIN)
BOOL CreateMainWnd()
{
	if ( CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainWndProc ) == NULL )
		return ExitError( STR_CANT_CREATE_MAIN ), FALSE;

	CheckWindows();
	SetForegroundWindow( GetTopWindow( GetDesktopWindow() ) );
	return TRUE;
}
