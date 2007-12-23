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
// cfgmonitoring.cpp
// Code of 'Monitoring' configuration page
//
// Note: MSL = Memory Status Logging (or simply logging) feature
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE FUNCTIONS
//- =================


/// Updates MSL-related controls
static void UpdateMSLControls( HWND hDlg )
{
	char str[256];
	BOOL bMSL = IsMSLEnabled();

	sprintf( str, STR_MSL_STATUS, bMSL ? STR_ENABLED : STR_DISABLED );
	SDIM( hDlg, IDC_MSLINFO, WM_SETTEXT, 0, (LPARAM)str );
	sprintf( str, "%s "STR_LOGGING, bMSL ? STR_DISABLE: STR_ENABLE );
	SDIM( hDlg, IDC_MSL, WM_SETTEXT, 0, (LPARAM)str );

	SDIM( hDlg, IDC_LOGFILE, WM_SETTEXT, 0, (LPARAM)g_Config.pszMSL_LogName );
	SDIM( hDlg, IDC_LOGAPPEND, BM_SETCHECK, g_Config.bMSL_Append ? BST_CHECKED : BST_UNCHECKED, 0 );
	sprintf( str, "%u", g_Config.dwMSL_LimitSize/KB );
	SDIM( hDlg, IDC_MAXLOGSIZE, WM_SETTEXT, 0, (LPARAM)str );
	SDIM( hDlg, IDC_LOGLIMIT, BM_SETCHECK, g_Config.dwMSL_LimitSize ? BST_CHECKED : BST_UNCHECKED, 0 );
	SDIM( hDlg, IDC_DIRECTWRITE, BM_SETCHECK, g_Config.bDirectWriting ? BST_CHECKED : BST_UNCHECKED, 0 );

	EnableWindow( GetDlgItem( hDlg, IDC_LOGFILE ), (WPARAM)!bMSL );
	EnableWindow( GetDlgItem( hDlg, IDC_BROWSE ), (WPARAM)!bMSL );
	EnableWindow( GetDlgItem( hDlg, IDC_LOGLIMIT ), (WPARAM)!bMSL );
	EnableWindow( GetDlgItem( hDlg, IDC_MAXLOGSIZE ), (WPARAM)!bMSL );
	EnableWindow( GetDlgItem( hDlg, IDC_LOGAPPEND ), (WPARAM)!bMSL );
}


/// Refreshes dialog controls when config has changed
static void OnConfigChange( HWND hDlg )
{
	char str[64];

	// setup config controls
	sprintf( str, "%u", g_Config.dwInterval );
	SDIM( hDlg, IDC_INTERVAL, WM_SETTEXT, 0, (LPARAM)str );
}


/// Handles WM_INITDIALOG message - initializes page
static void OnInitDialog( HWND hDlg )
{
	char str[64];
	DWORD dwCount;

	OnConfigChange( hDlg );

	// set up constant controls
	LockMSData( &dwCount, NULL );
	UnlockMSData();
	sprintf( str, "%u", dwCount );
	SDIM( hDlg, IDC_MEMSTATSCOUNT, WM_SETTEXT, 0, (LPARAM)str );
	UpdateMSLControls( hDlg );
}


