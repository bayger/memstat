; MemStatXP.nsi
;
; It will install MemStat XP into a directory that the user selects
;

; The name of the installer
Name "MemStat XP"

; The file to write
OutFile "memstat-1.6.0.0-pl-setup.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\MemStat XP"
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\MemStat\InstallDir "Install_Dir"
MiscButtonText "Wróæ" "Dalej" "Anuluj" "Zamknij"
InstallButtonText "Instaluj"
Caption "Instalator programu MemStat XP"
SubCaption 1 " " 
SubCaption 2 " " 
SubCaption 3 " " 
SubCaption 4 " "
SpaceTexts "Wymagane: " "Dostêpne: "
EnabledBitmap three-check.bmp
DisabledBitmap three-nocheck.bmp

; The text to prompt the user to enter a directory
ComponentText "Ten program zainstaluje MemStat XP w wersji 1.6.0.0 (po polsku) na Twoim komputerze." " " "Wybierz opcje, które chcesz zainstalowaæ:"
; The text to prompt the user to enter a directory
DirText "Teraz wska¿ instalatorowi katalog, w którym chcesz zainstalowaæ program." "Wybierz katalog docelowy:" "Przegl¹daj..."
DetailsButtonText "Szczegó³y"
CompletedText "Zrobione"

; The stuff to install
Section "MemStat XP (wymagane)"
  ExecWait '"$INSTDIR\memstat.exe" off'
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "memstat.exe"
  File "licencja.htm"
  ; Start menu shortcuts
  CreateDirectory "$SMPROGRAMS\MemStat XP"
  CreateShortCut "$SMPROGRAMS\MemStat XP\MemStat XP.lnk" "$INSTDIR\memstat.exe" "" "$INSTDIR\memstat.exe" 0
  CreateShortCut "$SMPROGRAMS\MemStat XP\Zobacz licencjê GNU.lnk" "$INSTDIR\licencja.htm"
  CreateShortCut "$SMPROGRAMS\MemStat XP\Odinstaluj.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\MemStat\InstallDir "Install_Dir" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MemStat" "DisplayName" "MemStat XP (tylko usuwanie)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MemStat" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteUninstaller "uninstall.exe"
SectionEnd

; optional section
Section "Skrót na Pulpicie"
  CreateShortCut "$DESKTOP\MemStat XP.lnk" "$INSTDIR\memstat.exe" "" "$INSTDIR\memstat.exe" 0
SectionEnd

; uninstall stuff

UninstallText "Ten program odinstaluje MemStat XP. Upewnij siê, ¿e MemStat XP nie jest uruchominy i kliknij 'Odinstaluj' aby kontynuowaæ." "Odinstaluj z:"
UninstallCaption "Deinstalator programu MemStat XP"
UninstallSubCaption 0 " "
UninstallSubCaption 1 " "
UninstallSubCaption 2 " "
UninstallButtonText "Odinstaluj"

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
