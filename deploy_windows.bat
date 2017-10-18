@echo off
set BIN_DIR=%CD%\bin
set LUA_JIT_VER=2.0.4
set LUA_JIT_DIR=%CD%\external\luajit-%LUA_JIT_VER%
set PROJECT_DIR=%CD%

REM copy luajit build dir to ide bin
echo Copying luajit files to bin... >&2
RMDIR /s /q %BIN_DIR%\lua
MKDIR %BIN_DIR%\lua
ROBOCOPY %LUA_JIT_DIR%\build %BIN_DIR%\lua /MIR /NFL /NDL /NJH /NJS /nc /ns /np
echo Copying lua51.dll file to bin... >&2
COPY %LUA_JIT_DIR%\build\bin\lua51.dll %BIN_DIR%\lua51.dll

cd %CD%
