@echo off
cls

echo.
echo [*] Compilando Release x64...
echo.
cmake --build build/x64 --config Release

echo.
echo [*] Compilando Release x86...
echo.
cmake --build build/x86 --config Release

echo.
echo [*] Listo!
echo.