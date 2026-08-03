;S25I - Miguel Ignacio

.code

distance_asm PROC
        ; ---- grab stack-passed args FIRST, before any push shifts rsp ----
        mov     r10, qword ptr [rsp+40]     ; r10 = Y2
        mov     r11, qword ptr [rsp+48]     ; r11 = Z

        ; ---- save non-volatile registers we're about to use ----
        push    rbx
        push    rsi
        push    rdi
        push    r12
        push    r13
        push    r14

        ; ---- move parameters into stable, non-volatile registers ----
        mov     ebx, ecx        ; ebx = n
        mov     rsi, rdx        ; rsi = X1
        mov     rdi, r8         ; rdi = X2
        mov     r12, r9         ; r12 = Y1
        mov     r13, r10        ; r13 = Y2
        mov     r14, r11        ; r14 = Z

        xor     eax, eax        ; eax = i = 0
        test    ebx, ebx
        jle     done            ; if n <= 0, nothing to do

loop_start:
        cmp     eax, ebx
        jge     done

        ; ---- dx = X2[i] - X1[i]; dx2 = dx*dx ----
        movsd   xmm0, real8 ptr [rdi+rax*8]    ; xmm0 = X2[i]
        subsd   xmm0, real8 ptr [rsi+rax*8]    ; xmm0 = X2[i] - X1[i]
        mulsd   xmm0, xmm0                     ; xmm0 = dx*dx

        ; ---- dy = Y2[i] - Y1[i]; dy2 = dy*dy ----
        movsd   xmm1, real8 ptr [r13+rax*8]    ; xmm1 = Y2[i]
        subsd   xmm1, real8 ptr [r12+rax*8]    ; xmm1 = Y2[i] - Y1[i]
        mulsd   xmm1, xmm1                     ; xmm1 = dy*dy

        ; ---- Z[i] = sqrt(dx2 + dy2) ----
        addsd   xmm0, xmm1                     ; xmm0 = dx*dx + dy*dy
        sqrtsd  xmm0, xmm0                     ; xmm0 = sqrt(...)
        movsd   real8 ptr [r14+rax*8], xmm0    ; Z[i] = xmm0

        inc     eax
        jmp     loop_start

done:
        pop     r14
        pop     r13
        pop     r12
        pop     rdi
        pop     rsi
        pop     rbx
        ret
distance_asm ENDP

END
