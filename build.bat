@echo off
REM Builds the project using NASM (kernel_asm_nasm.asm) + Dev-C++'s bundled
REM MinGW-w64 GCC. Run this from a Windows command prompt with both `nasm`
REM and `gcc` on PATH (add Dev-C++'s MinGW64\bin folder to PATH if needed).

echo Assembling x86-64 kernel with NASM...
nasm -f win64 src\kernel_asm_nasm.asm -o kernel_asm.o
if errorlevel 1 goto :error

echo Compiling C sources with GCC...
gcc -O2 -Isrc -c src\main.c -o main.o
if errorlevel 1 goto :error
gcc -O2 -Isrc -c src\kernel_c.c -o kernel_c.o -msse2
if errorlevel 1 goto :error

echo Linking...
gcc main.o kernel_c.o kernel_asm.o -o VectorDistanceKernel.exe -lm
if errorlevel 1 goto :error

echo.
echo Build succeeded: VectorDistanceKernel.exe
goto :eof

:error
echo.
echo Build FAILED.
exit /b 1
