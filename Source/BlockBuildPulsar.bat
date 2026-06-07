
echo Beginning!

SETLOCAL EnableDelayedExpansion
@echo off
cls
del build\*.o

echo Success, Next phase: Debug thing

::DEBUG only works if you have the map and readelf (which is part of MinGW)
SET "debug="
::SET "cwDWARF="
::if "%1" equ "-d" SET "debug=-debug=0x803992E0 -map=^"Dolphin Emulator\Maps\RMCP01.map^" -readelf=^"C:\MinGW\bin\readelf.exe^""
::if "%1" equ "-d" SET "cwDWARF=-g"

echo Success, Next phase: Sources and Compiler thing

:: Sources and Compiler
SET "ENGINE=.\KamekInclude"
set "GAMESOURCE=.\GameSource"
SET "PULSAR=.\PulsarEngine"

echo Success, Codewarrior Route

:: Change this as necessary depending on where you put CodeWarrior
SET CC="C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"

echo Success, Next phase: Riivolution Destination

:: Riivolution Destination (change as necessary)
SET "RIIVO="

echo Success, Next phase: Compiler flags and folder

:: Compiler flags and folder
SET CFLAGS=-I- -i %ENGINE% -i %GAMESOURCE% -i %PULSAR% ^
  -opt all -inline auto -enum int -fp hard -sdata 0 -sdata2 0 -maxerrors 1 -func_align 4 %cwDWARF%
SET DEFINE=

if "!CC!" == "" (
    echo You need to specify the path to CodeWarrior by setting the CC variable
    exit /b 1
)

echo Success, Next phase: CPP Sources

:: CPP Sources
SET CPPFILES=
for /R %PULSAR% %%f in (*.cpp) do SET "CPPFILES=%%f !CPPFILES!"

echo Success, Next phase: Compile CPP

:: Compile CPP
%CC% %CFLAGS% -c -o "build/kamek.o" "%ENGINE%\kamek.cpp"

echo Success, Next phase: Set objects thing
pause

SET OBJECTS=
FOR %%H IN (%CPPFILES%) DO (
    ::echo "Compiling %%H..."
    %CC% %CFLAGS% %DEFINE% -c -o "build/%%~nH.o" "%%H"
    SET "OBJECTS=build/%%~nH.o !OBJECTS!"
)

echo Success, Next phase: Link
pause

:: Link
echo Linking... %time%
".\KamekLinker\Kamek.exe" "build/kamek.o" %OBJECTS% %debug% -dynamic -externals="%GAMESOURCE%/symbols.txt" -versions="%GAMESOURCE%/versions.txt" -output-combined=build\Code.pul

echo Success, Next phase: final error check?
pause

if %ErrorLevel% equ 0 if NOT "!RIIVO!" == "" (
    xcopy /Y build\*.pul "%RIIVO%\Binaries" /i /q
    echo Binaries copied
)

:end

echo Full Success!
pause

ENDLOCAL