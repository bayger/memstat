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
// freethread.cpp
// Memory freeing thread code
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- GLOBAL VARIABLES
//- ================


HANDLE					g_hFreeThread;          // handle to free-thread


//- PRIVATE VARIABLES
//- =================


static CRITICAL_SECTION	m_csFreeing;			// critical section
static BOOL				m_bIsFreeing;			// TRUE if thread is busy now
static BOOL				m_bShouldTerminate;		// TRUE if thread should terminate
static DWORD			m_dwAmount;				// amount of memory to free
static DWORD			m_dwTimes;				// how many times do it?
static BOOL				m_bCanFree;				// if TRUE program can free memory
static DWORD			m_dwStartTick;			// freeing start time

//- PRIVATE FUNCTIONS
//- =================


// Memory freeing thread procedure
static DWORD WINAPI FreeThreadProc( LPVOID lpParam )
{
	char* pChunk;
	DWORD i,n,d;

	m_bIsFreeing = FALSE;
	while ( !m_bShouldTerminate )
	{
		// first suspend it 
		m_bCanFree = TRUE;
		LeaveCriticalSection( &m_csFreeing );
		SuspendThread( g_hFreeThread );

		// free mem!
		n = m_dwTimes*m_dwAmount;
		for ( ; m_dwTimes > 0 && m_bCanFree; m_dwTimes-- )
		{
			// should we do it one more time?
			MEMORYSTATUS ms;
			GlobalMemoryStatus( &ms );
			if ( m_dwAmount <= ms.dwAvailPhys )
				break;

			//malloc doesn't work for big memory regions (>256MB)
			pChunk = (char*)VirtualAlloc( NULL, m_dwAmount, MEM_COMMIT, PAGE_READWRITE );
			if ( pChunk != NULL )
			{
				d = 0;
				// 1st pass
				for ( i = 0; i < m_dwAmount && m_bCanFree; g_dwAmountDone = (DWORD)(((float)d/m_dwAmount)*1000.0f), d += 2048, i += 4096 )
					pChunk[i] = 0;
				// 2nd pass
				for ( i = 2048; i < m_dwAmount && m_bCanFree; g_dwAmountDone = (DWORD)(((float)d/m_dwAmount)*1000.0f), d += 2048, i += 4096 )
					pChunk[i] = 0;
				VirtualFree( pChunk, 0, MEM_RELEASE );
				free( pChunk );
			}
			else
				// TODO: signal that there is no memory, but somehow different.
				ExitError( STR_NO_MEMORY );
			
		}
		g_dwAmountDone = 0;
		EnterCriticalSection( &m_csFreeing );
		m_bIsFreeing = FALSE;
	}

	return 0;
}


//- PUBLIC FUNCTIONS
//- ================


/// Creates and runs memory freeing thread
void CreateFreeThread()
{
	DWORD id;

	InitializeCriticalSection( &m_csFreeing );

	EnterCriticalSection( &m_csFreeing );
	g_hFreeThread = CreateThread( NULL, 0, FreeThreadProc, NULL, 0, &id );

	if ( g_hFreeThread == NULL )
		ExitError( STR_CANT_CREATE_FREEING_THREAD );
}


/// Frees memory if free-thread not busy
BOOL TryFree( DWORD dwSize, DWORD dwTimes )
{
	// check if thread is not busy
	EnterCriticalSection( &m_csFreeing );  
	if ( m_bIsFreeing )
	{
		LeaveCriticalSection( &m_csFreeing );
		return FALSE;
	}

	if ( dwSize == 0 )
	{
		LeaveCriticalSection( &m_csFreeing );
		return TRUE;
	}

	SDIM( g_hwcpFreeing, IDC_FREENOW, WM_SETTEXT, 0, (LPARAM)STR_BUSY );
	EnableWindow( GetDlgItem( g_hwcpFreeing, IDC_FREENOW ), FALSE );
	m_bIsFreeing = TRUE;
	m_dwAmount = dwSize;
	m_dwTimes = g_Config.dwMaxRetries;
	g_dwAmountDone = 1;
	ShowWindow( GetDlgItem(g_hwcpFreeing,IDC_FREEING), SW_SHOW );
	LeaveCriticalSection( &m_csFreeing );
	m_dwStartTick = GetTickCount();

	// resume thread (free memory)
	while ( ResumeThread( g_hFreeThread ) > 1 ) ;
	return TRUE;
}


/// Frees memory up by dwSize
BOOL TryFreeUp( DWORD dwSize, DWORD dwTimes )
{
	// check for free memory first
	MEMORYSTATUS localMS;
	DWORD dwCurFree, dwFreeTo;
	GlobalMemoryStatus( &localMS );
	dwCurFree = localMS.dwAvailPhys;
	dwFreeTo = dwCurFree + dwSize;
	if ( dwFreeTo > localMS.dwTotalPhys )
	{
		char str[256];
		sprintf( str, STR_CURRENTLY_CANT_FREE,
			dwCurFree/(float)MB,
			dwFreeTo/(float)MB,
			localMS.dwTotalPhys/(float)MB );
		MessageBox( GetForegroundWindow(), str, STR_MESSAGE, MB_ICONINFORMATION|MB_APPLMODAL|MB_OK );
		return FALSE;
	}
	
	return TryFree( dwFreeTo, dwTimes );
}


/// Simply cancels current freeing operation
void CancelFreeing()
{
	m_bCanFree = FALSE;
}


/// Changes priority of freeing thread
void UpdateFreePriority()
{
	int iPriority;

	switch ( g_Config.dwAutoPriority )
	{
		case 0:
			iPriority = THREAD_PRIORITY_IDLE;
			break;
		case 1:
			iPriority = THREAD_PRIORITY_NORMAL;
			break;
		case 2:
			iPriority = THREAD_PRIORITY_HIGHEST;
			break;
	}
	SetThreadPriority( g_hFreeThread, iPriority );
}


/// Checks freeing time
void CheckFreeingTime()
{
	if ( g_Config.bLimitTime && m_bIsFreeing )
	{
		DWORD curTime;
		curTime = GetTickCount()-m_dwStartTick;
		if ( curTime > (1000*g_Config.dwLimitTime) )
			m_bCanFree = FALSE;
	}
}