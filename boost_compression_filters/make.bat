@echo off
setlocal EnableExtensions
rem build.bat performs a clean build; make.bat performs an incremental build.
rem Paths are anchored to this script, not the current working directory.
set "filter_config=Release"
if /I not "%~1"=="/64x" goto usage
if not "%~2"=="" (
    if /I "%~2"=="/D" (
        set "filter_config=Debug"
    ) else if /I not "%~2"=="/R" goto usage
)
if not "%~3"=="" goto usage

if not defined BDS (
    where fv_vars.bat >nul 2>&1
    if errorlevel 1 (
        echo ERROR: Run from the RAD Studio command prompt, or put fv_vars.bat on PATH.
        exit /b 1
    )
    call fv_vars.bat
)
if not defined BDS (
    echo ERROR: RAD Studio environment initialization did not set BDS.
    exit /b 1
)
if not defined BDSBOOSTROOT set "BDSBOOSTROOT=%BDS%\include\boost_1_89"
if not defined BDSCOMMONDIR (
    echo ERROR: BDSCOMMONDIR is not set. Run rsvars.bat or fv_vars.bat first.
    exit /b 1
)
where cmake.exe >nul 2>&1
if errorlevel 1 (
    echo ERROR: Embarcadero CMake is not on PATH.
    exit /b 1
)
where ninja.exe >nul 2>&1
if errorlevel 1 (
    echo ERROR: Ninja is not on PATH.
    exit /b 1
)
set "filter_compiler=%BDS%\bin64\bcc64x.exe"
if not exist "%filter_compiler%" (
    echo ERROR: Cannot find "%filter_compiler%".
    exit /b 1
)
set "filter_build_dir=%~dp0build\Win64x\%filter_config%"
set "filter_fresh="
set "filter_clean="
if /I "%~n0"=="build" (
    set "filter_fresh=--fresh"
    set "filter_clean=--clean-first"
)
cmake %filter_fresh% -S "%~dp0." -B "%filter_build_dir%" -G Ninja ^
    "-DCMAKE_C_COMPILER=%filter_compiler%" "-DCMAKE_CXX_COMPILER=%filter_compiler%" ^
    "-DCMAKE_BUILD_TYPE=%filter_config%" "-DBOOST_ROOT=%BDSBOOSTROOT%" ^
    "-DBDS_COMMON_DIR=%BDSCOMMONDIR%"
if errorlevel 1 exit /b 1
cmake --build "%filter_build_dir%" %filter_clean%
if errorlevel 1 exit /b 1
echo Static/import libraries are in "%BDSCOMMONDIR%\lib\Win64x\%filter_config%".
echo DLLs are in "%BDSCOMMONDIR%\DLL\Win64x\%filter_config%".
exit /b 0

:usage
echo Usage: %~nx0 /64x [/D ^| /R]
echo        /D builds Debug; /R builds Release ^(the default^).
exit /b 2
