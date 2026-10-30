@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\build-windows.ps1" -RunWindowTest
if errorlevel 1 (
 echo.
 echo The Windows build did not finish. See the message above and WINDOWS-START-HERE.txt.
 pause
 exit /b 1
)
echo.
echo Your Windows plugin package is in the dist folder.
pause
