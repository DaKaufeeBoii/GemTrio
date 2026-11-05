@echo off
title GemTrio - Native PC Port
cd /d "%~dp0emerald_recomp_bin"

if not exist "gba_bios.bin" (
    if exist "..\..\gba_bios\gba_bios.bin" (
        copy "..\..\gba_bios\gba_bios.bin" "gba_bios.bin" >nul
    )
)

if exist "..\build\emerald\pokeemerald.gba" (
    copy /y "..\build\emerald\pokeemerald.gba" "gemtrio.gba" >nul
)

echo ========================================================
echo             GemTrio - Native PC Port
echo ========================================================
echo Starting Emerald Recomp native engine...
start "" "EmeraldRecomp.exe"
