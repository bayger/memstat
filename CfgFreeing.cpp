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
// cfgfreeing.cpp
// Code of 'Freeing' configuration page
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE VARIABLES
//- =================


static HWND            m_hPageWnd;	// handle to page


//- PRIVATE FUNCTIONS
//- =================


/// Updates hotkey that toggles the chart window
static void UpdateFreeHotkey()
{
	WORD wVKey = LOBYTE(g_Config.dwFreeHotkey);
	WORD wMods = HIBYTE(g_Config.dwFreeHotkey);
	RegisterHotKey( m_hPageWnd, HK_FREENOW, wMods, wVKey );
}


/// Handles all dialog controls commands sent by WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	switch ( wID )
	{
		case IDC_AUTOENABLED:
		case IDC_AUTODISABLED:
			// enable or disable automatic freeing
			g_Config.bAutoDisabled = SDIM( hDlg, IDC_AUTODISABLED, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			break;

		case IDC_FREENOW:
			if ( wNotifyCode == BN_CLICKED )
				TryFreeUp( g_Config.dwManualFree, 3 );
			break;

		case IDC_HOTKEYFREENOW:
			if ( wNotifyCode == EN_CHANGE )
			{
				// set global hotkey
				g_Config.dwFreeHotkey = (WORD)SDIM( hDlg, wID, HKM_GETHOTKEY, 0, 0 );
				UpdateFreeHotkey();
			}
			break;
	}
}


/// Handle track-bar notification messages
static void OnHScroll( HWND hDlg, WORD wNotifyCode, WORD wPos, WORD wID )
{
	char str[256];
	DWORD dwMinFreePos, dwFreeToPos;

	switch ( wID )
	{
		case IDC_FREETO:
		// 'free-to' tracking
			dwMinFreePos = SDIM( hDlg, IDC_MINFREE, TBM_GETPOS, 0, 0 );
			dwFreeToPos = SDIM( hDlg, IDC_FREETO, TBM_GETPOS, 0, 0 );
			if ( dwFreeToPos < dwMinFreePos )
			{
				dwFreeToPos = dwMinFreePos;
				SDIM( hDlg, IDC_FREETO, TBM_SETPOS, TRUE, dwMinFreePos );
			}

			g_Config.dwFreeTo = dwFreeToPos * KB64;
			sprintf( str, "%.2f MB", g_Config.dwFreeTo / (float)MB );
			SDIM( hDlg, IDC_SHOWFREETO, WM_SETTEXT, 0, (LPARAM)str );
			sprintf( str, "(%.0f%%)", (g_Config.dwFreeTo * 100.0f) / g_msMemInfo.dwTotalPhys );
			SDIM( hDlg, IDC_PERCFREETO, WM_SETTEXT, 0, (LPARAM)str );
			break;

		case IDC_MINFREE:
		// 'min-free' tracking
			dwMinFreePos = SDIM( hDlg, IDC_MINFREE, TBM_GETPOS, 0, 0 );
			dwFreeToPos = SDIM( hDlg, IDC_FREETO, TBM_GETPOS, 0, 0 );
			if ( dwFreeToPos < dwMinFreePos )
			{
				dwMinFreePos = dwFreeToPos;
				SDIM( hDlg, IDC_MINFREE, TBM_SETPOS, TRUE, dwMinFreePos );
			}

			g_Config.dwMinFree = dwMinFreePos * KB64;
			sprintf( str, "%.2f MB", g_Config.dwMinFree / (float)MB );
			SDIM( hDlg, IDC_SHOWMIN, WM_SETTEXT, 0, (LPARAM)str );
			sprintf( str, "(%.0f%%)", (g_Config.dwMinFree * 100.0f) / g_msMemInfo.dwTotalPhys );
			SDIM( hDlg, IDC_PERCMIN, WM_SETTEXT, 0, (LPARAM)str );
			break;

		case IDC_MANUALFREE:
		// 'manual-free' tracking
			char str[256];
			DWORD dwCnt, dwCur, dwCurFree, dwToFree;
			MEMSTATITEM* lpMemStat, *lpAll;
			lpAll = LockMSData( &dwCnt, &dwCur );
			lpMemStat = &lpAll[dwCur];
			dwCurFree = lpMemStat->dwAvailPhys;
			UnlockMSData();
			dwToFree = dwCurFree+g_Config.dwManualFree;

			g_Config.dwManualFree = SDIM( hDlg, IDC_MANUALFREE, TBM_GETPOS, 0, 0 ) * KB64;
			sprintf( str, STR_MFU_BUTTON, 
				g_Config.dwManualFree / (float)MB, dwToFree/(float)MB );
			SDIM( hDlg, IDC_FREENOW, WM_SETTEXT, 0, (LPARAM)str );
			break;
	}
}


