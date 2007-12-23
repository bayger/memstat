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

#ifndef	__CHOOSERGB_H__
#define	__CHOOSERGB_H__


// dialog creation struct
typedef	struct _CHOOSERGB
{
	DWORD         dwFlags;
	LPSTR         lpszCaption;
	LPCOLORREF    lpColor;
	COLORREF      clrSet;
	COLORREF      clrOld;
	HWND          hwndParent;
}	CHOOSERGB, *LPCHOOSERGB;


// CHOOSERGB.dwFlags defines
#define	CRGB_INITCOLOR    1
#define	CRGB_PREVENTHEX   2
#define	CRGB_REALTIME     4


BOOL ChooseRGB( LPCHOOSERGB lpChooseRGB );


#endif