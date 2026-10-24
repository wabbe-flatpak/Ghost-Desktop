@echo off
title Build Wabbe Control Center
cd /d "%~dp0"

echo ================================
echo  BUILD WABBE CONTROL CENTER
 echo ================================
echo.

where g++ >nul 2>nul
if errorlevel 1 (
    echo ERRO: g++ nao encontrado.
    echo Instale o MSYS2 UCRT64 e adicione C:\msys64\ucrt64\bin no PATH.
    pause
    exit /b 1
)

where windres >nul 2>nul
if errorlevel 1 (
    echo ERRO: windres nao encontrado.
    echo Instale mingw-w64-ucrt-x86_64-binutils no MSYS2.
    pause
    exit /b 1
)

echo Compilando recurso do icone...
windres app.rc -O coff -o app.res
if errorlevel 1 goto erro

echo Compilando executavel...
g++ WabbeControlCenter.cpp app.res -o WabbeControlCenter.exe -mwindows -municode -static -std=c++17 -luser32 -lshell32 -lgdi32 -ldwmapi -lshlwapi -lole32
if errorlevel 1 goto erro

echo.
echo ================================
echo  BUILD CONCLUIDA
 echo ================================
echo.
echo Executavel gerado:
echo %cd%\WabbeControlCenter.exe
echo.
pause
exit /b 0

:erro
echo.
echo ERRO ao compilar.
pause
exit /b 1
