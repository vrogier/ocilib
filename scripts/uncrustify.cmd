@echo off

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR%..\src"
set "HDR_DIR=%SCRIPT_DIR%..\include\ocilibc"
set "CPP_DIR=%SCRIPT_DIR%..\include\ocilibcpp"
set "LIST_FILE_C=%SCRIPT_DIR%uncrustify-c.txt"
set "LIST_FILE_CPP=%SCRIPT_DIR%uncrustify-cpp.txt"

if exist "%LIST_FILE_C%" del "%LIST_FILE_C%"
if exist "%LIST_FILE_CPP%" del "%LIST_FILE_CPP%"

for %%f in ("%SRC_DIR%\*.c" "%SRC_DIR%\*.h") do (
    >>"%LIST_FILE_C%" echo ..\src\%%~nxf
)

for %%f in ("%HDR_DIR%\*.h") do (
    >>"%LIST_FILE_C%" echo ..\include\ocilibc\%%~nxf
)

for /r "%CPP_DIR%" %%f in (*.hpp) do (
    set "ABS=%%f"
    >>"%LIST_FILE_CPP%" echo ..\include\ocilibcpp!ABS:*ocilibcpp=!
)

endlocal

uncrustify.exe  -c uncrustify-c.cfg -F uncrustify-c.txt --replace
uncrustify.exe  -c uncrustify-cpp.cfg -F uncrustify-cpp.txt --replace

del /q uncrustify-c.txt              2>nul
del /q uncrustify-cpp.txt            2>nul
del /q /s "..\src\*.md5~"            2>nul
del /q /s "..\src\*.unc-backup~"     2>nul
del /q /s "..\src\*.unc-backup.md5~" 2>nul
del /q /s "..\include\*.md5~"        2>nul
del /q /s "..\include\*.unc-backup~" 2>nul
del /q /s "..\include\*.unc-backup.md5~" 2>nul

