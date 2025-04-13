setlocal EnableDelayedExpansion
SET "QT_DIR=C:\Qt\6.8.1\llvm-mingw_64\bin"
SET "EXE_NAME=falloutequestria.exe"
SET "REL_PATH=build\windows-release"
SET "EXE_PATH=%REL_PATH%\%EXE_PATH%"
SET "WINDEPLOY_QT=%QT_DIR%\windeployqt.ee"

MKDIR "%REL_PATH%"
COPY "%1" "%EXE_PATH%"
PUSHD "%REL_PATH%"

%WINDEPLOY_QT%  --qmldir "..\..\src" "%EXE_NAME%"
