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
// memstat.cpp
// Main source file of Memory Status application
//


#include "stdafx.h"
#include "res\resource.h"
#include "memstat.h"


//- GLOBAL VARIABLES
//- ================


HINSTANCE					g_hInstance;			// holds app instance handle
HANDLE						g_hMainThread;			// main thread handle
DWORD						g_idMainThread;			// and ID
MEMORYSTATUS				g_msMemInfo;			// global memory information
char						g_lpszHelp[261];		// helpfile full path
HACCEL						g_hAccel;				// accelerators
OSVERSIONINFO				g_OSVersion;			// operating system version
DWORD						g_dwAmountDone;			// progress

_SetLayeredWindowProperties *SetLayeredWindowProperties_USER32;


/// MemStat settings:
MEMSTATCONFIG		g_Config = {
	TRUE,						// bAlwaysOnTop;			
	FALSE,						// bAutoYScale;			
	FALSE,						// bAutoDisabled;			
	KB*256,						// dwMinFree;				
	MB,							// dwFreeTo;				
	4*MB,						// dwManualFree;			
	0,							// dwAutoPriority;			
	1000,						// dwInterval;				
	0,							// dwChartHotkey;			
	TI_NUMERICAL,				// dwTrayIconStyle;		
	RGB(255,251,240),			// crChart;				
	RGB(10,36,106),				// crBackground;			
	RGB(166,202,240),			// crText;					
	RGB(0,106,175),				// crLines;				
	FALSE,						// bMSL_Append;			
	0,							// dwMSL_LimitSize;		
	TRUE,						// bAutoSave;				
	FALSE,						// bAutoMSL;				
	FALSE,						// bStartupFree;			
	4*MB,						// dwStartupFree;			
	0,							// dwFreeHotkey;			
	FALSE,						// bBlendChartWnd;			
	38,							// dwBlendFactor;			
	TRUE,						// bBusyIcon;				
	CWS_VISIBLE,				// dwChartState;
	RGB(255,251,240),			// crCaptionBg;				
	RGB(10,36,106),				// crCaptionText;				
	{0,0,0,0},					// posChartWnd;				
	{0,0,0,0},					// posMainWnd;				
	"memstat.log",				// pszMSL_LogName[261];	
	STR_DEFAULT_LOG_FORMAT,		// pszMSL_LogFormat[256];
	STR_DEFAULT_CAPTION_FORMAT, // pszCaptionFormat;
	STR_DEFAULT_TOOLTIP_FORMAT,	// pszTooltipFormat;
	TRUE,						// bLimitTime
	60,							// dwLimitTime
	3,							// dwMaxRetries
	FALSE						// bDirectWrite
};


