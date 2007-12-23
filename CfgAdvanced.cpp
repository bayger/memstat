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
// cfgadvanced.cpp
// Code of 'Advanced' config page
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"

typedef	HRESULT STDAPICALLTYPE uxFuncTypeDword(HWND,LPCWSTR,LPCWSTR);


//- PRIVATE VARIABLES
//- =================


static HWND            m_hDeskIcons;	// desktop icons window handle


//- PRIVATE FUNCTIONS
//- =================


/// Handles track-bar (slider) notification messages
static void OnHScroll( HWND hDlg, WORD wNotifyCode, WORD wPos, WORD wID )
{
	char str[64];

	switch ( wID )
	{
		case IDC_FASSLIDER:
		// 'startup-free' tracking
			g_Config.dwStartupFree = SDIM( hDlg, wID, TBM_GETPOS, 0, 0 ) * KB64;
			sprintf( str, STR_FASTO, g_Config.dwStartupFree / (float)MB );
			SDIM( hDlg, IDC_FASTXT, WM_SETTEXT, 0, (LPARAM)str );
			break;
	}
}


/// Refreshes dialog controls when changing configuration
static void OnConfigChange( HWND hDlg )
{
	// set up all dialog controls
	SDIM( hDlg, IDC_AUTOSAVE, BM_SETCHECK,
		g_Config.bAutoSave ? BST_CHECKED : BST_UNCHECKED, 0 );
	SDIM( hDlg, IDC_AUTOMSL, BM_SETCHECK,
		g_Config.bAutoMSL ? BST_CHECKED : BST_UNCHECKED, 0 );
	SDIM( hDlg, IDC_AUTOSTART, BM_SETCHECK,
		IsAutostartEnabled() ? BST_CHECKED : BST_UNCHECKED, 0 );
	SDIM( hDlg, IDC_FASCHECK, BM_SETCHECK,
		g_Config.bStartupFree ? BST_CHECKED : BST_UNCHECKED, 0 );

	SDIM( hDlg, IDC_PRIORITY, CB_SETCURSEL, g_Config.dwAutoPriority, 0 );
	SDIM( hDlg, IDC_MAXRETRIES, CB_SETCURSEL, g_Config.dwMaxRetries-1, 0 );

	// slider setup
	WORD wMax = (WORD)(g_msMemInfo.dwTotalPhys / KB64);
	SDIM( hDlg, IDC_FASSLIDER, TBM_SETRANGE, TRUE, MAKELONG(0,wMax) );
	SDIM( hDlg, IDC_FASSLIDER, TBM_SETPOS, TRUE, g_Config.dwStartupFree / KB64 );
	OnHScroll( hDlg, 0, 0, IDC_FASSLIDER );

	SDIM( hDlg, IDC_LIMITENABLED, BM_SETCHECK,
		g_Config.bLimitTime ? BST_CHECKED : BST_UNCHECKED, 0 );
	EnableWindow( GetDlgItem( hDlg, IDC_MAX_TIME ), g_Config.bLimitTime );
	char str[16];
	sprintf( str, "%i", g_Config.dwLimitTime );
	SDIM( hDlg, IDC_MAX_TIME, WM_SETTEXT, 0, (LPARAM)str );

	// desktop icons mode changing
	/* REMOVED
	if (( m_hDeskIcons ) && ( g_Config.bDTPTransparent != BST_INDETERMINATE ))
	{
		// set state of listview (icons)
		COLORREF crNewBk = g_Config.bDTPTransparent == BST_CHECKED
			? CLR_NONE 
			: GetSysColor( COLOR_DESKTOP );
		if ( crNewBk != ListView_GetTextBkColor( m_hDeskIcons ) )
		{
			ListView_SetTextBkColor( m_hDeskIcons, crNewBk );
			InvalidateRect( m_hDeskIcons, NULL, RDW_ERASE );
			UpdateWindow( m_hDeskIcons );
		}
	}
	*/
}


