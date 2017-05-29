@echo off

set BUILD=%CD%\build
rmdir %BUILD% /s /q
mingw32-make.exe clean -s
mingw32-make.exe -f Makefile_windows -j6 TARGET_DEP=$(LUAJIT_A) -s

mkdir %BUILD%
mkdir %BUILD%\bin\
mkdir %BUILD%\include
 
copy src\luajit.exe %BUILD%\bin\luajit.exe
copy src\lua51.dll %BUILD%\bin\lua51.dll
copy src\libluajit.a %BUILD%\bin\libluajit.a
copy src\lauxlib.h %BUILD%\include\lauxlib.h
copy src\lua.h %BUILD%\include\lua.h
copy src\lua.hpp %BUILD%\include\lua.hpp
copy src\luaconf.h %BUILD%\include\luaconf.h
copy src\luajit.h %BUILD%\include\luajit.h
copy src\lualib.h %BUILD%\include\lualib.h
xcopy src\jit %BUILD%\bin\lua\jit /s /e /y /i