/// registry entry map:
static MEMSTATREGENTRY m_regEntries[] = {
	{ REG_DWORD, "MinFree", &g_Config.dwMinFree, 0 },
	{ REG_DWORD, "AutoFree", &g_Config.dwFreeTo, 0 },
	{ REG_DWORD, "ManualFree", &g_Config.dwManualFree, 0 },
	{ REG_DWORD, "AutoDisabled", &g_Config.bAutoDisabled, 0 },
	{ REG_DWORD, "AutoPriority", &g_Config.dwAutoPriority, 0 },
	{ REG_DWORD, "Interval", &g_Config.dwInterval, 0 },
	{ REG_DWORD, "AlwaysOnTop", &g_Config.bAlwaysOnTop, 0 },
	{ REG_DWORD, "AutoYScale", &g_Config.bAutoYScale, 0 },
	{ REG_DWORD, "ChartColor", &g_Config.crChart, 0 },
	{ REG_DWORD, "BgChartColor", &g_Config.crBackground, 0 },
	{ REG_DWORD, "LineColor", &g_Config.crLines, 0 },
	{ REG_DWORD, "TextColor", &g_Config.crText, 0 },
	{ REG_DWORD, "Chart Hotkey", &g_Config.dwChartHotkey, 0 },
	{ REG_DWORD, "Tray style", &g_Config.dwTrayIconStyle, 0 },
	{ REG_SZ,    "MSL Logname", &g_Config.pszMSL_LogName, 0 },
	{ REG_SZ,    "MSL Logformat", &g_Config.pszMSL_LogFormat, 0 },
	{ REG_SZ,    "CaptionFormat", &g_Config.pszCaptionFormat, 0 },
	{ REG_SZ,    "TooltipFormat", &g_Config.pszTooltipFormat, 0 },
	{ REG_DWORD, "MSL Appending", &g_Config.bMSL_Append, 0 },
	{ REG_DWORD, "MSL Limit", &g_Config.dwMSL_LimitSize, 0 },
	{ REG_DWORD, "AutoMSL", &g_Config.bAutoMSL, 0 },
	{ REG_DWORD, "AutoSave", &g_Config.bAutoSave, 0 },
	{ REG_DWORD, "StartupFree", &g_Config.bStartupFree, 0 },
	{ REG_DWORD, "StartupFreeSize", &g_Config.dwStartupFree, 0 },
	{ REG_DWORD, "Free Hotkey", &g_Config.dwFreeHotkey, 0 },
	{ REG_DWORD, "BlendChartWnd", &g_Config.bBlendChartWnd, 0 },
	{ REG_DWORD, "BlendFactor", &g_Config.dwBlendFactor, 0 },
	{ REG_DWORD, "HourglassIcon", &g_Config.bBusyIcon, 0 },
	{ REG_DWORD, "ChartWndState", &g_Config.dwChartState, 0 },
	{ REG_DWORD, "CaptionBgColor", &g_Config.crCaptionBg, 0 },
	{ REG_DWORD, "CaptionTextColor", &g_Config.crCaptionText, 0 },
	{ REG_BINARY, "ChartWndPos", &g_Config.posChartWnd, 16 },
	{ REG_BINARY, "MainWndPos", &g_Config.posMainWnd, 16 },
	{ REG_DWORD, "TimeLimitOn", &g_Config.bLimitTime, 0 },
	{ REG_DWORD, "TimeLimitSeconds", &g_Config.dwLimitTime, 0 },
	{ REG_DWORD, "MaxRetries", &g_Config.dwMaxRetries, 0 },
	{ REG_DWORD, "DirectWriting", &g_Config.bDirectWriting, 0 }
};


//- PUBLIC FUNCTIONS
//- ================


/// Centers window (TRUE = in desktop, FALSE = in parent wnd)
void CenterWindow( HWND hWnd, BOOL bToDesktop )
{
	RECT rect,rect2;

	if ( bToDesktop )
		GetClientRect( GetDesktopWindow(), &rect );
	else
		GetClientRect( GetParent( hWnd ), &rect );
	GetWindowRect( hWnd, &rect2 );
	SetWindowPos( hWnd, 0, 
		((rect.right-rect.left)-(rect2.right-rect2.left))/2,
		((rect.bottom-rect.top)-(rect2.bottom-rect2.top))/2,
		0, 0, 
		SWP_NOSIZE|SWP_NOZORDER );
}


/// Positions window near the system tray
void TrayPosWindow( HWND hWnd )
{
	RECT rDesk, rWnd, rTray;
	DWORD dwW, dwH, dwX, dwY;
	HWND hwndTray;

	// measure window and tray
	GetWindowRect( GetDesktopWindow(), &rDesk );
	GetWindowRect( hWnd, &rWnd );
	dwW = rWnd.right - rWnd.left;
	dwH = rWnd.bottom - rWnd.top + 4;
	hwndTray = FindWindowEx( FindWindow( "Shell_TrayWnd", NULL ), NULL, "TrayNotifyWnd", NULL );
	if ( hwndTray == NULL )
		return;
	GetWindowRect( hwndTray, &rTray );

	// position window  
	dwX = ( rTray.left < (rDesk.right/2) ) ? rTray.left : rTray.right - dwW;
	dwY = ( rTray.top > (rDesk.bottom/2) ) ? rTray.top - dwH : rTray.bottom + 4;
	SetWindowPos( hWnd, 0, dwX, dwY, 0, 0, SWP_NOSIZE|SWP_NOZORDER );
}


