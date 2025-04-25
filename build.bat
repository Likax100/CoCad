@echo off
setlocal enabledelayedexpansion
set files=
for %%f in (.\src\*.cpp) do (
    rem Exclude CoCad_Server.cpp as we build it seperately 
    if not "%%~nxf"=="CoCad_Server.cpp" (
      set files=!files! %%f
    ) 
)
g++ -m64 -std=c++17 -I"C:\tools\msys64\mingw64\include" -I"C:\Users\lukas\OneDrive\Documents\GitHub\CoCad\include" -L"C:\tools\msys64\mingw64\lib" -o app.exe !files! .\src\glad.c -g -lopengl32 -lglfw3 -lgdi32 -lsqlite3 -lpthread -lws2_32 -lmswsock


