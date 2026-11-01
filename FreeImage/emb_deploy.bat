@echo off
call fv_vars.bat
xcopy /Y /Q ".\Dist\x64\FreeImage.h" "%BDSCOMMONDIR%\include"
xcopy /Y /Q ".\Wrapper\FreeImagePlus\FreeImagePlus.h" "%BDSCOMMONDIR%\include"

rem xcopy /Y /Q ".\Dist\x32\FreeImage.dll" "%BDSCOMMONDIR%\dll\Win32"
rem implib -a "%BDSCOMMONDIR%\lib\Win32\FreeImage" ".\Dist\x32\FreeImage.dll"

xcopy /Y /Q ".\Dist\x64\FreeImage.dll" "%BDSCOMMONDIR%\dll\Win64x"
rem mkexp "%BDSCOMMONDIR%\lib\Win64\FreeImage.a" ".\Dist\x64\FreeImage.dll"
xcopy /Y /Q ".\Dist\x64\FreeImage.lib" "%BDSLIB%\Win64x"
