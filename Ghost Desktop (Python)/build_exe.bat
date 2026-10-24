@echo off
cd /d "%~dp0"
where py >nul 2>nul
if errorlevel 1 (
    echo ERRO: Python oficial nao encontrado. Use o py launcher do python.org.
    pause
    exit /b 1
)
echo Instalando dependencias e o PyInstaller...
py -m pip install -r requirements.txt pyinstaller
if errorlevel 1 goto erro

echo.
echo Gerando o .exe (pode demorar 1 a 3 minutos)...
py -m PyInstaller --noconsole --onefile --name Wabbe --icon app.ico --add-data "app.ico;." --collect-all customtkinter --hidden-import pystray._win32 --noconfirm main.py
if errorlevel 1 goto erro

echo.
echo ================================
echo  PRONTO: %cd%\dist\Wabbe.exe
echo ================================
pause
exit /b 0

:erro
echo.
echo ERRO. Copie o texto acima e mande para o Claude.
pause
exit /b 1
