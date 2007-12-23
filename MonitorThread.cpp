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
// monitorthread.cpp
// Memory monitoring thread [with logging]
//


#include "stdafx.h"
#include "memstat.h"


//- GLOBAL VARIABLES
//- ================


HANDLE					g_hMonitorThread;       // monitoring thread handle


//- PRIVATE VARIABLES
//- =================


static CRITICAL_SECTION m_csMSData;            // memory status access object
static CRITICAL_SECTION m_csMSL;
static BOOL            m_bMSDataLocked;        // TRUE if MSData locked
static MEMSTATITEM*    m_lpMemStats;           // mem-stats data (array)
static DWORD           m_dwMemStatsCount = MAX_SAMPLES; // num of elements in mem-stats
static DWORD           m_dwMemStatsCur;        // current item in mem-stats
static BOOL            m_bMemMonitoring;       // TRUE if monitor thread running
static BOOL            m_bLogging = FALSE;     // MSL running flag
static FILE           *m_fhLog;                // MSL logfile handle


//- PRIVATE FUNCTIONS
//- =================


/// Returns current size of the file
static long fsize(FILE* f)
{
	long save = ftell(f);
	fseek(f,0,SEEK_END);
	long size = ftell(f);
	fseek(f,save,SEEK_SET);
	return size;
}


/// Writes data to the log file
static void WriteToMSL()
{
	char str[512];

	EnterCriticalSection( &m_csMSL );
	if ( m_bLogging )
	{
		if ( g_Config.dwMSL_LimitSize )
			if ( (fsize( m_fhLog ) + 256) > (long)g_Config.dwMSL_LimitSize )
			{
				LeaveCriticalSection( &m_csMSL );
				DisableMSL();
				return;
			}

		FormatRecentSample( g_Config.pszMSL_LogFormat, str );
		fprintf( m_fhLog, "%s\n", str );

		// must be linked with COMMODE.OBJ to work!!!
		if ( g_Config.bDirectWriting )
			fflush( m_fhLog );
	}
	
	LeaveCriticalSection( &m_csMSL );
}


/// Memory monitoring thread
static DWORD WINAPI MonitorThreadProc( LPVOID lpParameter )
{
	m_bMemMonitoring = TRUE;

	while ( m_bMemMonitoring )
	{
		// gain information
		LockMSData( NULL, NULL );
		if ( ++m_dwMemStatsCur >= m_dwMemStatsCount )
			m_dwMemStatsCur = 0;
		GlobalMemoryStatus( (LPMEMORYSTATUS)&m_lpMemStats[m_dwMemStatsCur] );
		m_lpMemStats[m_dwMemStatsCur].dwTickCount = GetTickCount();
		GetLocalTime( &m_lpMemStats[m_dwMemStatsCur].timeCurrent );
	
		// wait some time
		UnlockMSData();
		WriteToMSL();
		Sleep( g_Config.dwInterval );
	}

	return TRUE;
}


//- PUBLIC FUNCTIONS
//- ================


// Creates suspended monitoring thread
void CreateMonitorThread()
{
	// initialize variables
	m_dwMemStatsCur = 0;
	m_lpMemStats = (MEMSTATITEM*) malloc( sizeof(MEMSTATITEM) * m_dwMemStatsCount );
	memset( m_lpMemStats, 0, sizeof(MEMSTATITEM) * m_dwMemStatsCount );
	InitializeCriticalSection( &m_csMSData );
	InitializeCriticalSection( &m_csMSL );
	GlobalMemoryStatus( (LPMEMORYSTATUS)&m_lpMemStats[0] );

	// create monitoring thread
	DWORD idMonThread;
	g_hMonitorThread = CreateThread( NULL, 0, MonitorThreadProc, NULL, 0, &idMonThread );
	if ( g_hMonitorThread == NULL ) 
		ExitError( STR_CANT_CREATE_MONITORING_THREAD );
}


/// Allows other thread to operate on memory status data
MEMSTATITEM* LockMSData( LPDWORD lpdwCount, LPDWORD lpdwCur )
{
	EnterCriticalSection( &m_csMSData );
	m_bMSDataLocked = TRUE;

	if ( lpdwCount != NULL )  *lpdwCount = m_dwMemStatsCount;
	if ( lpdwCur != NULL )    *lpdwCur = m_dwMemStatsCur;
	return m_lpMemStats;
}


/// Restore standard access to memory status data
void UnlockMSData()
{
	_ASSERT( m_bMSDataLocked );

	if ( m_bMSDataLocked )
	{
		m_bMSDataLocked = FALSE;
		LeaveCriticalSection( &m_csMSData );
	}
}


/// This one copy last monitoring sample to the specified location
void GetRecentSample( LPMEMSTATITEM pDest )
{
	DWORD dwCnt, dwCur;
	LPMEMSTATITEM pItems;

	pItems = LockMSData( &dwCnt, &dwCur );
	memcpy( pDest, &pItems[dwCur], sizeof(MEMSTATITEM) );
	UnlockMSData();
}


/// Starts memory status logging
BOOL EnableMSL()
{
	EnterCriticalSection( &m_csMSL );
	if ( m_bLogging )
	{
		LeaveCriticalSection( &m_csMSL );
		return FALSE;
	}

	// open file log
	if ( g_Config.bMSL_Append )
		m_fhLog = fopen( g_Config.pszMSL_LogName, "a" );
	else
		m_fhLog = fopen( g_Config.pszMSL_LogName, "w" );
	if ( m_fhLog == NULL )
		return FALSE;

	// check for size
	if ( g_Config.dwMSL_LimitSize )
		if ( (fsize( m_fhLog ) + 256) > (long)g_Config.dwMSL_LimitSize )
		{
			LeaveCriticalSection( &m_csMSL );
			fclose( m_fhLog );
			return FALSE;
		}

	// set logging flag
	m_bLogging = TRUE;

	// write header
	/* REMOVED
	SYSTEMTIME systime;
	char str[256];

	GetLocalTime( &systime );
	GetTimeFormat( NULL, 0, &systime,
		"H':'mm':'ss", str, 255 );
	fprintf( m_fhLog, "Start: %s ", str );
	GetDateFormat( NULL, 0, &systime,
		"d MMMM yyyy", str, 255 );
	fprintf( m_fhLog, "%s\n", str );
	*/

	LeaveCriticalSection( &m_csMSL );
	PostThreadMessage( g_idMainThread, WM_UPDATEMSLCTRLS, 0, 0 );
	return TRUE;
}


/// Stops logging
void DisableMSL()
{
	EnterCriticalSection( &m_csMSL );
	if ( !m_bLogging )
	{
		LeaveCriticalSection( &m_csMSL );
		return;
	}
	/* REMOVED
	SYSTEMTIME systime;
	char str[256];

	GetLocalTime( &systime );
	GetTimeFormat( NULL, 0, &systime,
		"H':'mm':'ss", str, 255 );
	fprintf( m_fhLog, "Stop: %s ", str );
	GetDateFormat( NULL, 0, &systime,
		"d MMMM yyyy", str, 255 );
	fprintf( m_fhLog, "%s\n", str );
	fclose( m_fhLog );
	*/
	m_bLogging = FALSE;

	LeaveCriticalSection( &m_csMSL );
	PostThreadMessage( g_idMainThread, WM_UPDATEMSLCTRLS, 0, 0 );
}


/// Returns TRUE if logging is enabled
BOOL IsMSLEnabled()
{
	return m_bLogging;
}