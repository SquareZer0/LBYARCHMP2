;=============================================================================
; kernel_asm.asm  (NASM syntax, for SASM + Dev-C++/MinGW-w64 toolchain)
;
; void distance_asm(int n, double* X1, double* X2, double* Y1, double* Y2, double* Z)
;
; Computes Z[i] = sqrt( (X2[i]-X1[i])^2 + (Y2[i]-Y1[i])^2 )  for i = 0..n-1
; using scalar SSE2 floating-point instructions (movsd/subsd/mulsd/addsd/sqrtsd)
; operating on the low 64-bit lane of the XMM registers.
;
; This targets the Microsoft x64 calling convention, which is what
; MinGW-w64 GCC (the compiler Dev-C++ uses on 64-bit Windows) follows too:
;   ecx = n            (arg1, 32-bit int)
;   rdx = X1            (arg2, pointer)
;   r8  = X2            (arg3, pointer)
;   r9  = Y1            (arg4, pointer)
;   [rsp+40] = Y2        (arg5, pointer, passed on the stack)
;   [rsp+48] = Z         (arg6, pointer, passed on the stack)
;
; Assemble with:  nasm -f win64 kernel_asm.asm -o kernel_asm.o
; (SASM's "Compile" button does this for you - see README-DevCpp-SASM.md)
;
; NOTE on symbol names: MinGW-w64 GCC on 64-bit Windows does NOT prepend
; an underscore to C symbol names (that's a 32-bit cdecl convention), so
; "distance_asm" here matches "extern void distance_asm(...)" in main.c
; exactly. If you ever build 32-bit, this whole file needs to change -
; different calling convention, different registers, no rax/rdi/r8-r15.
;=============================================================================

section .text
global distance_asm

distance_asm:
        ; ---- grab stack-passed args FIRST, before any push shifts rsp ----
        mov     r10, [rsp+40]       ; r10 = Y2
        mov     r11, [rsp+48]       ; r11 = Z

        ; ---- save non-volatile registers we're about to use ----
        push    rbx
        push    rsi
        push    rdi
        push    r12
        push    r13
        push    r14

        ; ---- move parameters into stable, non-volatile registers ----
        mov     ebx, ecx            ; ebx = n
        mov     rsi, rdx            ; rsi = X1
        mov     rdi, r8             ; rdi = X2
        mov     r12, r9             ; r12 = Y1
        mov     r13, r10            ; r13 = Y2
        mov     r14, r11            ; r14 = Z

        xor     eax, eax            ; eax = i = 0
        test    ebx, ebx
        jle     .done               ; if n <= 0, nothing to do

.loop_start:
        cmp     eax, ebx
        jge     .done

        ; ---- dx = X2[i] - X1[i]; dx2 = dx*dx ----
        movsd   xmm0, [rdi+rax*8]   ; xmm0 = X2[i]
        subsd   xmm0, [rsi+rax*8]   ; xmm0 = X2[i] - X1[i]
        mulsd   xmm0, xmm0          ; xmm0 = dx*dx

        ; ---- dy = Y2[i] - Y1[i]; dy2 = dy*dy ----
        movsd   xmm1, [r13+rax*8]   ; xmm1 = Y2[i]
        subsd   xmm1, [r12+rax*8]   ; xmm1 = Y2[i] - Y1[i]
        mulsd   xmm1, xmm1          ; xmm1 = dy*dy

        ; ---- Z[i] = sqrt(dx2 + dy2) ----
        addsd   xmm0, xmm1          ; xmm0 = dx*dx + dy*dy
        sqrtsd  xmm0, xmm0          ; xmm0 = sqrt(...)
        movsd   [r14+rax*8], xmm0   ; Z[i] = xmm0

        inc     eax
        jmp     .loop_start

.done:
        pop     r14
        pop     r13
        pop     r12
        pop     rdi
        pop     rsi
        pop     rbx
        ret
