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
// choosergb.cpp
// Code of 'Choose color' dialog box
//


#include "stdafx.h"
#include "choosergb.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE VARIABLES
//- =================

static HWND            m_hwndChooseRGB;		// handle to dialog


//- PRIVATE FUNCTIONS
//- =================


/// Handles all dialog controls commands sent by WM_COMMAND msg
static BOOL OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	LPCHOOSERGB lpCRGB = (LPCHOOSERGB)GetWindowLong( hDlg, GWL_USERDATA );

	switch ( wID )
	{
		case IDOK:
			*(lpCRGB->lpColor) = lpCRGB->clrSet;
			EndDialog( hDlg, TRUE );
			break;

		case IDCANCEL:
			*(lpCRGB->lpColor) = lpCRGB->clrOld;
			EndDialog( hDlg, FALSE );
			break;

		default:
			return FALSE;
	}
	return TRUE;
}


/// Handles track-bar notification messages
static BOOL OnHScroll( HWND hDlg, WORD wNotifyCode, WORD wPos, WORD wID )
{
	LPCHOOSERGB lpCRGB = (LPCHOOSERGB)GetWindowLong( hDlg, GWL_USERDATA );
	DWORD dwVal,r,g,b;
	WORD wDestID;
	char str[32];

	// update clrSet
	switch ( wID )
	{
		case IDC_RED:
		case IDC_GREEN:
		case IDC_BLUE:
			r = SDIM( hDlg, IDC_RED, TBM_GETPOS, 0, 0 );
			g = SDIM( hDlg, IDC_GREEN, TBM_GETPOS, 0, 0 );
			b = SDIM( hDlg, IDC_BLUE, TBM_GETPOS, 0, 0 );
			lpCRGB->clrSet = RGB(r,g,b);
			if ( lpCRGB->dwFlags & CRGB_REALTIME )
				*(lpCRGB->lpColor) = lpCRGB->clrSet;
			break;

		default:
			return FALSE;
	}

	// update text control
	switch ( wID )
	{
		case IDC_RED:
			dwVal = GetRValue( lpCRGB->clrSet );
			wDestID = IDC_RTXT;
			break;

		case IDC_GREEN:
			dwVal = GetGValue( lpCRGB->clrSet );
			wDestID = IDC_GTXT;
			break;

		case IDC_BLUE:
			dwVal = GetBValue( lpCRGB->clrSet );
			wDestID = IDC_BTXT;
			break;
	}
	sprintf( str, "%u", dwVal );
	SDIM( hDlg, wDestID, WM_SETTEXT, 0, (LPARAM)str );
	RedrawWindow( GetDlgItem( hDlg, IDC_COLOR ), NULL, NULL, RDW_INVALIDATE );

	return TRUE;
}


/// Draws color-area
static BOOL OnDrawItem( HWND hDlg, WORD wID, LPDRAWITEMSTRUCT lpDIS )
{
	RECT r;
	HBRUSH hbrColor;
	LOGBRUSH lb;
	LPCHOOSERGB lpCRGB = (LPCHOOSERGB)GetWindowLong( hDlg, GWL_USERDATA );

	GetClientRect( GetDlgItem( hDlg, wID ), &r );
	lb.lbColor = lpCRGB->clrSet;
	lb.lbStyle = BS_SOLID;
	lb.lbHatch = 0;
	hbrColor = CreateBrushIndirect( &lb );
	//SelectObject( lpDIS->hDC, hbrColor );
	//Rectangle( lpDIS->hDC, r.left, r.top, r.right, r.bottom );
	//SelectObject( lpDIS->hDC, GetStockObject( WHITE_BRUSH ) );
	FillRect( lpDIS->hDC, &r, hbrColor );
	DeleteObject( hbrColor );

	return TRUE;
}


/// Initializes 'Choose RGB color' dialog box (handles WM_INITDIALOG message)
static BOOL OnInitDialog( HWND hDlg, LPCHOOSERGB lpChooseRGB )
{
	char str[32];

	// save dialog data
	SetWindowLong( hDlg, GWL_USERDATA, (LONG)lpChooseRGB );
	lpChooseRGB->clrOld = *(lpChooseRGB->lpColor);

	// interpret flags
	if ( lpChooseRGB->dwFlags & CRGB_INITCOLOR )
		lpChooseRGB->clrSet = *(lpChooseRGB->lpColor);
	else
		lpChooseRGB->clrSet = RGB(127,127,127);
	ShowWindow( GetDlgItem( hDlg, IDC_HEX ), 
		lpChooseRGB->dwFlags & CRGB_PREVENTHEX ? SW_HIDE : SW_SHOW );

	// setup sliders
	SDIM( hDlg, IDC_RED, TBM_SETRANGE, FALSE, MAKELONG(0,255) );
	SDIM( hDlg, IDC_GREEN, TBM_SETRANGE, FALSE, MAKELONG(0,255) );
	SDIM( hDlg, IDC_BLUE, TBM_SETRANGE, FALSE, MAKELONG(0,255) );
	SDIM( hDlg, IDC_RED, TBM_SETPOS, TRUE, GetRValue(lpChooseRGB->clrSet) );
	SDIM( hDlg, IDC_GREEN, TBM_SETPOS, TRUE, GetGValue(lpChooseRGB->clrSet) );
	SDIM( hDlg, IDC_BLUE, TBM_SETPOS, TRUE, GetBValue(lpChooseRGB->clrSet) );

	// rgb text
	sprintf( str, "%u", GetRValue(lpChooseRGB->clrSet) );
	SDIM( hDlg, IDC_RTXT, WM_SETTEXT, 0, (LPARAM)str );
	sprintf( str, "%u", GetGValue(lpChooseRGB->clrSet) );
	SDIM( hDlg, IDC_GTXT, WM_SETTEXT, 0, (LPARAM)str );
	sprintf( str, "%u", GetBValue(lpChooseRGB->clrSet) );
	SDIM( hDlg, IDC_BTXT, WM_SETTEXT, 0, (LPARAM)str );

	// set caption
	if ( lpChooseRGB->lpszCaption != NULL )
		SendMessage( hDlg, WM_SETTEXT, 0, (LPARAM)lpChooseRGB->lpszCaption );

	return TRUE;
}


/// 'Choose color' dialog box procedure
static BOOL WINAPI ChooseRGBDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			return OnInitDialog( hDlg, (LPCHOOSERGB)lParam );

		case WM_COMMAND:
			if ( lParam != NULL )
				return OnCtrlCommand( hDlg, HIWORD(wParam), LOWORD(wParam), (HWND)lParam );
			break;

		case WM_HSCROLL:
			return OnHScroll( hDlg, LOWORD(wParam), HIWORD(wParam), GetDlgCtrlID((HWND)lParam) );

		case WM_DRAWITEM:
			return OnDrawItem( hDlg, (WORD)wParam, (LPDRAWITEMSTRUCT)lParam );
	 }

	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


/// Executes 'Choose color' dialog box
// **************************************************************************

BOOL ChooseRGB( LPCHOOSERGB lpChooseRGB )
{
	return DialogBoxParam( g_hInstance, MAKEINTRESOURCE(IDD_RGB), 
				 lpChooseRGB->hwndParent, ChooseRGBDlgProc, (LPARAM)lpChooseRGB );
}