call "c:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=amd64
msbuild tests\ocilib_tests.vcxproj /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
