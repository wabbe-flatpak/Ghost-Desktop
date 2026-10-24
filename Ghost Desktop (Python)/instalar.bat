@echo off
cd /d "%~dp0"
where py >nul 2>nul
if errorlevel 1 (
    echo ERRO: Python oficial nao encontrado.
    echo Instale em https://www.python.org/downloads/ e marque "Add python.exe to PATH".
    pause
    exit /b 1
)
echo Usando este Python:
py --version
echo.
echo Instalando dependencias...
py -m pip install -r requirements.txt
if errorlevel 1 (
    echo.
    echo ERRO ao instalar. Copie o texto acima e mande para o Claude.
    pause
    exit /b 1
)
echo.
echo Pronto. Agora use o iniciar.bat
pause
