# Microsoft Developer Studio Project File - Name="MemStat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MemStat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MemStat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MemStat.mak" CFG="MemStat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MemStat - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MemStat - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MemStat - Win32 Release PL" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MemStat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MEMSTAT_LANG_US" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG" /d "MEMSTAT_LANG_US"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 msimg32.lib commode.obj kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
OutDir=.\Release
TargetName=MemStat
SOURCE="$(InputPath)"
PreLink_Cmds=kill.bat $(OutDir)\$(TargetName).exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MemStat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "MEMSTAT_LANG_US" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG" /d "MEMSTAT_LANG_US"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 commode.obj kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /profile /debug /machine:I386
# Begin Special Build Tool
OutDir=.\Debug
TargetName=MemStat
SOURCE="$(InputPath)"
PreLink_Cmds=kill.bat $(OutDir)\$(TargetName).exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MemStat - Win32 Release PL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MemStat___Win32_Release_PL"
# PROP BASE Intermediate_Dir "MemStat___Win32_Release_PL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MemStat___Win32_Release_PL"
# PROP Intermediate_Dir "MemStat___Win32_Release_PL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "LANG_PL" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "MEMSTAT_LANG_PL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "LANG_PL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "MEMSTAT_LANG_PL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msimg32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 msimg32.lib commode.obj kernel32.lib user32.lib gdi32.lib winspool.lib comctl32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:windows /machine:I386
# Begin Special Build Tool
OutDir=.\MemStat___Win32_Release_PL
TargetName=MemStat
SOURCE="$(InputPath)"
PreLink_Cmds=kill.bat $(OutDir)\$(TargetName).exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "MemStat - Win32 Release"
# Name "MemStat - Win32 Debug"
# Name "MemStat - Win32 Release PL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CfgAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\CfgAdvanced.cpp
# End Source File
# Begin Source File

SOURCE=.\CfgAppearance.cpp
# End Source File
# Begin Source File

SOURCE=.\CfgFreeing.cpp
# End Source File
# Begin Source File

SOURCE=.\CfgMonitoring.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartParentWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ChartWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseRGB.cpp
# End Source File
# Begin Source File

SOURCE=.\FormatEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\FreeThread.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWnd.cpp

!IF  "$(CFG)" == "MemStat - Win32 Release"

!ELSEIF  "$(CFG)" == "MemStat - Win32 Debug"

# ADD CPP /YX"StdAfx.h"

!ELSEIF  "$(CFG)" == "MemStat - Win32 Release PL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MemBench.cpp
# End Source File
# Begin Source File

SOURCE=.\MemStat.cpp

!IF  "$(CFG)" == "MemStat - Win32 Release"

!ELSEIF  "$(CFG)" == "MemStat - Win32 Debug"

# ADD CPP /YX"StdAfx.h"

!ELSEIF  "$(CFG)" == "MemStat - Win32 Release PL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MonitorThread.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "MemStat - Win32 Release"

!ELSEIF  "$(CFG)" == "MemStat - Win32 Debug"

# ADD CPP /YX"StdAfx.h"

!ELSEIF  "$(CFG)" == "MemStat - Win32 Release PL"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CfgAbout.h
# End Source File
# Begin Source File

SOURCE=.\CfgAdvanced.h
# End Source File
# Begin Source File

SOURCE=.\CfgAppearance.h
# End Source File
# Begin Source File

SOURCE=.\CfgFreeing.h
# End Source File
# Begin Source File

SOURCE=.\CfgMonitoring.h
# End Source File
# Begin Source File

SOURCE=.\ChartParentWnd.h
# End Source File
# Begin Source File

SOURCE=.\ChartWnd.h
# End Source File
# Begin Source File

SOURCE=.\ChooseRGB.h
# End Source File
# Begin Source File

SOURCE=.\FormatEdit.h
# End Source File
# Begin Source File

SOURCE=.\FreeThread.h
# End Source File
# Begin Source File

SOURCE=.\LangPL.h
# End Source File
# Begin Source File

SOURCE=.\LangUS.h
# End Source File
# Begin Source File

SOURCE=.\MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\MemStat.h
# End Source File
# Begin Source File

SOURCE=.\MonitorThread.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\Res\iconfont.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\memstat.ico
# End Source File
# Begin Source File

SOURCE=.\Res\MemStat.rc
# End Source File
# Begin Source File

SOURCE=.\Res\resource.h
# End Source File
# Begin Source File

SOURCE=.\Res\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\Res\trayinfo.ico
# End Source File
# Begin Source File

SOURCE=.\Res\traylow.ico
# End Source File
# Begin Source File

SOURCE=.\Res\trayok.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\Res\MemStat.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
