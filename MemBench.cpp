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
// membench.cpp
// Memory benchmark code
//


#include "stdafx.h"
#include "memstat.h"
#include "res\resource.h"


//- PRIVATE VARIABLES
//- =================


static HWND		m_hBenchWnd;		// bench-window handle
static DWORD	m_dwNumPasses;		// number of passes
static DWORD	m_dwSize;			// test memory size


//- PRIVATE FUNCTIONS
//- =================


/// Tests Doubled 32-bit stream reading
static void Bench_I_Read( LPVOID pBuffer )
{
	DWORD time, totalTime;
	DWORD curPass;
	char str[64];

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_DOUBLEDREADING );
	// init timers
	totalTime = 0;
	for( curPass = 0; curPass < m_dwNumPasses; curPass++ )
	{
		time = timeGetTime();
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			mov esi, pBuffer
			mov edi, pBuffer
			add edi, 4
			shr ecx, 3
			align 16
loop_read:
			mov eax,[esi]
			mov ebx,[edi]
			add esi, 8
			add edi, 8
			dec ecx
			jnz loop_read
			
			popad
		}
		totalTime += timeGetTime()-time;
		
		// update progress bar
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 
			SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_GETPOS, 0, 0 )+1, 
			0 );
	}

	// calculate bandwidth
	float bandwidth = (((float)m_dwSize*m_dwNumPasses)/MB)/(totalTime*0.001f);
	sprintf( str, "%.2f", bandwidth );
	SDIM( m_hBenchWnd, IDC_I_READ, WM_SETTEXT, 0, (LPARAM)str );
}


/// Tests Doubled 32-bit stream writing
static void Bench_I_Write( LPVOID pBuffer )
{
	DWORD time, totalTime;
	DWORD curPass;
	char str[64];

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_DOUBLEDWRITING );
	// init timers
	totalTime = 0;
	for( curPass = 0; curPass < m_dwNumPasses; curPass++ )
	{
		time = timeGetTime();
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			mov edi, pBuffer
			mov esi, pBuffer
			add esi, 4
			shr ecx, 3
loop_write:
			mov [edi],eax
			mov [esi],eax
			add edi, 8
			add esi, 8
			dec ecx
			jnz loop_write

			popad
		}	
		totalTime += timeGetTime()-time;
		
		// update progress bar
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 
			SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_GETPOS, 0, 0 )+1, 
			0 );
	}

	// calculate bandwidth
	float bandwidth = (((float)m_dwSize*m_dwNumPasses)/MB)/(totalTime*0.001f);
	sprintf( str, "%.2f", bandwidth );
	SDIM( m_hBenchWnd, IDC_I_WRITE, WM_SETTEXT, 0, (LPARAM)str );
}


/// Tests Doubled 32-bit stream copying
static void Bench_I_Copy( LPVOID pBuffer )
{
	DWORD time, totalTime;
	DWORD curPass;
	char str[64];

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_DOUBLEDCOPYING );
	// init timers
	totalTime = 0;
	for( curPass = 0; curPass < m_dwNumPasses; curPass++ )
	{
		time = timeGetTime();
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			shr ecx, 1
			mov esi, pBuffer
			mov edi, pBuffer
			add edi, ecx
			xor eax, eax
			shr ecx, 3
loop_copy:
			mov eax,[esi]
			mov ebx,[esi+4]
			add esi,8
			mov [edi],eax
			mov [edi+4],ebx
			add edi,8
			dec ecx
			jnz loop_copy
				
			popad
		}
		totalTime += timeGetTime()-time;
		
		// update progress bar
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 
			SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_GETPOS, 0, 0 )+1, 
			0 );
	}

	// calculate bandwidth
	float bandwidth = (((float)m_dwSize*m_dwNumPasses)/MB)/(totalTime*0.001f);
	sprintf( str, "%.2f", bandwidth );
	SDIM( m_hBenchWnd, IDC_I_MOVE, WM_SETTEXT, 0, (LPARAM)str );
}


/// Tests Linear 32-bit stream reading
static void Bench_L_Read( LPVOID pBuffer )
{
	DWORD time, totalTime;
	DWORD curPass;
	char str[64];

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_LINEARREADING );
	// init timers
	totalTime = 0;
	for( curPass = 0; curPass < m_dwNumPasses; curPass++ )
	{
		time = timeGetTime();
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			mov esi, pBuffer
			shr ecx, 2
			align 16
loop_read:
			mov eax,[esi]
			add esi, 4
			dec ecx
			jnz loop_read
			
			popad
		}
		totalTime += timeGetTime()-time;
		
		// update progress bar
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 
			SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_GETPOS, 0, 0 )+1, 
			0 );
	}

	// calculate bandwidth
	float bandwidth = (((float)m_dwSize*m_dwNumPasses)/MB)/(totalTime*0.001f);
	sprintf( str, "%.2f", bandwidth );
	SDIM( m_hBenchWnd, IDC_L_READ, WM_SETTEXT, 0, (LPARAM)str );
}


