rem Run the ASAN-instrumented ANSI x64 test suite (see build_asan.bat).
rem VsDevCmd puts clang_rt.asan_dynamic-x86_64.dll on PATH.
call "c:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64
set OCILIB_LOG=DEBUG
set OCILIB_LOG_CATEGORIES=*
set OCILIB_LOG_OUTPUT=asan_log.txt
"%~dp0..\x64\Release - ANSI - ASAN\ocilib_tests.exe" %*
