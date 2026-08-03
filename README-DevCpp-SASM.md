# Building & Demoing with SASM + Embarcadero Dev-C++

This project needs **two separate tools** because neither one alone handles
both C and assembly:

- **SASM** assembles `src/kernel_asm_nasm.asm` (NASM syntax) into an object
  file (`.o`).
- **Dev-C++** compiles `src/main.c` and `src/kernel_c.c`, then **links**
  them together with that object file into the final `.exe`.

SASM never touches the C code, and Dev-C++ never touches the `.asm` file
directly — it only needs the `.o` that SASM produces.

## ⚠️ Before you start: make sure everything is 64-bit

This assignment is x86-**64**. That only works if:
- Dev-C++ is set to build 64-bit binaries (Tools → Compiler Options → make
  sure a 64-bit MinGW-w64 profile is selected, not a 32-bit `i686` one).
- SASM assembles for 64-bit (`nasm -f win64`, not `-f win32`).

If you accidentally mix 32-bit and 64-bit pieces, linking will fail with
"undefined reference to `distance_asm`" (32-bit MinGW prepends an
underscore to symbol names, 64-bit doesn't — so the names won't match).

## Step 1 — Assemble the kernel in SASM

1. Open SASM, open `src/kernel_asm_nasm.asm`.
2. Go to **Settings** (gear icon) → make sure:
   - Assembler: **NASM**
   - Architecture: **x64**
3. This file has **no `main`/entry point** — it's just a function meant to
   be called from C, so don't try to "Run" it in SASM (it'll fail to link
   into a standalone program, which is expected). Instead:
   - Use **Build only / Compile** (not "Build and Run") so SASM stops
     after assembling, or just let it fail at the link stage — either way,
     the object file gets created in the same folder as the `.asm` file.
4. Confirm the `.o` file exists (e.g. `kernel_asm_nasm.o`).

   If you'd rather not fight SASM's UI, you can do the same thing from a
   command prompt (SASM ships `nasm.exe`, or install NASM separately and
   put it on your PATH):
   ```
   nasm -f win64 src\kernel_asm_nasm.asm -o kernel_asm.o
   ```

## Step 2 — Set up the Dev-C++ project

1. Dev-C++ → **File → New → Project** → *Console Application* → **C
   Project** (not C++) → name it `VectorDistanceKernel`.
2. Make sure the project is targeting **64-bit** (Project Options →
   general/compiler tab has an architecture setting in recent Embarcadero
   Dev-C++ builds).
3. **Project → Add to Project…** and add:
   - `src/main.c`
   - `src/kernel_c.c`
   - `src/kernel_c.h` (optional to add, but harmless)

## Step 3 — Link in the assembled object file

Dev-C++ doesn't have a menu option to "add an object file" the same way it
adds source files, so you feed it to the linker directly:

1. **Project → Project Options → Parameters** tab.
2. In the **Linker** box (bottom one, not the compiler one), add the full
   path to your assembled object file, e.g.:
   ```
   "C:\path\to\kernel_asm_nasm.o"
   ```
3. Click OK.

## Step 4 — Build & run

**Execute → Compile** (or F9/F11 to compile+run). Dev-C++ will:
- compile `main.c` and `kernel_c.c` with its bundled MinGW-w64 GCC,
- link the results together with `kernel_asm_nasm.o`,
- produce `VectorDistanceKernel.exe`.

Run it — you should see the same three sections as before (worked example
vs. answer key, correctness check, timing benchmark).

## Command-line alternative (useful for the video, since it's faster to show)

If you add Dev-C++'s bundled MinGW `bin` folder to your PATH (something
like `C:\Program Files\Embarcadero\Dev-Cpp\MinGW64\bin`), you can do the
whole build in one shot from a terminal — good for a quick, legible video
segment:

```bat
nasm -f win64 src\kernel_asm_nasm.asm -o kernel_asm.o
gcc -O2 -Isrc -c src\main.c -o main.o
gcc -O2 -Isrc -c src\kernel_c.c -o kernel_c.o -msse2
gcc main.o kernel_c.o kernel_asm.o -o VectorDistanceKernel.exe -lm
VectorDistanceKernel.exe
```

(I test-built this exact pipeline — NASM assembling the kernel, MinGW-w64
GCC compiling and linking it with the C sources — and confirmed it
produces a valid PE32+ Windows x86-64 executable, so the toolchain
combination itself is sound.)

## What to show in the video

1. `src/kernel_asm_nasm.asm` open in SASM — walk through the register
   usage (Microsoft x64 calling convention) and the scalar SSE2
   instructions (`movsd`/`subsd`/`mulsd`/`addsd`/`sqrtsd`).
2. Assembling it in SASM (or via `nasm` on the command line).
3. `main.c` / `kernel_c.c` open in Dev-C++ — briefly point out the SSE2
   intrinsics in `kernel_c.c` and the `extern void distance_asm(...)`
   declaration in `main.c` that ties the two languages together.
4. Adding the object file in Project Options → Parameters.
5. Compiling and running — showing the worked-example check, the
   correctness check (C vs ASM on random data), and the timing results.

## Troubleshooting

- **`undefined reference to 'distance_asm'`** → almost always a 32-bit vs
  64-bit mismatch, or SASM assembled with `-f win32` instead of `-f win64`.
- **`undefined reference to '_distance_asm'`** (leading underscore) →
  you're on a 32-bit toolchain somewhere; switch everything to 64-bit.
- **NASM syntax errors** → make sure you're using `src/kernel_asm_nasm.asm`
  (NASM syntax), not `src/kernel_asm.asm` (that one is MASM syntax, for
  Visual Studio's `ml64` — it will not assemble in NASM/SASM).
