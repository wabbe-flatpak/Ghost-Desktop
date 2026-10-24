@echo off
cd /d "%~dp0"
echo Rodando o Wabbe com console para mostrar erros...
echo.
py --version
py main.py
echo.
echo ---- O programa terminou. Se apareceu um erro acima, copie e mande para o Claude ----
pause
