@echo off
setlocal

if not exist build mkdir build

if exist tools\zig\zig.exe (
    set "ZIG_CACHE_DIR=%TEMP%\zig-cache"
    set "ZIG_GLOBAL_CACHE_DIR=%TEMP%\zig-global"
    tools\zig\zig.exe cc -std=c11 -O2 -Wall -Wextra main.c -o build\CainiaoStation.exe
    exit /b %errorlevel%
)

where msbuild >nul 2>nul
if not errorlevel 1 (
    msbuild CainiaoStation.sln /t:Build /p:Configuration=Release /p:Platform=x64 /m
    exit /b %errorlevel%
)

where cl >nul 2>nul
if not errorlevel 1 (
    cl /nologo /utf-8 /W4 /std:c11 main.c /Fe:build\CainiaoStation.exe
    exit /b %errorlevel%
)

where gcc >nul 2>nul
if not errorlevel 1 (
    gcc -std=c11 -Wall -Wextra -O2 main.c -o build\CainiaoStation.exe
    exit /b %errorlevel%
)

echo [ERROR] 未找到 cl 或 gcc，请先安装 Visual Studio C/C++ 工具链或 MinGW。
exit /b 1
