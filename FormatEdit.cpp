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
// formatedit.cpp 
// Source code of the format editing dialog box (IDD_FORMAT)
//


#include "stdafx.h"
#include "memstat.h"
#include "res/resource.h"


//- PRIVATE VARIABLES
//- =================


/// Elements definition:
static char*	m_pItems[] = STR_FORMAT_ITEMS };
static char 	m_pSymbols[] = { 'p', 's', 'P', 'S', 'u', 'b', 'B', 't', 'd', 'v', 'n' };

/// Other:
static char		m_pszFormat[256];			// temporary format string
static int		m_dwMode;					// mode (0-bytes,1-kb,2-mb,3-gb)
static HWND		m_hDlg;						// handle to dialog
static int		m_dwCursorPos;				// current position of carret


//- PRIVATE FUNCTIONS
//- =================


/// Converts bytes to desired units
static float ConvBytes( long bytes, int mode )
{
	if ( mode == 1 ) return bytes/(float)KB;
	if ( mode == 2 ) return bytes/(float)MB;
	if ( mode == 3 ) return bytes/(((float)MB*1024));
	return (float)bytes;
}


/// Converts number to desired units
static float ConvNum( long num, int mode )
{
	if ( mode == 1 ) return num/(float)1000;
	if ( mode == 2 ) return num/(float)1.0e6;
	if ( mode == 3 ) return num/(((float)1.0e9));
	return (float)num;
}


/// Updates preview
static void UpdatePreview()
{
	char str[512];
	FormatRecentSample( m_pszFormat, str );
	SDIM( m_hDlg, IDC_FORMAT_PREVIEW, WM_SETTEXT, 0, (LPARAM)str );
}


/// Adds selected item to the format string
static void AddItem()
{
	char itemCode[8];
	LONG selIndex;

	selIndex = SDIM( m_hDlg, IDC_ELEMENTS, LB_GETCURSEL, 0, 0 );
	if ( selIndex != LB_ERR )
	{
		// where to insert new item
		DWORD x1,x2;
		SDIM( m_hDlg, IDC_FORMAT, EM_GETSEL, (WPARAM)&x1, (LPARAM)&x2 );

		// adds '%'+[modifier:G/M/K]+item-character
		sprintf( itemCode, "$%s%c", m_dwMode==1?"K":m_dwMode==2?"M":m_dwMode==3?"G":"", m_pSymbols[selIndex] );
		SDIM( m_hDlg, IDC_FORMAT, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)itemCode );

		SDIM( m_hDlg, IDC_FORMAT, WM_GETTEXT, 255, (LPARAM)m_pszFormat );
		UpdatePreview();
		SetFocus( GetDlgItem( m_hDlg, IDC_FORMAT ) );
		//l = SDIM( m_hDlg, IDC_FORMAT, EM_LINELENGTH, 0, 0 );
		//SDIM( m_hDlg, IDC_FORMAT, EM_SETSEL, l, l );
	}
}


/// Handles WM_INITDIALOG message - initializes dialog controls
static void OnInitDialog( HWND hDlg )
{
	INT tabs[] = { 8 };
	m_hDlg = hDlg;

	CenterWindow( hDlg, TRUE );
	SDIM( hDlg, IDC_FORMAT, WM_SETTEXT, 0, (LPARAM)m_pszFormat );
	SDIM( hDlg, IDC_ELEMENTS, LB_SETTABSTOPS, 1, (LPARAM)&tabs );
	for( int i=0; i<STR_FORMAT_ITEMS_CNT; i++ )
		SDIM( hDlg, IDC_ELEMENTS, LB_ADDSTRING, 0, (LPARAM)m_pItems[i] );
	SDIM( hDlg, IDC_ELEMENTS, LB_SETCURSEL, 0, 0 );
	UpdatePreview();

	CheckRadioButton( hDlg, IDC_BYTES, IDC_GIGABYTES, IDC_BYTES );
	m_dwMode = 0;

	LONG l;
	SetFocus( GetDlgItem( hDlg, IDC_FORMAT ) );
	l = SDIM( hDlg, IDC_FORMAT, EM_LINELENGTH, 0, 0 );
	SDIM( hDlg, IDC_FORMAT, EM_SETSEL, l, l );
}


