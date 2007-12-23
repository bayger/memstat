/*
** Copyright (C) 1999 Patryk Bajer (a.k.a. Bayger)
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

// Root header of MemStat application

#ifndef	__MEMSTAT_H__
#define	__MEMSTAT_H__


// languages
#ifdef MEMSTAT_LANG_US
#include "LangUS.h"
#endif

#ifdef MEMSTAT_LANG_PL
#include "LangPL.h"
#endif


// sizes constants:
#define	KB                    1024
#define	KB64                  (64*KB)
#define	MB                    (1024*KB)

// custom app-messages:
#define	WM_TRAYICON           (WM_APP + 1)
#define	WM_INTERVALCHANGED    (WM_APP + 2)
#define	WM_UPDATEMSLCTRLS     (WM_APP + 3)
#define	WM_CONFIGCHANGE       (WM_APP + 4)

// tray-icon styles
#define	TI_STANDARD           0
#define	TI_FREEBAR            1
#define	TI_MINICHART          2
#define TI_NUMERICAL		  3
#define TI_NUMERICAL2		  4

// chart window states
#define CWS_VISIBLE				1
#define CWS_WINDOWSHADE			2
#define CWS_CHART_AUTOHIDE		3
#define CWS_DOCKED				4

// others:
#define	MIN_INTERVAL		100
#define	DEFAULT_INTERVAL	1000
#define	MSG_OK				0
#define	MSG_CANCEL			-1
#define	HK_TOGGLECHART		1
#define	HK_FREENOW			2
#define MAX_SAMPLES			2048

#define	SDIM SendDlgItemMessage
#define CLICKBTN( hDlg, wID ) SendMessage( hDlg, WM_COMMAND, MAKELONG(wID,BN_CLICKED), (LPARAM)GetDlgItem( hDlg, wID ) );
#define	RedrawDlgItem( hWnd, wID ) RedrawWindow(GetDlgItem(hWnd,wID),NULL,NULL,RDW_INVALIDATE)


/// Special structure that holds window position and size
typedef struct _MEMSTATWINDOWPOS
{
	LONG			dwPositionX;
	LONG			dwPositionY;
	LONG			dwWidth;
	LONG			dwHeight;
} MEMSTATWNDPOS;


/// MemStat XP Configuration structure
typedef	struct _MEMSTATCONFIG
{
	BOOL			bAlwaysOnTop;			// chart window always on top (AOT) mode
	BOOL			bAutoYScale;			// scale Y-values dynamically in chart
	BOOL			bAutoDisabled;			// TRUE if auto freeing is disabled
	DWORD			dwMinFree;				// min.amount of free physical memory (p.m.)
	DWORD			dwFreeTo;				// desired amount of free p.m.
	DWORD			dwManualFree;			// amount of free p.m. to free manually
	DWORD			dwAutoPriority;			// actually it is p.m. freeing priority
	DWORD			dwInterval;				// samplerate of monitoring
	DWORD			dwChartHotkey;			// chart window hot key
	DWORD			dwTrayIconStyle;		// style of tray icon (see TI_* defs)
	COLORREF		crChart;				// color of chart
	COLORREF		crBackground;			// color of chart background
	COLORREF		crText;					// color of text labels
	COLORREF		crLines;				// color of horiz. lines
	BOOL			bMSL_Append;			// logfile appending mode (no overwrite)
	DWORD			dwMSL_LimitSize;		// logfile limit (0 = no limit)
	BOOL			bAutoSave;				// TRUE if program saves cfg at exit
	BOOL			bAutoMSL;				// TRUE if program run MSL at startup
	BOOL			bStartupFree;			// TRUE if program frees p.m. ar startup
	DWORD			dwStartupFree;			// amount of p.m. to free at startup
	DWORD			dwFreeHotkey;			// manual-free hotkey
	BOOL			bBlendChartWnd;			// chart window blending (yes/no)
	DWORD			dwBlendFactor;			// blending factor
	BOOL			bBusyIcon;				// shows hourglass icon when program is busy
	BOOL			dwChartState;			// state of the chart window (bit 0: visible, bit 1: caption only)
	COLORREF		crCaptionBg;			// color of caption background
	COLORREF		crCaptionText;			// color of caption text
	MEMSTATWNDPOS	posChartWnd;			// position and size of chart window
	MEMSTATWNDPOS	posMainWnd;				// position of main window
	char			pszMSL_LogName[261];	// filename of logfile
	char			pszMSL_LogFormat[256];	// log format
	char			pszCaptionFormat[256];	// caption format
	char			pszTooltipFormat[256];	// tooltip format
	BOOL			bLimitTime;				// is time limit enabled
	DWORD			dwLimitTime;			// time limit in seconds
	DWORD			dwMaxRetries;			// max numer of retries
	BOOL			bDirectWriting;			// TRUE if direct write mode
}	MEMSTATCONFIG, *LPMEMSTATCONFIG;


/// configuration entry mapping structure
typedef struct _MEMSTATREGENTRY
{
	DWORD		type;
	LPSTR		name;
	PVOID		where;
	DWORD		size;
} MEMSTATREGENTRY;


/// monitoring sample data
typedef struct _MEMSTATITEM
{
	// copy of MEMORYSTATUS struct
	DWORD		dwLength;
	DWORD		dwMemoryLoad;
	SIZE_T		dwTotalPhys; 
	SIZE_T		dwAvailPhys; 
	SIZE_T		dwTotalPageFile; 
	SIZE_T		dwAvailPageFile; 
	SIZE_T		dwTotalVirtual; 
	SIZE_T		dwAvailVirtual; 
	// and additional fields:
	DWORD		dwTickCount;		// boot up tick count
	SYSTEMTIME	timeCurrent;		// current time and date
} MEMSTATITEM, *LPMEMSTATITEM;


// imported funtion types:
typedef	BOOL WINAPI _SetLayeredWindowProperties(HWND, COLORREF, BYTE, DWORD);


extern HINSTANCE       g_hInstance;
extern HANDLE          g_hMainThread;
extern DWORD           g_idMainThread;
extern MEMORYSTATUS    g_msMemInfo;
extern MEMSTATCONFIG   g_Config;
extern char            g_lpszHelp[261];
extern HACCEL          g_hAccel;
extern OSVERSIONINFO   g_OSVersion;
extern DWORD           g_dwAmountDone;

extern _SetLayeredWindowProperties *SetLayeredWindowProperties_USER32;


void CenterWindow( HWND hWnd, BOOL bToDesktop );
void TrayPosWindow( HWND hWnd );
BOOL IsAutostartEnabled();
void EnableAutostart( BOOL bState );
void SaveConfig( LPMEMSTATCONFIG lpConfig );
void LoadConfig( LPMEMSTATCONFIG lpConfig );
void ExitError( LPSTR lpszMessage );


//- ALL INCLUDES
//- ============


#include "Version.h"
#include "ChooseRGB.h"
#include "FormatEdit.h"

#include "FreeThread.h"
#include "MonitorThread.h"

#include "MainWnd.h"
#include "ChartParentWnd.h"
#include "ChartWnd.h"
#include "CfgFreeing.h"
#include "CfgMonitoring.h"
#include "CfgAppearance.h"
#include "CfgAdvanced.h"
#include "CfgAbout.h"


#endif