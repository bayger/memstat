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

// Polish language strings

#ifndef LANG_INCLUDED
#define LANG_INCLUDED


#define STR_CANT_CREATE_DIALOG_PAGE "Nie mo�na utworzy� zak�adki!"
#define STR_TAB_ABOUT "O programie"
#define STR_TAB_ADVANCED "Zaawansowane"
#define STR_TAB_APPEARANCE "Wygl�d"
#define STR_TAB_FREEING "Zwalnianie"
#define STR_TAB_MONITORING "Monitorowanie"
#define STR_FASTO "do %.2f MB"
#define STR_RESETCFG "Jeste� pewien, �e chcesz przywr�ci�\nz rejestru wszystkie ustawienia?"
#define STR_WARNING "Ostrze�enie"
#define STR_CFGSAVED "Ustawienia zosta�y zapisane."
#define STR_MESSAGE "Komunikat"
#define STR_SET_BG_COLOR "Ustaw kolor t�a wykresu"
#define STR_SET_CHART_COLOR "Ustaw kolor linii wykresu"
#define STR_SET_LINE_COLOR "Ustaw kolor linii poziomych"
#define STR_SET_TEXT_COLOR "Ustaw kolor tekstu wykresu"
#define STR_SET_BG_CAPTION "Ustaw kolor t�a nag��wka"
#define STR_SET_TEXT_CAPTION "Ustaw kolor tekstu nag��wka"
#define STR_MFU_BUTTON "Zwolnij %.2f MB\n(docelowo: %.2f MB)"
#define STR_PRI_IDLE "Ja�owy (zalecany)"
#define STR_PRI_NORMAL "Normalny"
#define STR_PRI_HIGH "Wysoki"
#define STR_MSL_STATUS "Logowanie jest teraz %s."
#define STR_ENABLED "W��CZONE"
#define STR_DISABLED "WY��CZONE"
#define STR_LOGGING "logowanie"
#define STR_ENABLE "W��cz"
#define STR_DISABLE "Wy��cz"
#define STR_CANT_ENABLE_MSL "Nie mo�na w��czy� logowania!\n\nPrawdopodobnie zosta�a podana z�a nazwa pliku\nlub plik logowania si� przepe�ni�!"
#define STR_ERROR "B��d"
#define STR_CANT_CREATE_CHART "Nie mo�na utworzy� okna wykresu!"
#define STR_CANT_CREATE_MAIN "Nie mo�na utworzy� g��wnego okna!"
#define STR_NO_MEMORY "Brak wolnej pami�ci wirtualnej do dzia�ania programu. MemStat XP zostanie zamkni�ty.\n"
#define STR_CANT_CREATE_FREEING_THREAD "Nie mo�na utworzy� w�tka zwalniaj�cego!"
#define STR_BUSY "Zaj�ty"
#define STR_CURRENTLY_CANT_FREE "Aktualnie jest %.2f MB wolnej pami�ci fizycznej a\npr�bujesz zwolni� j� do poziomu %.2f MB. Nie mo�esz tego zrobi�, poniewa�\nmasz tylko %.2f MB pami�ci fizycznej zainstalowanej w systemie!!!"
#define STR_TRAYMENU_FREEUP "Zwolnij %.2f MB (docelowo: %.2f MB)"
#define STR_TRAYMENU_TOOLTIP "MemStat XP * Wolna fizyczna: %i MB * Wolny plik wymiany: %i MB"
#define STR_CHART_CAPTION "Wolny RAM: %.2f MB (%.0f%%), U�yty SWAP: %.2f MB"
#define STR_FREEING_FREEUP "Zwolnij %.2f MB\n(docelowo: %.2f MB)"
#define STR_FREEING "Zwalnianie (%.0f%% zrobione)"
#define STR_BENCH_DOUBLEDREADING "Odczyt, dost�p podw�jny, strumie� danych 32-bit ..."
#define STR_BENCH_DOUBLEDWRITING "Zapis, dost�p podw�jny, strumie� danych 32-bit ..."
#define STR_BENCH_DOUBLEDCOPYING "Kopiowanie, dost�p podw�jny, strumie� danych 32-bit ..."
#define STR_BENCH_LINEARREADING "Odczyt, dost�p liniowy, strumie� danych 32-bit ..."
#define STR_BENCH_LINEARWRITING "Zapis, dost�p liniowy, strumie� danych 32-bit ..."
#define STR_BENCH_LINEARCOPYING "Kopiowanie, dost�p liniowy, strumie� danych 32-bit ..."
#define STR_BENCH_LOWMEMORY "Musisz u�y� do testu minimum 4 MB pami�ci fizycznej!"
#define STR_BENCH_WARNING "MemStat XP zaraz uruchomi test szybko�ci pami�ci.\nSystem nie b�dzie odpowiada� na dzia�ania u�ytkownika\n" \
		"a� test zostanie uko�czony.\n\nCzy jeste� pewien, �e chcesz go uruchomi� teraz?\n\nUWAGA: Je�li aktualnie wypalasz p�yt� CD ten test mo�e przerwa� ten proces,\n" \
		"co w konsekwencji mo�e prowadzi� do b��d�w podczas nagrywania!"
