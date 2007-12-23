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

// English language strings

#ifndef LANG_INCLUDED
#define LANG_INCLUDED

#define STR_CANT_CREATE_DIALOG_PAGE "Can't create dialog page!"
#define STR_TAB_ABOUT "About"
#define STR_TAB_ADVANCED "Advanced"
#define STR_TAB_APPEARANCE "Appearance"
#define STR_TAB_FREEING "Freeing"
#define STR_TAB_MONITORING "Monitoring"
#define STR_FASTO "to %.2f MB"
#define STR_RESETCFG "Are you sure you want to restore\nall settings from registry?"
#define STR_WARNING "Warning"
#define STR_CFGSAVED "Configuration settings has been saved."
#define STR_MESSAGE "Message"
#define STR_SET_BG_COLOR "Set color of chart background"
#define STR_SET_CHART_COLOR "Set color of the chart line"
#define STR_SET_LINE_COLOR "Set color of the horizontal lines"
#define STR_SET_TEXT_COLOR "Set color of the chart text"
#define STR_SET_BG_CAPTION "Set background color of the caption"
#define STR_SET_TEXT_CAPTION "Set text color of the caption"
#define STR_MFU_BUTTON "Free up %.2f MB\n(target free: %.2f MB)"
#define STR_PRI_IDLE "Idle (recommended)"
#define STR_PRI_NORMAL "Normal"
#define STR_PRI_HIGH "High"
#define STR_MSL_STATUS "Logging is %s now."
#define STR_ENABLED "ENABLED"
#define STR_DISABLED "DISABLED"
#define STR_LOGGING "logging"
#define STR_ENABLE "Enable"
#define STR_DISABLE "Disable"
#define STR_CANT_ENABLE_MSL "Can't start logging!\n\nProbably there is wrong log file name\nor log file exceeded size limit!"
#define STR_ERROR "Error"
#define STR_CANT_CREATE_CHART "Can't create chart window"
#define STR_CANT_CREATE_MAIN "Can't create main window"
#define STR_NO_MEMORY "It seems that there is no enough virtual memory\nfor MemStat XP to run, so it will terminate now.\n"
#define STR_CANT_CREATE_FREEING_THREAD "Can't create memory freeing thread!"
#define STR_BUSY "Busy"
#define STR_CURRENTLY_CANT_FREE "Currently there is %.2f MB of free physical memory and\nyou are trying to free it to %.2f MB. You can't do that, because\nyou have only %.2f MB of physical memory installed!!!"
#define STR_TRAYMENU_FREEUP "Free up %.2f MB (target: %.2f MB)"
#define STR_TRAYMENU_TOOLTIP "MemStat XP * Free physical: %i MB * Free swap: %i MB"
#define STR_CHART_CAPTION "Free RAM: %.2f MB (%.0f%%), SWAP usage: %.2f MB"
#define STR_FREEING_FREEUP "Free up %.2f MB\n(target: %.2f MB)"
#define STR_FREEING "Freeing (%.0f%% done)"
#define STR_BENCH_DOUBLEDREADING "Doubled reading of 32-bit data stream..."
#define STR_BENCH_DOUBLEDWRITING "Doubled writing of 32-bit data stream..."
#define STR_BENCH_DOUBLEDCOPYING "Doubled copying of 32-bit data stream..."
#define STR_BENCH_LINEARREADING "Linear reading of 32-bit data stream..."
#define STR_BENCH_LINEARWRITING "Linear writing of 32-bit data stream..."
#define STR_BENCH_LINEARCOPYING "Linear copying of 32-bit data stream..."
#define STR_BENCH_LOWMEMORY "You must use at least 4 MB of memory for this benchmark!"
#define STR_BENCH_WARNING "MemStat XP is about to run benchmark process.\nThe system will be not responding to your input\n" \
		"until process end.\n\nAre you sure you want to run benchmark now?\n\nNOTE: If you are now burning CD benchmark will probably\n" \
		"interrupt this process, which may cause a buffer underrun error!"
#define STR_BENCH_PREPARING "Preparing memory..."
#define STR_BENCH_NO_MEMORY "Can't execute benchmark - no enough memory!"
#define STR_DONE "Done"
#define STR_BENCH_CANT "Sorry, MemStat XP is currently freeing memory\nand can't run benchmark now. Try again later."
#define STR_BENCH_SIZE "%i MB (%.0f %% of free memory)"
#define STR_BENCH_PROCESSED "Bytes processed in each test: "
#define STR_CANT_CREATE_MONITORING_THREAD "Can't initialize memory monitoring thread!"
#define STR_MEMSTAT_CAPTION "MemStat XP Configuration"

// default font used in chart window caption
#define STR_FONT "Arial"
#define FONT_CHARSET DEFAULT_CHARSET

// default format strings:
#define STR_DEFAULT_LOG_FORMAT "$t [$b] $p"
#define STR_DEFAULT_CAPTION_FORMAT "Free PM: $Mp MB, Used swap: $MS MB"
#define STR_DEFAULT_TOOLTIP_FORMAT "MemStat XP * Free PM: $Mp MB, Free swap: $Ms MB"

// format items:
#define STR_FORMAT_ITEMS { "$p\tFree physical memory", "$s\tFree swap file", "$P\tUsed physical memory", \
	"$S\tUsed swap file", "$u\tMemory utilization (in percents)", \
	"$b\tBoot up time (in ms)", "$B\tBoot up time (hh:mm:ss)", \
	"$t \tCurrent time", "$d\tCurrent date", "$v\tMemStat XP version", \
	"$n\tNew line"
#define STR_FORMAT_ITEMS_CNT 11

// format dialog captions:
#define STR_EDIT_LOG_FORMAT "Edit log format"
#define STR_EDIT_CAPTION_FORMAT "Edit chart window caption format"
#define STR_EDIT_TOOLTIP_FORMAT "Edit tray icon tooltip format"

// tray icon styles:
#define STR_ICON_STYLES { "Chip", "Four bars", "Minichart", "Numerical", "Digital"
#define STR_ICON_STYLES_CNT 5


#endif