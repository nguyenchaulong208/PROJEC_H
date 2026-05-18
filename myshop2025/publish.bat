@echo off
REM MyShop 2025 - Publish Script (Release)
echo Dang dong goi ung dung...

if not defined VisualStudioVersion (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    )
)

msbuild MyShop2025.sln /p:Configuration=Release /p:Platform=x64 /t:Restore,Build
msbuild MyShop2025.sln /p:Configuration=Release /p:Platform=x64 /t:Build /p:AppxPackage=true

echo.
echo Done. Kiem tra thu muc AppPackages.
pause