#define STR_BENCH_PREPARING "Przygotowuj� pami�� do testu..."
#define STR_BENCH_NO_MEMORY "Nie mo�na uruchomi� testu - za ma�o pami�ci!"
#define STR_DONE "Zrobione"
#define STR_BENCH_CANT "Niestety, MemStat XP aktualnie zwalnia pami��\ni nie mo�e uruchomi� teraz testu. Spr�buj p�niej."
#define STR_BENCH_SIZE "%i MB (%.0f %% wolnej pami�ci)"
#define STR_BENCH_PROCESSED "Bajt�w u�ytych do ka�dego z test�w: "
#define STR_CANT_CREATE_MONITORING_THREAD "Nie mo�na utworzy� w�tka monitoruj�cego!"
#define STR_MEMSTAT_CAPTION "Konfiguracja MemStat XP"

// default font used in chart window caption
#define STR_FONT "Arial"
#define FONT_CHARSET EASTEUROPE_CHARSET

// default format strings:
#define STR_DEFAULT_LOG_FORMAT "$t [$b] $p"
#define STR_DEFAULT_CAPTION_FORMAT "Wolna fizyczna: $Mp MB, U�yty swap: $MS MB"
#define STR_DEFAULT_TOOLTIP_FORMAT "MemStat XP * Wolna fizyczna: $Mp MB, Wolny swap: $Ms MB"

// format items:
#define STR_FORMAT_ITEMS { "$p\tWolna pami�� fizyczna", "$s\tWolny plik wymiany", \
	"$P\tU�yta pami�� fizyczna", "$S\tU�yty plik wymiany", "$u\tUtylizacja pami�ci (w procentach)", \
	"$b\tCzas pracy systemu (w ms)", "$B\tCzas pracy systemu (hh:mm:ss)", \
	"$t \tBie��cy czas (hh:mm:ss)", "$d\tBie��ca data (dd/mm/yyyy)", "$v\tWersja programu MemStat XP", \
	"$n\tNowa linia"
#define STR_FORMAT_ITEMS_CNT 11

// format dialog captions:
#define STR_EDIT_LOG_FORMAT "Edycja formatu logowania"
#define STR_EDIT_CAPTION_FORMAT "Edycja formatu nag��wka okna wykresu"
#define STR_EDIT_TOOLTIP_FORMAT "Edycja formatu dymka ikonki"

// tray icon styles:
#define STR_ICON_STYLES { "Chip", "Cztery paski", "Miniwykres", "Numeryczny", "Cyfrowy"
#define STR_ICON_STYLES_CNT 5


#endif