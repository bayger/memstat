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
// cfgabout.cpp 
// Code of 'About' config page
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE FUNCTIONS
//- =================


/// Handles WM_INITDIALOG message - sets up dialog controls
static void OnInitDialog( HWND hDlg )
{
	// set up all dialog controls
#ifdef _DEBUG
	SDIM( hDlg, IDC_VERSTR, WM_SETTEXT, 0, (LPARAM)"MemStat XP v"MEMSTAT_VERSION_STR" (debug)" );
#else
	SDIM( hDlg, IDC_VERSTR, WM_SETTEXT, 0, (LPARAM)"MemStat XP v"MEMSTAT_VERSION_STR );
#endif
	
	SDIM( hDlg, IDC_EMAIL, WM_SETTEXT, 0, (LPARAM)"e-mail: bayger@box43.gnet.pl" );
}


/// Handles all dialog controls commands sent by WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	static char str[512];

	switch ( wID )
	{
	case IDC_WWW:
		if ( wNotifyCode == BN_CLICKED )
		{
			ShellExecute( g_hMainWnd, "open", "http://www.memstat.prv.pl", "", "", NULL );
		}
		break;
	}
}


/// Callback message handler for About page window
static BOOL CALLBACK CfgAboutProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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

	case WM_ERASEBKGND:
		SetWindowLong( hDlg, DWL_MSGRESULT, TRUE );
		return TRUE;
	}
	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


/// Creates 'About' configuration page
HWND CreateCfgAbout( HWND hwndTab, HWND hwndTabOwner )
{
	TCITEM tci;
	HWND hDlg;

	// create and position
	hDlg = CreateDialog( g_hInstance, MAKEINTRESOURCE(IDD_CFG_ABOUT), hwndTabOwner, CfgAboutProc );

	if ( hDlg == NULL )
	return ExitError( STR_CANT_CREATE_DIALOG_PAGE ), NULL;

	// add tab control item
	memset( &tci, 0, sizeof(tci) );
	tci.mask = TCIF_PARAM | TCIF_TEXT;
	tci.pszText = STR_TAB_ABOUT;
	tci.lParam = (LPARAM)hDlg;
	SendMessage( hwndTab, TCM_INSERTITEM, 0, (LPARAM)&tci );
	
	return hDlg;
}