/// Tests Linear 32-bit stream writing
static void Bench_L_Write( LPVOID pBuffer )
{
	DWORD time, totalTime;
	DWORD curPass;
	char str[64];

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_LINEARWRITING );
	// init timers
	totalTime = 0;
	for( curPass = 0; curPass < m_dwNumPasses; curPass++ )
	{
		time = timeGetTime();
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			mov edi, pBuffer
			shr ecx, 2
loop_write:
			mov [edi],eax
			add edi, 4
			dec ecx
			jnz loop_write

			popad
		}	
		totalTime += timeGetTime()-time;
		
		// update progress bar
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 
			SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_GETPOS, 0, 0 )+1, 
			0 );
	}

	// calculate bandwidth
	float bandwidth = (((float)m_dwSize*m_dwNumPasses)/MB)/(totalTime*0.001f);
	sprintf( str, "%.2f", bandwidth );
	SDIM( m_hBenchWnd, IDC_L_WRITE, WM_SETTEXT, 0, (LPARAM)str );
}


/// Tests Linear 32-bit stream copying
static void Bench_L_Copy( LPVOID pBuffer )
{
	DWORD time, totalTime;
	DWORD curPass;
	char str[64];

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_LINEARCOPYING);
	// init timers
	totalTime = 0;
	for( curPass = 0; curPass < m_dwNumPasses; curPass++ )
	{
		time = timeGetTime();
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			shr ecx, 1
			mov esi, pBuffer
			mov edi, pBuffer
			add edi, ecx
			xor eax, eax
			shr ecx, 2
loop_copy:
			mov eax,[esi]
			add esi,4
			mov [edi],eax
			add edi,4
			dec ecx
			jnz loop_copy
				
			popad
		}
		totalTime += timeGetTime()-time;
		
		// update progress bar
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 
			SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_GETPOS, 0, 0 )+1, 
			0 );
	}

	// calculate bandwidth
	float bandwidth = (((float)m_dwSize*m_dwNumPasses)/MB)/(totalTime*0.001f);
	sprintf( str, "%.2f", bandwidth );
	SDIM( m_hBenchWnd, IDC_L_MOVE, WM_SETTEXT, 0, (LPARAM)str );
}


/// Simply does benchmark with selected parameters
static void DoBenchmark()
{
	DWORD dwProgressMax;
	HANDLE curProcess;

	// truncate and check size (must be not less than 4 MB)
	m_dwSize = m_dwSize & 0xfff00000;
	if ( m_dwSize < 4*MB )
	{
		MessageBox( GetForegroundWindow(), STR_BENCH_LOWMEMORY, STR_MESSAGE, MB_OK|MB_ICONINFORMATION );
		return;
	}

	if ( MessageBox( GetForegroundWindow(), STR_BENCH_WARNING, STR_WARNING, MB_YESNO|MB_ICONWARNING ) == IDNO )
		return;

	RedrawWindow( m_hBenchWnd, NULL, NULL, RDW_UPDATENOW );

	// switch priority to realtime
	curProcess = GetCurrentProcess();
	SetPriorityClass( curProcess, REALTIME_PRIORITY_CLASS );

	// allocate memory buffer (truncated to 1MB size)
	char *buffer;
	buffer = (char*)VirtualAlloc( NULL, m_dwSize, MEM_COMMIT, PAGE_READWRITE );

	dwProgressMax = 1 + m_dwNumPasses*6;

	// benchmark
	if ( buffer != NULL )
	{
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETRANGE, 0, MAKELONG(0,dwProgressMax) );
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 0, 0 );
		SDIM( m_hBenchWnd, IDC_I_READ, WM_SETTEXT, 0, (LPARAM)"..." );
		SDIM( m_hBenchWnd, IDC_I_WRITE, WM_SETTEXT, 0, (LPARAM)"..." );
		SDIM( m_hBenchWnd, IDC_I_MOVE, WM_SETTEXT, 0, (LPARAM)"..." );
		SDIM( m_hBenchWnd, IDC_L_READ, WM_SETTEXT, 0, (LPARAM)"..." );
		SDIM( m_hBenchWnd, IDC_L_WRITE, WM_SETTEXT, 0, (LPARAM)"..." );
		SDIM( m_hBenchWnd, IDC_L_MOVE, WM_SETTEXT, 0, (LPARAM)"..." );

		EnableWindow( GetDlgItem(m_hBenchWnd,IDOK), FALSE );
		EnableWindow( GetDlgItem(m_hBenchWnd,IDCANCEL), FALSE );

		TIMECAPS tc;
		timeGetDevCaps( &tc, sizeof(tc) );
		timeBeginPeriod( tc.wPeriodMin );
		SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_BENCH_PREPARING );
		// prepare buffer
		_asm
		{
			pushad
			mov	ecx, m_dwSize
			mov edi, buffer
			shr ecx, 2
			rep stosd
			mov	ecx, m_dwSize
			mov esi, buffer
			shr ecx, 2
			rep lodsd
			popad
		}
		SDIM( m_hBenchWnd, IDC_BENCH_DONE, PBM_SETPOS, 1, 0 );

		Bench_I_Read( buffer );
		Bench_I_Write( buffer );
		Bench_I_Copy( buffer );
		Bench_L_Read( buffer );
		Bench_L_Write( buffer );
		Bench_L_Copy( buffer );

		timeEndPeriod( tc.wPeriodMin );

		EnableWindow( GetDlgItem(m_hBenchWnd,IDOK), TRUE );
		EnableWindow( GetDlgItem(m_hBenchWnd,IDCANCEL), TRUE );
	}
	else
		MessageBox( GetForegroundWindow(), STR_BENCH_NO_MEMORY, STR_ERROR, MB_OK|MB_ICONERROR );

	SDIM( m_hBenchWnd, IDC_BENCH_OP, WM_SETTEXT, 0, (LPARAM)STR_DONE );
	SetPriorityClass( curProcess, NORMAL_PRIORITY_CLASS );

	// release memory
	if ( buffer != NULL )
		VirtualFree( buffer, 0, MEM_RELEASE );
}