/// Handles WM_COMMAND message
static BOOL OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	switch ( wID )
	{
		case IDOK:
			SDIM( hDlg, IDC_FORMAT, WM_GETTEXT, 255, (LPARAM)m_pszFormat );
			EndDialog( hDlg, TRUE );
			break;

		case IDCANCEL:
			EndDialog( hDlg, FALSE );
			break;

		case IDC_FORMAT:
			if ( wNotifyCode == EN_CHANGE )
			{
				SDIM( hDlg, IDC_FORMAT, WM_GETTEXT, 255, (LPARAM)m_pszFormat );
				UpdatePreview();
			}
			break;

		case IDC_BYTES:
			if ( wNotifyCode == BN_CLICKED )
				m_dwMode = 0;
			break;

		case IDC_KILOBYTES:
			if ( wNotifyCode == BN_CLICKED )
				m_dwMode = 1;
			break;

		case IDC_MEGABYTES:
			if ( wNotifyCode == BN_CLICKED )
				m_dwMode = 2;
			break;

		case IDC_GIGABYTES:
			if ( wNotifyCode == BN_CLICKED )
				m_dwMode = 3;
			break;

		case IDC_ADDITEM:
			if ( wNotifyCode == BN_CLICKED )
				AddItem();
			break;

		case IDC_ELEMENTS:
			if ( wNotifyCode == LBN_DBLCLK )
				AddItem();
			break;

		default:
			return FALSE;
	}
	return TRUE;
}


/// Format editing dialog box message handler
static BOOL WINAPI EditFormatDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			SendMessage( hDlg, WM_SETTEXT, 0, (LPARAM)lParam );
			OnInitDialog( hDlg );
			break;

		case WM_COMMAND:
			if ( lParam != NULL )
				return OnCtrlCommand( hDlg, HIWORD(wParam), LOWORD(wParam), (HWND)lParam );
			break;
	 }

	return FALSE;
}


//- PUBLIC FUNCTIONS
//- ================


/// Formats recent monitoring sample using given format-string
void FormatRecentSample( char* pFormat, char* pDest )
{
	int i,l,n,mode;
	char timepDest[16], datepDest[16];
	MEMSTATITEM recentSample;

	// preparing items
	GetRecentSample( &recentSample );
	GetTimeFormat( NULL, 0, &recentSample.timeCurrent, "HH':'mm':'ss", timepDest, 15 );
	GetDateFormat( NULL, 0, &recentSample.timeCurrent, "dd'/'MM'/'yyyy", datepDest, 15 );

	// processing
	n = 0;
	l = strlen( pFormat );
	for( i=0; i<l; i++ )
		if ( n > (512-16) )
			break;
		else if ( pFormat[i] == '$' ) 
		{
			mode = 0;
			switch ( pFormat[++i] )
			{
			case 'K': mode = 1; break;
			case 'M': mode = 2; break;
			case 'G': mode = 3; break;
			default: i--; break;
			}
			
			switch ( pFormat[++i] )
			{
			case '$': pDest[n++] = '$'; break;
			case 'p': n += sprintf( &pDest[n], mode==0?"%.0f":"%.2f", ConvBytes(recentSample.dwAvailPhys,mode) ); break;
			case 's': n += sprintf( &pDest[n], mode==0?"%.0f":"%.2f", ConvBytes(recentSample.dwAvailPageFile,mode) ); break;
			case 'P': n += sprintf( &pDest[n], mode==0?"%.0f":"%.2f", ConvBytes(recentSample.dwTotalPhys-recentSample.dwAvailPhys,mode) ); break;
			case 'S': n += sprintf( &pDest[n], mode==0?"%.0f":"%.2f", ConvBytes(recentSample.dwTotalPageFile-recentSample.dwAvailPageFile,mode) ); break;
			case 'u': n += sprintf( &pDest[n], "%i", recentSample.dwMemoryLoad ); break;
			case 'b': n += sprintf( &pDest[n], mode==0?"%.0f":"%.2f", ConvNum(recentSample.dwTickCount,mode) ); break;
			case 'B': n += sprintf( &pDest[n], "%0.2i:%0.2i:%0.2i", recentSample.dwTickCount/3600000, (recentSample.dwTickCount-(recentSample.dwTickCount/3600000)*3600000)/60000, (recentSample.dwTickCount-(recentSample.dwTickCount/60000)*60000)/1000 ); break;
			case 't': n += sprintf( &pDest[n], "%s", timepDest ); break;
			case 'd': n += sprintf( &pDest[n], "%s", datepDest ); break;
			case 'v': n += sprintf( &pDest[n], "%s", MEMSTAT_VERSION_STR ); break;
			case 'n': n += sprintf( &pDest[n], "\n" ); break;
				break;
			}
		}
		else
			pDest[n++] = pFormat[i];

	// terminating destination string
	pDest[n] = 0;
}


/// Executes format edition dialog box for specified string
BOOL EditFormat( char* pszInOut, char* dialogCaption )
{
	strcpy( m_pszFormat, pszInOut );
	if ( DialogBoxParam( g_hInstance, MAKEINTRESOURCE(IDD_FORMAT), g_hMainWnd, EditFormatDlgProc, (LPARAM)dialogCaption ) )
	{
		strcpy( pszInOut, m_pszFormat );
		return TRUE;
	}
	return FALSE;
}	