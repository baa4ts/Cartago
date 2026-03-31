@echo off
cls

echo.
echo [*] Configurando x64...
echo.
cmake -B build/x64 -G "Visual Studio 17 2022" -A x64

echo.
echo [*] Configurando x86...
echo.
cmake -B build/x86 -G "Visual Studio 17 2022" -A Win32

echo.
echo [*] Listo!
echo.