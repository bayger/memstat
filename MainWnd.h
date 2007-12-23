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

#ifndef	__MAINWND_H__
#define	__MAINWND_H__


extern HWND     g_hMainWnd;
extern HWND     g_hwndTooltip;
extern HWND     g_hwcpCurrent;
extern HWND     g_hwcpFreeing;
extern HWND     g_hwcpMonitoring;
extern HWND     g_hwcpAppearance;
extern HWND     g_hwcpAdvanced;
extern HWND     g_hwcpAbout;
extern DWORD	g_dwLastIconValue;

void UpdateMemStats();
BOOL CreateMainWnd();
BOOL WINAPI MemBenchDlgProc(  HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );


#endif
