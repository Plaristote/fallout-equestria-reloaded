setlocal EnableDelayedExpansion
SET "QT_DIR=C:\Qt\6.8.1\llvm-mingw_64\bin"
SET "RELATIVE_SOURCE_DIR=src\"
SET "windeploy_qt=%QT_DIR%\windeployqt.exe"
SET "EXE_PATH=build\windows-release\falloutequestria.exe"
MKDIR build\windows-release
CD build\windows-release

COPY "%1" "%EXE_PATH%"

%windeploy-qt%  --qmldir "..\..\%RELATIVE_SOURCE_DIR%" "%EXE_PATH%"
