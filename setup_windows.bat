@echo off
set MINGW_DIR=C:\Qt\Tools\mingw530_32\bin
set BIN_DIR=%CD%\bin
set LUA_JIT_VER=2.0.4
set LUA_JIT_DIR=%CD%\external\luajit-%LUA_JIT_VER%
set LUAROCKS_VER=2.4.2-win32
set LUAROCKS_DIR=%CD%\external\luarocks-%LUAROCKS_VER%
set PROJECT_DIR=%CD%

set BUILD_TPYE="RELEASE"
set BUILD_LUA=0
set INSTALL_LUAROCKS=1
set INSTALL_LUAROCKS_PACKAGES=mobdebug luacheck busted luacov
set CLEAN_IDE_SETTINGS=0

REM rebuild luajit 
if %BUILD_LUA% == 1 (
	echo Building LuaJIT >&2
	cd %LUA_JIT_DIR%
	call build_windows.bat
)

REM copy luajit build dir to ide bin
echo Copying luajit files to bin... >&2
RMDIR /s /q %BIN_DIR%\lua
MKDIR %BIN_DIR%\lua
ROBOCOPY %LUA_JIT_DIR%\build %BIN_DIR%\lua /MIR /NFL /NDL /NJH /NJS /nc /ns /np
echo Copying lua51.dll file to bin... >&2
COPY %LUA_JIT_DIR%\build\bin\lua51.dll %BIN_DIR%\lua51.dll

REM copy setup scripts to bin/setup
echo Copying setup scripts to bin/setup... >&2
RMDIR /s /q %BIN_DIR%\setup
MKDIR %BIN_DIR%\setup
COPY %PROJECT_DIR%\setup_clean_windows.bat %BIN_DIR%\setup\setup_clean_windows.bat
COPY %PROJECT_DIR%\setup_luarocks_windows.bat %BIN_DIR%\setup\setup_luarocks_windows.bat

REM copy luarocks sources to bin
echo Copying luarocks sources to bin/setup... >&2
ROBOCOPY %LUAROCKS_DIR% %BIN_DIR%\setup\luarocks /MIR /NFL /NDL /NJH /NJS /nc /ns /np

REM install luarocks inside bin dir
if %INSTALL_LUAROCKS% == 1 (
	echo Installing luarocks inside bin...
	cd %BIN_DIR%\setup\
	call setup_luarocks_windows.bat
	cd %BIN_DIR%\luarocks\
	for %%a in (%INSTALL_LUAROCKS_PACKAGES%) do (
		echo Installing lua package %%a... >&2
		call luarocks.bat install %%a
	)
)

cd %CD%
