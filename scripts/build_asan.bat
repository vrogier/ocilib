rem Build the MBCS x64 test suite with MSVC AddressSanitizer (/fsanitize=address).
rem Output goes to a dedicated intermediate/output dir so it never mixes with
rem the regular "Release - MBCS" objects used by coverage runs.
rem Note: MSVC ASAN does not perform leak detection (no LSan on Windows);
rem run the Linux ASan+UBSan+LSan build for leak gating.
call "c:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64
msbuild "%~dp0..\tests\ocilib_tests.vcxproj" /p:Configuration="Release - MBCS" /p:Platform=x64 /p:EnableASAN=true /p:IntDir="ocilib_tests\x64\Release - MBCS - ASAN\\" /p:OutDir="..\x64\Release - MBCS - ASAN\\" /m /v:minimal /nologo
