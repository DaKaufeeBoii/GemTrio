@echo off
title GemTrio - Native PC Port
cd /d "%~dp0"

if exist "GemTrio.exe" (
    start "" "GemTrio.exe"
) else (
    echo [ERROR] GemTrio.exe launcher not found.
    pause
)
