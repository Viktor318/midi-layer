@echo off
chcp 65001 >nul
setlocal EnableExtensions EnableDelayedExpansion

rem ============================================================================
rem  MidiLayer - Windowsos release-csomag összeállítása
rem
rem  Előfeltétel: a Qt Creatorban a Release konfiguráció le van fordítva.
rem  Eredmény:    deploy\MidiLayer-<verzió>-win64\  mappa és mellette egy .zip
rem
rem  Ha a Qt máshova van telepítve, vagy más verzió, csak az alábbi
rem  változókat kell átírni.
rem ============================================================================

set "QT_ROOT=C:\Qt"
set "QT_VERSION=6.12.0"
set "QT_KIT=mingw_64"
set "BUILD_DIR_NAME=Desktop_Qt_6_12_0_MinGW_64_bit_Release"

rem A szkript a saját mappájából (a projekt gyökeréből) dolgozik
cd /d "%~dp0"

rem --- Verziószám kiolvasása a midiLayer.pro "VERSION=" sorából ---------------
set "APP_VERSION="
for /f "tokens=2 delims==" %%V in ('findstr /b /c:"VERSION=" midiLayer.pro') do set "APP_VERSION=%%V"
if not defined APP_VERSION (
    echo [HIBA] Nem találom a VERSION= sort a midiLayer.pro fájlban.
    goto :hiba
)
set "APP_VERSION=%APP_VERSION: =%"

set "EXE=build\%BUILD_DIR_NAME%\MidiLayer\MidiLayer.exe"
set "PKG_NAME=MidiLayer-%APP_VERSION%-win64"
set "OUT_DIR=deploy\%PKG_NAME%"
set "ZIP_FILE=deploy\%PKG_NAME%.zip"

echo.
echo MidiLayer %APP_VERSION% - Windowsos csomag készítése
echo ----------------------------------------------------

rem --- Ellenőrzések ------------------------------------------------------------
if not exist "%EXE%" (
    echo [HIBA] Nem találom a lefordított programot:
    echo        %EXE%
    echo        Fordítsd le a Qt Creatorban a Release konfigurációt.
    goto :hiba
)

set "QT_BIN=%QT_ROOT%\%QT_VERSION%\%QT_KIT%\bin"
if not exist "%QT_BIN%\windeployqt.exe" (
    echo [HIBA] Nem találom a windeployqt.exe-t itt:
    echo        %QT_BIN%
    echo        Állítsd be a QT_ROOT / QT_VERSION / QT_KIT változókat a szkript elején.
    goto :hiba
)

rem A MinGW fordító mappája a Qt Tools alatt (pl. mingw1510_64) - a legutolsót választjuk
set "MINGW_BIN="
for /d %%D in ("%QT_ROOT%\Tools\mingw*_64") do set "MINGW_BIN=%%D\bin"
if not defined MINGW_BIN (
    echo [HIBA] Nem találom a MinGW fordítót a %QT_ROOT%\Tools mappában.
    goto :hiba
)

rem A windeployqt a PATH-ból keresi a MinGW futtatókönyvtárait
set "PATH=%QT_BIN%;%MINGW_BIN%;%PATH%"

rem --- Előző csomag törlése (csak a saját kimeneti mappánkat) -----------------
if exist "%OUT_DIR%" rmdir /s /q "%OUT_DIR%"
if exist "%ZIP_FILE%" del /q "%ZIP_FILE%"
mkdir "%OUT_DIR%" || goto :hiba

rem --- Fájlok másolása -----------------------------------------------------------
echo [1/4] Program másolása...
copy /y "%EXE%" "%OUT_DIR%\" >nul || goto :hiba

echo [2/4] Adatok, betűtípusok és alapbeállítások másolása...
rem A data mappa az exe mellett = hordozható mód (a beállítások a program mappájába kerülnek)
xcopy /e /i /q /y "dist\data" "%OUT_DIR%\data" >nul || goto :hiba
xcopy /e /i /q /y "dist\fonts" "%OUT_DIR%\fonts" >nul || goto :hiba
copy /y "dist\midiLayer.ini" "%OUT_DIR%\" >nul || goto :hiba
copy /y "LICENSE" "%OUT_DIR%\LICENSE.txt" >nul || goto :hiba
copy /y "README.md" "%OUT_DIR%\" >nul
copy /y "README.en.md" "%OUT_DIR%\" >nul

echo [3/4] Qt-s DLL-ek és bővítmények másolása (windeployqt)...
windeployqt --release --compiler-runtime "%OUT_DIR%\MidiLayer.exe" >"deploy\windeployqt.log" 2>&1
if errorlevel 1 (
    echo [HIBA] A windeployqt hibát jelzett, részletek: deploy\windeployqt.log
    goto :hiba
)

echo [4/4] ZIP készítése...
powershell -NoProfile -Command "Compress-Archive -Path '%OUT_DIR%' -DestinationPath '%ZIP_FILE%' -Force"
if errorlevel 1 (
    echo [HIBA] A ZIP elkészítése nem sikerült.
    goto :hiba
)

echo.
echo Kész!
echo   Mappa: %OUT_DIR%
echo   ZIP:   %ZIP_FILE%
echo.
echo Próbáld ki: indítsd el a %OUT_DIR%\MidiLayer.exe programot a Fájlkezelőből.
echo.
pause
exit /b 0

:hiba
echo.
echo A csomag NEM készült el.
echo.
pause
exit /b 1
