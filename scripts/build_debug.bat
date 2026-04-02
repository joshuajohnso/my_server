@echo off
title qmake and nmake build prompt
set MINGW_PATH=e:\QT\Tools\mingw810_64
set QT_DIR=e:\QT\5.15.2\mingw81_64
set BUILD_DIR=%cd%\build
set PRO_DIR=%cd%
set PATH=%MINGW_PATH%\bin;%QT_DIR%\bin;%PATH%


if not exist "%BUILD_DIR%" (
    md "%BUILD_DIR%"
)

pushd "%BUILD_DIR%"
qmake.exe "%PRO_DIR%\server.pro" -spec win32-g++ "CONFIG+=debug" "CONFIG+=console"
if exist "%BUILD_DIR%\debug\server.exe" del "%BUILD_DIR%\debug\server.exe"
@REM e:\QT\Tools\QtCreator\bin\jom.exe -j4
%MINGW_PATH%\bin\mingw32-make -f Makefile.Debug
cd /d "%BUILD_DIR%\debug"
if not exist "%BUILD_DIR%\debug\Qt5Core.dll" (
    windeployqt server.exe
)
popd