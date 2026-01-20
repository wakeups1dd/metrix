; OpenMeters NSIS Installer Script
; Creates a Windows installer for OpenMeters

!define APP_NAME "OpenMeters"
!define APP_VERSION "0.1.0"
!define APP_PUBLISHER "OpenMeters Contributors"
!define APP_URL "https://github.com/yourusername/openmeters"
!define APP_EXECUTABLE "openmeters.exe"
!define APP_INSTALL_DIR "$PROGRAMFILES64\${APP_NAME}"

; Modern UI
!include "MUI2.nsh"

; Installer attributes
Name "${APP_NAME}"
OutFile "OpenMeters-Setup.exe"
InstallDir "${APP_INSTALL_DIR}"
RequestExecutionLevel admin

; Modern UI Configuration
!define MUI_ICON "icon.ico"
!define MUI_UNICON "icon.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.bmp"
!define MUI_WELCOMEPAGE_TITLE "Welcome to ${APP_NAME}"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of ${APP_NAME} ${APP_VERSION}."

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Languages
!insertmacro MUI_LANGUAGE "English"

; Installer sections
Section "Application" SecApp
    SectionIn RO ; Required section
    
    SetOutPath "$INSTDIR"
    
    ; Install executable
    File "${APP_EXECUTABLE}"
    
    ; Install documentation
    File "README.md"
    File "LICENSE"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Create Start Menu shortcuts
    CreateDirectory "$SMPROGRAMS\${APP_NAME}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXECUTABLE}"
    CreateShortcut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    
    ; Registry entries
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
        "DisplayName" "${APP_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
        "DisplayVersion" "${APP_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
        "Publisher" "${APP_PUBLISHER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
        "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
        "URLInfoAbout" "${APP_URL}"
    
    ; Create config directory
    CreateDirectory "$APPDATA\${APP_NAME}"
SectionEnd

Section "Desktop Shortcut" SecDesktop
    CreateShortcut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXECUTABLE}"
SectionEnd

; Uninstaller
Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\${APP_EXECUTABLE}"
    Delete "$INSTDIR\README.md"
    Delete "$INSTDIR\LICENSE"
    Delete "$INSTDIR\Uninstall.exe"
    
    ; Remove shortcuts
    Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
    Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
    RMDir "$SMPROGRAMS\${APP_NAME}"
    Delete "$DESKTOP\${APP_NAME}.lnk"
    
    ; Remove registry entries
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
    
    ; Remove config (optional - ask user)
    ; RMDir /r "$APPDATA\${APP_NAME}"
    
    ; Remove installation directory
    RMDir "$INSTDIR"
SectionEnd

; Section descriptions
LangString DESC_SecApp ${LANG_ENGLISH} "Install ${APP_NAME} application files."
LangString DESC_SecDesktop ${LANG_ENGLISH} "Create a desktop shortcut."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecApp} $(DESC_SecApp)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} $(DESC_SecDesktop)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

