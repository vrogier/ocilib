@echo off

setlocal

set "SCRIPT_DIR=%~dp0"
set "SRC_DIR=%SCRIPT_DIR%..\src"
set "HDR_DIR=%SCRIPT_DIR%..\include\ocilibc"
set "LIST_FILE=%SCRIPT_DIR%uncrustify.txt"

if exist "%LIST_FILE%" del "%LIST_FILE%"

for %%f in ("%SRC_DIR%\*.c" "%SRC_DIR%\*.h") do (
    >>"%LIST_FILE%" echo ..\src\%%~nxf
)

for %%f in ("%HDR_DIR%\*.h") do (
    >>"%LIST_FILE%" echo ..\include\ocilibc\%%~nxf
)

endlocal

uncrustify.exe  -c uncrustify.cfg -F uncrustify.txt --replace

del /q uncrustify.txt             2>nul
del /q "..\src\*.unc-backup.md5~" 2>nul
del /q "..\src\*.md5~"            2>nul
del /q "..\src\*.md5~"            2>nul