/// Returns TRUE if autostart registry entry exist
BOOL IsAutostartEnabled()
{
	BOOL bRetVal;
	HKEY Key;
	char vname[64];
	DWORD vname_size;
	DWORD value_type,i;
	char value[256];
	DWORD value_size;
	LONG status;

	bRetVal = FALSE;
	if ( RegOpenKeyEx( HKEY_CURRENT_USER,
				"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
				0, KEY_ALL_ACCESS, &Key ) == ERROR_SUCCESS )
	{
		i = 0;
		do 
		{
			vname_size = 64;
			value_size = 256;
			status = RegEnumValue( Key, i, (LPTSTR)vname, &vname_size, NULL, 
														 &value_type, (LPBYTE)value, &value_size);
			if ( status == ERROR_SUCCESS )
			{
				if ( strcmp( "MemStat", vname ) == 0 )
					bRetVal = TRUE;
			}
			i++;
		} while ( status == ERROR_SUCCESS );
		
		RegCloseKey( Key );
	}

	return bRetVal;
}


/// Enables or disables autostart feature
void EnableAutostart( BOOL bEnable )
{
	HKEY MemStatKey;
	DWORD whathappened;

	
	if (RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
											NULL, NULL, REG_OPTION_NON_VOLATILE,
											KEY_SET_VALUE, NULL, &MemStatKey, &whathappened ) == ERROR_SUCCESS )
	{
		if ( bEnable )
			RegSetValueEx( MemStatKey, "MemStat", 0, REG_SZ, (LPBYTE)__argv[0], strlen(__argv[0]));
		else
			RegDeleteValue( MemStatKey, "MemStat" );
	}
	RegCloseKey( MemStatKey );
}


/// Loads configuration from the system registry
void LoadConfig( LPMEMSTATCONFIG lpConfig )
{
	HKEY MemStatKey;
	char vname[64];
	DWORD vname_size;
	DWORD value_type,i;
	char value[256];
	DWORD value_size;
	LONG status;

	i = 0;
	GlobalMemoryStatus(&g_msMemInfo);
	if ( lpConfig == NULL )
		lpConfig = &g_Config;

	g_Config.dwMinFree = (DWORD)(g_msMemInfo.dwTotalPhys/MB * 0.05f) * MB;
	g_Config.dwFreeTo = (DWORD)(g_msMemInfo.dwTotalPhys/MB * 0.10f) * MB;
	g_Config.dwManualFree = (DWORD)(g_msMemInfo.dwTotalPhys/MB * 0.02f) * MB;

	g_Config.crCaptionBg = GetSysColor( COLOR_BTNFACE );
	g_Config.crCaptionText = GetSysColor( COLOR_BTNTEXT );
	g_Config.crBackground = GetSysColor( COLOR_BTNSHADOW );
	g_Config.crChart = GetSysColor( COLOR_BTNTEXT );
	g_Config.crLines = GetSysColor( COLOR_BTNFACE );
	g_Config.crText = GetSysColor( COLOR_BTNHIGHLIGHT );

	// values loading from registry
	if ( RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\MemStat", 0, KEY_ALL_ACCESS, &MemStatKey ) 
			 == ERROR_SUCCESS )
	{
		do 
		{
			vname_size = 64;
			value_size = 256;
			status = RegEnumValue( MemStatKey, i, (LPTSTR)vname, &vname_size, NULL, 
														 &value_type, (LPBYTE)value, &value_size);
			if ( status == ERROR_SUCCESS )
			{
				for( DWORD n=0; n<(sizeof(m_regEntries)/sizeof(MEMSTATREGENTRY)); n++ )
					if ( stricmp( m_regEntries[n].name, vname ) == 0 )
						if ( m_regEntries[n].type == REG_DWORD )
							*((LPDWORD)(m_regEntries[n].where)) = *((DWORD*)&value);
						else if ( m_regEntries[n].type == REG_SZ )
							strcpy( (char*)m_regEntries[n].where, value );
						else if ( m_regEntries[n].type == REG_BINARY )
							memcpy( m_regEntries[n].where, &value, m_regEntries[n].size );
			}
			i++;
		} while ( status == ERROR_SUCCESS );

		RegCloseKey( MemStatKey );
	}

	// value checking
	if ( lpConfig->dwFreeTo > g_msMemInfo.dwTotalPhys )
		lpConfig->dwFreeTo = 0;
	if ( lpConfig->dwManualFree > g_msMemInfo.dwTotalPhys)
		lpConfig->dwManualFree = 0;
	if ( lpConfig->dwMinFree > lpConfig->dwFreeTo )
		lpConfig->dwMinFree = 0;
	if ( lpConfig->dwAutoPriority > 2)
		lpConfig->dwAutoPriority = 0;
	if ( lpConfig->dwInterval < MIN_INTERVAL )
		lpConfig->dwInterval = DEFAULT_INTERVAL;
	if ( lpConfig->dwTrayIconStyle > TI_NUMERICAL2 )
		lpConfig->dwTrayIconStyle = TI_STANDARD;
}