/// Handles all dialog controls commands sent via WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	char str[256];
	DWORD dwVal;
	BOOL bSuccess;

	switch ( wID )
	{
		case IDC_INTERVAL:
			if ( wNotifyCode == EN_KILLFOCUS )
			{
				// changes monitoring samplerate
				SDIM( hDlg, IDC_INTERVAL, WM_GETTEXT, 255, (LPARAM)str );
				sscanf( str, "%u", &dwVal );
				if ( dwVal < MIN_INTERVAL ) dwVal = MIN_INTERVAL;
				g_Config.dwInterval = dwVal;
				sprintf( str, "%u", g_Config.dwInterval );
				SDIM( hDlg, IDC_INTERVAL, WM_SETTEXT, 0, (LPARAM)str );
				SendMessage( g_hMainWnd, WM_INTERVALCHANGED, 0, 0 );
			}
			break;

		case IDC_MSL:
			if ( wNotifyCode == BN_CLICKED )
			{
				// enables or disables MSL (dep.on current MSL state)
				bSuccess = IsMSLEnabled() ? DisableMSL(), TRUE : EnableMSL();
				if ( !bSuccess )
				{
					MessageBox( hDlg, STR_CANT_ENABLE_MSL, 
						STR_ERROR, MB_OK | MB_ICONHAND );
					SetFocus( GetDlgItem( hDlg, IDC_LOGFILE ) );
				}
			}
			break;

		case IDC_LOGFILE:
			if ( wNotifyCode == EN_KILLFOCUS )
				// updates log filename
				SDIM( hDlg, IDC_LOGFILE, WM_GETTEXT, 255, (LPARAM)g_Config.pszMSL_LogName );
			break;

		case IDC_LOGAPPEND:
			if ( wNotifyCode == BN_CLICKED )
				// updates log appending flag state
				g_Config.bMSL_Append = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_DIRECTWRITE:
			if ( wNotifyCode == BN_CLICKED )
				// updates log appending flag state
				g_Config.bDirectWriting = SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_EDITFORMAT:
			EditFormat( g_Config.pszMSL_LogFormat, STR_EDIT_LOG_FORMAT );
			break;

		case IDC_ADDITEM:
			HMENU hmPopup;
			RECT r;
			WORD menuItemID;
			hmPopup = LoadMenu( g_hInstance, MAKEINTRESOURCE(IDR_LOGITEMS) );
			GetWindowRect( GetDlgItem(hDlg,IDC_ADDITEM), &r );
			menuItemID = TrackPopupMenu( GetSubMenu(hmPopup,0), TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD, r.right, r.top, 0, hDlg, NULL );
			//AddItemToLog( menuItemID );
			DestroyMenu( hmPopup );
			break;

		case IDC_LOGLIMIT:
			if ( wNotifyCode == BN_CLICKED )
			{
				// clears limit or sets standard 64KB limit (dep.on checkbox state)
				g_Config.dwMSL_LimitSize = 
					SDIM( hDlg, wID, BM_GETCHECK, 0, 0 ) == BST_CHECKED ?
						64*KB :
						0;
				UpdateMSLControls( hDlg );
			}
			break;

		case IDC_MAXLOGSIZE:
			if ( wNotifyCode == EN_KILLFOCUS )
			{
				// updates new log size limit
				SDIM( hDlg, wID, WM_GETTEXT, 255, (LPARAM)str );
				sscanf( str, "%u", &dwVal );
				g_Config.dwMSL_LimitSize = dwVal * KB;
				UpdateMSLControls( hDlg );
			}
			break;
	}
}


/// 'Monitoring' page window procedure
static BOOL CALLBACK CfgMonitoringProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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

		case WM_UPDATEMSLCTRLS:
			// this app-message is sent by monitoring thread to update
			// MSL-related controls (when for example you or thread disable MSL)
			UpdateMSLControls( hDlg );
			return TRUE;

		case WM_CONFIGCHANGE:
			OnConfigChange( hDlg );
			break;
	}
	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


/// Creates 'Monitoring' configuration page
HWND CreateCfgMonitoring( HWND hwndTab, HWND hwndTabOwner )
{
	TCITEM tci;
	HWND hWnd;

	// create and position
	hWnd = CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_CFG_MONITORING), 
		hwndTabOwner, CfgMonitoringProc );

	if ( hWnd == NULL )
		return ExitError( STR_CANT_CREATE_DIALOG_PAGE ), NULL;

	// add tab control item
	memset( &tci, 0, sizeof(tci) );
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.pszText = STR_TAB_MONITORING;
	tci.lParam = (LPARAM)hWnd;
	SendMessage( hwndTab, TCM_INSERTITEM, 0, (LPARAM)&tci );
	
	return hWnd;
}