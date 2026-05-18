@echo off
REM =============================
REM MyShop 2025 - Build Script
REM =============================

echo MyShop 2025 - Xay dung ung dung
echo ================================

REM Check for Visual Studio
if not defined VisualStudioVersion (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    ) else (
        echo Khong tim thay Visual Studio 2022!
        echo Vui long mo "Developer Command Prompt for VS 2022" va chay lai.
        pause
        exit /b 1
    )
)

echo.
echo Dang xay dung project...
msbuild MyShop2025.sln /p:Configuration=Debug /p:Platform=x64 /t:Restore,Build

if %errorlevel% equ 0 (
    echo.
echo Xay dung thanh cong!
    echo File output: x64\Debug\MyShop2025.exe
) else (
    echo.
    echo Xay dung that bai! Vui long kiem tra loi.
)

pause
