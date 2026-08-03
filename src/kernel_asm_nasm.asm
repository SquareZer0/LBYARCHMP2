; kernel_asm_nasm.asm
; void distance_asm(int n, double* X1, double* X2, double* Y1, double* Y2, double* Z)
; Computes Z[i] = sqrt((X2[i]-X1[i])^2 + (Y2[i]-Y1[i])^2) using scalar SIMD instructions

section .text
global distance_asm

distance_asm:
    ; Arguments (Windows x64 ABI):
    ; ecx = n
    ; rdx = X1
    ; r8  = X2
    ; r9  = Y1
    ; [rsp+40] = Y2
    ; [rsp+48] = Z

    mov     r10, [rsp+40]               ; Y2
    mov     r11, [rsp+48]               ; Z

    xor     rax, rax                    ; i = 0

.loop_start:
    cmp     eax, ecx
    jge     .done

    ; dx = X2[i] - X1[i]
    movsd   xmm0, [r8 + rax*8]
    subsd   xmm0, [rdx + rax*8]
    mulsd   xmm0, xmm0                  ; dx^2

    ; dy = Y2[i] - Y1[i]
    movsd   xmm1, [r10 + rax*8]
    subsd   xmm1, [r9 + rax*8]
    mulsd   xmm1, xmm1                  ; dy^2

    ; Z[i] = sqrt(dx^2 + dy^2)
    addsd   xmm0, xmm1
    sqrtsd  xmm0, xmm0
    movsd   [r11 + rax*8], xmm0

    inc     eax
    jmp     .loop_start

.done:
    ret