/// Handles WM_INITDIALOG message - initializes page
static void OnInitDialog( HWND hDlg )
{
	// we gets list view window of the desktop icon
	m_hDeskIcons = FindWindowEx( 
		FindWindowEx( 
			FindWindow( "Progman", "Program Manager" ), 
			NULL, 
			"SHELLDLL_DefView", 
			NULL ),
		NULL,
		"SysListView32",
		NULL );

	// check if there is standard desktop manager (progman)
	EnableWindow( GetDlgItem( hDlg, IDC_DTPTRANSP), (m_hDeskIcons != NULL) && (g_OSVersion.dwMajorVersion < 5) );
	EnableWindow( GetDlgItem( hDlg, IDC_DISABLE_VT), (g_OSVersion.dwMajorVersion+g_OSVersion.dwMinorVersion/100.0f) > 5 );

	// setup constant controls
	SDIM( hDlg, IDC_PRIORITY, CB_ADDSTRING, 0, (LPARAM)STR_PRI_IDLE );
	SDIM( hDlg, IDC_PRIORITY, CB_ADDSTRING, 0, (LPARAM)STR_PRI_NORMAL );
	SDIM( hDlg, IDC_PRIORITY, CB_ADDSTRING, 0, (LPARAM)STR_PRI_HIGH );

	char str[16];
	for( int i=1; i<=16; i++ )
	{
		sprintf( str, "%i", i );
		SDIM( hDlg, IDC_MAXRETRIES, CB_ADDSTRING, 0, (LPARAM)str );
	}

	// standard controls setup
	OnConfigChange( hDlg );
}


/// Handles all dialog controls commands sent by WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	static char str[512];

	switch ( wID )
	{
		case IDC_PRIORITY:
			if ( wNotifyCode == CBN_SELCHANGE )
			{
				// update free-thread priority
				g_Config.dwAutoPriority = SDIM( hDlg, IDC_PRIORITY, CB_GETCURSEL, 0, 0 );
				UpdateFreePriority();
			}
			break;

		case IDC_MAXRETRIES:
			if ( wNotifyCode == CBN_SELCHANGE )
			{
				g_Config.dwMaxRetries = SDIM( hDlg, IDC_MAXRETRIES, CB_GETCURSEL, 0, 0 )+1;
			}
			break;

		case IDC_LIMITENABLED:
			if ( wNotifyCode == BN_CLICKED )
			{
				g_Config.bLimitTime = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
				OnConfigChange( hDlg );
			}
			break;

		case IDC_MAX_TIME:
			if ( wNotifyCode == EN_KILLFOCUS )
			{
				SDIM( hDlg, wID, WM_GETTEXT, 255, (LPARAM)str );
				g_Config.dwLimitTime = atol( str );
				OnConfigChange( hDlg );
			}
			break;

		case IDC_AUTOSTART:
			if ( wNotifyCode == BN_CLICKED )
				// updating autostart feature (instant!)
				EnableAutostart( SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED );
			break;

		case IDC_AUTOMSL:
			if ( wNotifyCode == BN_CLICKED )
				// update MSL-autostart flag
				g_Config.bAutoMSL = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_FASCHECK:
			if ( wNotifyCode == BN_CLICKED )
				// update startup freeing
				g_Config.bStartupFree = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_AUTOSAVE:
			if ( wNotifyCode == BN_CLICKED )
				// update configuration autosave flag
				g_Config.bAutoSave = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_RESETCFG:
			if ( wNotifyCode == BN_CLICKED )
				// restore settings from registry
				if ( MessageBox( hDlg, STR_RESETCFG, STR_WARNING, MB_YESNO | MB_ICONWARNING ) == IDYES )
				{
					LoadConfig( NULL );
					SendMessage( g_hMainWnd, WM_CONFIGCHANGE, 0, 0 );
				}
			break;

		case IDC_SAVECFG:
			if ( wNotifyCode == BN_CLICKED )
			{
				// save current config-settings
				SaveConfig( NULL );
				MessageBox( hDlg, STR_CFGSAVED, STR_MESSAGE, MB_OK | MB_ICONINFORMATION );
			}
			break;
	}
}


/// 'Advanced' page window procedure
static BOOL CALLBACK CfgAdvancedProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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

		case WM_HSCROLL:
			OnHScroll( hDlg, LOWORD(wParam), HIWORD(wParam), GetDlgCtrlID((HWND)lParam) );
			break;

		case WM_CONFIGCHANGE:
			OnConfigChange( hDlg );
			break;
	}
	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


// Creates 'Advanced' configuration page
HWND CreateCfgAdvanced( HWND hwndTab, HWND hwndTabOwner )
{
	TCITEM tci;
	HWND hWnd;

	// create and position
	hWnd = CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_CFG_ADVANCED), 
		hwndTabOwner, CfgAdvancedProc );

	if ( hWnd == NULL )
		return ExitError( STR_CANT_CREATE_DIALOG_PAGE ), NULL;

	// add tab control item
	memset( &tci, 0, sizeof(tci) );
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.pszText = STR_TAB_ADVANCED;
	tci.lParam = (LPARAM)hWnd;
	SendMessage( hwndTab, TCM_INSERTITEM, 0, (LPARAM)&tci );
	
	return hWnd;
}