/// Refreshes dialog controls when config has changed
static void OnConfigChange( HWND hDlg )
{
	// set up all dialog controls
	SDIM( hDlg, IDC_AUTODISABLED, BM_SETCHECK, g_Config.bAutoDisabled ? BST_CHECKED : BST_UNCHECKED, 0 );
	SDIM( hDlg, IDC_AUTOENABLED, BM_SETCHECK, !g_Config.bAutoDisabled ? BST_CHECKED : BST_UNCHECKED, 0 );
	SDIM( hDlg, IDC_AOT, BM_SETCHECK, g_Config.bAlwaysOnTop ? BST_CHECKED : BST_UNCHECKED, 0 );

	WORD wMax = (WORD)(g_msMemInfo.dwTotalPhys / KB64);
	SDIM( hDlg, IDC_MINFREE, TBM_SETRANGE, TRUE, MAKELONG(0,wMax) );
	SDIM( hDlg, IDC_MINFREE, TBM_SETPOS, TRUE, g_Config.dwMinFree / KB64 );
	SDIM( hDlg, IDC_FREETO, TBM_SETRANGE, TRUE, MAKELONG(0,wMax) );
	SDIM( hDlg, IDC_FREETO, TBM_SETPOS, TRUE, g_Config.dwFreeTo / KB64 );
	SDIM( hDlg, IDC_MANUALFREE, TBM_SETRANGE, TRUE, MAKELONG(0,wMax) );
	SDIM( hDlg, IDC_MANUALFREE, TBM_SETPOS, TRUE, g_Config.dwManualFree / KB64 );
	SDIM( hDlg, IDC_FREEING, PBM_SETRANGE, 0, MAKELONG(0,1000) );
	SDIM( hDlg, IDC_FREEING, PBM_SETPOS, g_dwAmountDone, 0 );

	OnHScroll( hDlg, 0, 0, IDC_MINFREE );
	OnHScroll( hDlg, 0, 0, IDC_FREETO );
	OnHScroll( hDlg, 0, 0, IDC_MANUALFREE );

	// manual-free hotkey ctrl
	SDIM( hDlg, IDC_HOTKEYFREENOW, HKM_SETRULES, 
		HKCOMB_NONE|HKCOMB_S|HKCOMB_A|HKCOMB_SA|HKCOMB_SCA|HKCOMB_CA|HKCOMB_SC,
		MAKELPARAM(HOTKEYF_CONTROL,0) );
	SDIM( hDlg, IDC_HOTKEYFREENOW, HKM_SETHOTKEY, g_Config.dwFreeHotkey, 0 );
	UpdateFreeHotkey();
}


/// Handles WM_INITDIALOG message - initializes page
static void OnInitDialog( HWND hDlg )
{
	// save hwnd
	m_hPageWnd = hDlg;
	OnConfigChange( hDlg );
}


/// 'Freeing' page window procedure
static BOOL CALLBACK CfgFreeingProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			OnInitDialog( hDlg );
			return TRUE;

		case WM_HSCROLL:
			OnHScroll( hDlg, LOWORD(wParam), HIWORD(wParam), GetDlgCtrlID((HWND)lParam) );
			break;

		case WM_COMMAND:
			if ( lParam != NULL )
				OnCtrlCommand( hDlg, HIWORD(wParam), LOWORD(wParam), (HWND)lParam );
			break;

		case WM_CONFIGCHANGE:
			OnConfigChange( hDlg );
			break;

		case WM_HOTKEY:
			if ( wParam == HK_FREENOW )
				OnCtrlCommand( hDlg, BN_CLICKED, IDC_FREENOW, 0 );
			break;
	}
	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


/// Creates 'Freeing' configuration page
HWND CreateCfgFreeing( HWND hwndTab, HWND hwndTabOwner )
{
	TCITEM tci;

	// create and position
	CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_CFG_FREEING), 
		hwndTabOwner, CfgFreeingProc );

	if ( m_hPageWnd == NULL )
		return ExitError( STR_CANT_CREATE_DIALOG_PAGE ), NULL;

	// add tab control item
	memset( &tci, 0, sizeof(tci) );
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.pszText = STR_TAB_FREEING;
	tci.lParam = (LPARAM)m_hPageWnd;
	SendMessage( hwndTab, TCM_INSERTITEM, 0, (LPARAM)&tci );
	
	return m_hPageWnd;
}