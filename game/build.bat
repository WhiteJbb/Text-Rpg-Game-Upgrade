@echo off
REM ============================================================
REM  Planet Earth (Text RPG) - build & run
REM  Double-click this file, or run it from the game/ folder.
REM  Requires gcc in PATH (e.g. MSYS2 UCRT64: C:\msys64\ucrt64\bin)
REM ============================================================
cd /d "%~dp0"

echo Building game.exe ...
gcc -std=c11 -Wall -Wextra main.c util.c save.c player.c battle.c item.c -o game.exe

if %errorlevel%==0 (
    echo.
    echo [OK] Build succeeded. Launching ...
    echo.
    game.exe
) else (
    echo.
    echo [FAIL] Build failed. See the errors above.
    echo  - Make sure the game window is closed before rebuilding.
    echo  - Make sure gcc is installed and on PATH.
    pause
)
