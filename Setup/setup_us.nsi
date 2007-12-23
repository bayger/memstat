; MemStatXP.nsi
;
; It will install MemStat XP into a directory that the user selects
;

; The name of the installer
Name "MemStat XP"

; The file to write
OutFile "memstat-1.6.0.0-setup.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\MemStat XP"
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\MemStat\InstallDir "Install_Dir"
Caption "MemStat XP Installer"
SubCaption 1 " " 
SubCaption 2 " " 
SubCaption 3 " " 
SubCaption 4 " "
EnabledBitmap three-check.bmp
DisabledBitmap three-nocheck.bmp

; The text to prompt the user to enter a directory
ComponentText "This program will install MemStat XP version 1.6.0.0 on your machine." " " "Select options you want to install:"
; The text to prompt the user to enter a directory
DirText "Please specify installation directory."

; The stuff to install
Section "MemStat XP (required)"
  ExecWait '"$INSTDIR\memstat.exe" off'
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "memstat.exe"
  File "license.htm"
  ; Start menu shortcuts
  CreateDirectory "$SMPROGRAMS\MemStat XP"
  CreateShortCut "$SMPROGRAMS\MemStat XP\MemStat XP.lnk" "$INSTDIR\memstat.exe" "" "$INSTDIR\memstat.exe" 0
  CreateShortCut "$SMPROGRAMS\MemStat XP\View GNU license.lnk" "$INSTDIR\license.htm"
  CreateShortCut "$SMPROGRAMS\MemStat XP\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\MemStat\InstallDir "Install_Dir" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MemStat" "DisplayName" "MemStat XP (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MemStat" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd

; optional section
Section "Desktop Shortcut"
  CreateShortCut "$DESKTOP\MemStat XP.lnk" "$INSTDIR\memstat.exe" "" "$INSTDIR\memstat.exe" 0
SectionEnd

; uninstall stuff

UninstallText "This program will uninstall MemStat XP. Click 'Uninstall' to continue."
UninstallCaption "MemStat XP Uninstaller"
UninstallSubCaption 0 " "
UninstallSubCaption 1 " "
UninstallSubCaption 2 " "

; special uninstall section.
Section "Uninstall"
  ExecWait '"$INSTDIR\memstat.exe" off'
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MemStat"
  DeleteRegKey HKCU SOFTWARE\MemStat
  ; remove files
  Delete $INSTDIR\*.*
  ; remove shortcuts, if any.
  Delete "$DESKTOP\MemStat XP.lnk"
  Delete "$SMPROGRAMS\MemStat XP\*.*"
  RMDir "$SMPROGRAMS\MemStat XP"
  ; remove directories used.
  RMDir "$INSTDIR"
SectionEnd

; eof
