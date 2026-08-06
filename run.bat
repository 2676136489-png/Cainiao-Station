@echo off
call build.bat
if errorlevel 1 exit /b 1
if exist "build\x64\Release\CainiaoStation.exe" (
    "build\x64\Release\CainiaoStation.exe"
) else (
    build\CainiaoStation.exe
)
