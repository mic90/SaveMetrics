@echo off
set INSTALL_DEST=%CD%\..\luarocks
set LUA_INCLUDE=%CD%\..\lua\include
set LUA_BIN=%CD%\..\lua\bin
set LUA_LIB=%LUA_BIN%
echo Installing luarocks... >&2
cd %CD%\luarocks
call install.bat /NOADMIN /MW /F /SELFCONTAINED /FORCECONFIG /Q  /P %INSTALL_DEST% /INC %LUA_INCLUDE% /LIB %LUA_LIB% /BIN %LUA_BIN%