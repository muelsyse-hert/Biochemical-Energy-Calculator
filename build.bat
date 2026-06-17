@echo off
setlocal

set "CC=gcc"
set "CFLAGS=-std=c11 -Wall -Wextra -Wpedantic"
set "SRC=main.c calc_small.c display.c"
set "OUT=bec.exe"

echo Building %OUT%...
%CC% %CFLAGS% %SRC% -o %OUT%
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build succeeded: %OUT%
endlocal