/// Handles all dialog controls commands sent by WM_COMMAND msg
static void OnCtrlCommand( HWND hDlg, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
	BOOL bReady;

	switch ( wID )
	{
	case IDCANCEL:
		if ( wNotifyCode == BN_CLICKED )
			EndDialog( hDlg, TRUE );
		break;

	case IDOK:
		bReady = TryFree( 0, 0 );
		if ( !bReady )
			MessageBox( GetForegroundWindow(), STR_BENCH_CANT, STR_MESSAGE, MB_OK|MB_ICONINFORMATION );
		else
		{
			MEMORYSTATUS localMS;
			GlobalMemoryStatus( &localMS );
			m_dwSize = (DWORD)((SDIM( hDlg, IDC_BENCH_PERC, TBM_GETPOS, 0, 0 ) / 1000.0f) * localMS.dwAvailPhys);
			m_dwNumPasses = SDIM( hDlg, IDC_BENCH_PASSES, TBM_GETPOS, 0, 0 );
			DoBenchmark();
		}
		break;
	}
}


/// Handle track-bar (slider) notification messages
static void OnHScroll( HWND hDlg, WORD wNotifyCode, WORD wPos, WORD wID )
{
	char str[64];
	UINT dwPerc, dwSize, dwNumPasses;
	DWORD dwCur, dwCnt, dwCurFree;
	LPMEMSTATITEM lpMem;

	lpMem = LockMSData( &dwCnt, &dwCur );
	dwCurFree = lpMem[dwCur].dwAvailPhys;
	UnlockMSData();

	dwPerc = SDIM( hDlg, IDC_BENCH_PERC, TBM_GETPOS, 0, 0 );
	dwSize = (DWORD)(dwCurFree*(dwPerc/1000.0f)) >> 20;
	dwNumPasses = SDIM( hDlg, IDC_BENCH_PASSES, TBM_GETPOS, 0, 0 );

	switch ( wID )
	{
		case IDC_BENCH_PERC:
		// 'benchmark-size' tracking
			sprintf( str, STR_BENCH_SIZE, dwSize, dwPerc/10.0f );
			SDIM( hDlg, IDC_BENCH_SIZE, WM_SETTEXT, 0, (LPARAM)str );
			
			break;

		case IDC_BENCH_PASSES:
			sprintf( str, "%i", dwNumPasses );
			SDIM( hDlg, IDC_PASSES_STR, WM_SETTEXT, 0, (LPARAM)str );
			break;
	}

	// update total memory access info
	float dvTotalAccess = (float)dwSize * dwNumPasses;
	if ( dvTotalAccess > 1024 )
		sprintf( str, STR_BENCH_PROCESSED"%.2f G", dvTotalAccess/1024.0f );
	else
		sprintf( str, STR_BENCH_PROCESSED"%.0f M", dvTotalAccess );
	SDIM( hDlg, IDC_TOTAL_STR, WM_SETTEXT, 0, (LPARAM)str );
}


/// Initializes memory benchmark dialog
static void OnInitDialog( HWND hDlg )
{
	m_hBenchWnd = hDlg;
	CenterWindow( hDlg, TRUE );
	SDIM( hDlg, IDC_BENCH_PERC, TBM_SETRANGE, TRUE, MAKELONG(1,1000) );
	SDIM( hDlg, IDC_BENCH_PERC, TBM_SETPOS, TRUE, 500 );
	SDIM( hDlg, IDC_BENCH_PASSES, TBM_SETRANGE, TRUE, MAKELONG(1,100) );
	SDIM( hDlg, IDC_BENCH_PASSES, TBM_SETPOS, TRUE, 4 );
	OnHScroll( hDlg, NULL, NULL, IDC_BENCH_PERC );
	OnHScroll( hDlg, NULL, NULL, IDC_BENCH_PASSES );
	SetTimer( hDlg, 1, 1000, NULL );
}


/// Memory benchmark dialog message proc
BOOL WINAPI MemBenchDlgProc(  HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
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

	case WM_DESTROY:
		KillTimer( hDlg, 1 );
		break;

	case WM_TIMER:
		OnHScroll( hDlg, NULL, NULL, IDC_BENCH_PERC );
		break;
	}
	return FALSE;

}