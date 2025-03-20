@echo off
setlocal enabledelayedexpansion
set files=
for %%f in (.\src\*.cpp) do (
    set files=!files! %%f
)
g++ -m64 -std=c++17 -I"C:\tools\msys64\mingw64\include" -I"K:\Uni\Final Year\c1682 - FYP\CADSoftware - CoCAD\include" -L"C:\tools\msys64\mingw64\lib" -L"C:\tools\sqlite3" -o app.exe !files! .\src\glad.c -lopengl32 -lglfw3 -lgdi32 -lsqlite3