/// Saves config to the system registry
void SaveConfig( LPMEMSTATCONFIG lpConfig )
{
	HKEY MemStatKey;
	DWORD whathappened;

	if ( lpConfig == NULL )
		lpConfig = &g_Config;

	if (RegCreateKeyEx( HKEY_CURRENT_USER, "Software\\MemStat", NULL, NULL, REG_OPTION_NON_VOLATILE,
											KEY_ALL_ACCESS, NULL, &MemStatKey, &whathappened ) == ERROR_SUCCESS )
	{
		for( DWORD n=0; n<(sizeof(m_regEntries)/sizeof(MEMSTATREGENTRY)); n++ )
			RegSetValueEx( MemStatKey, m_regEntries[n].name, 0, m_regEntries[n].type, (LPBYTE)m_regEntries[n].where, 
			m_regEntries[n].type==REG_SZ ? strlen((char*)m_regEntries[n].where) : 
			m_regEntries[n].size==0?4:m_regEntries[n].size);
		RegCloseKey( MemStatKey );
	}
}


/// Exits instantly with error message
void ExitError( char *lpszMessage )
{
	MessageBox( NULL, lpszMessage, STR_ERROR, MB_OK | MB_ICONHAND );
	if ( g_hMainWnd )
		DestroyWindow( g_hMainWnd );
	exit(127);
}


/// Inits current program instace
BOOL InitInstance( HINSTANCE hInst )
{
	SECURITY_ATTRIBUTES sa;

	// kill previous instance of memstat (restart)
	HWND hWnd = FindWindow( "MemoryChartParentWnd", NULL );
	if ( hWnd == NULL ) FindWindow( "MemoryChartWnd", NULL );
	SendMessage( GetParent( hWnd ), WM_DESTROY, 0, 0 );
	if ( __argc > 1 )
	{
		// quit if user requests only to quit ('off' command line parameter)
		if ( stricmp( __argv[1], "off" ) == 0 )
			exit(0);
	}

	// save instance
	g_hInstance = hInst;

	// version stuff
	memset( &g_OSVersion, 0, sizeof(g_OSVersion) );
	g_OSVersion.dwOSVersionInfoSize = sizeof(g_OSVersion);
	GetVersionEx( &g_OSVersion );

	// init libraries
	InitCommonControls();

	// get special pointers:
	HMODULE userDll = LoadLibrary( "USER32.DLL" );
	SetLayeredWindowProperties_USER32 = (_SetLayeredWindowProperties*)GetProcAddress( userDll, "SetLayeredWindowAttributes" );

	// get current thread id (for future message posting)
	g_idMainThread = GetCurrentThreadId();

	// make help filename with path
	int i;
	strcpy( g_lpszHelp, __argv[0] );
	i = strlen( __argv[0] );
	while ( i>0 )
		if ( g_lpszHelp[--i] == '\\' ) break;
	g_lpszHelp[++i] = 0;
	strcat( g_lpszHelp, "memstat.hlp" );

	// mutex stuff
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
//	CreateMutex( &sa, TRUE, "memstat_mutex" );
	return FALSE;//GetLastError() == ERROR_ALREADY_EXISTS;
}


/// Runs process message loop
int	Run()
{
	MSG msg;

	do
	{

		if ( GetMessage( &msg, NULL, 0, 0 ) )
		{
			// check for update messages
			if ( ( msg.hwnd == NULL ) && ( msg.message == WM_UPDATEMSLCTRLS ) )
				PostMessage( g_hwcpMonitoring, msg.message, 0, 0 );

			// check if dialog msg
			if ( IsDialogMessage( g_hMainWnd, &msg) )
				continue;

			// if not process it
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

	} while (msg.message != WM_QUIT);

	return msg.wParam;
}


//- ENTRY-POINT
//- ===========


int	APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// save current instance and check it
	if ( InitInstance( hInstance ) )
		return 0;

	// create main window
	if ( !CreateMainWnd() )
		return 0;

	// and finally run all
	return Run();
}