/*=============================================================================
 * main.c
 *
 * Driver program that calls both the C kernel and the x86-64 assembly
 * kernel and compares their output.
 *
 * Build target: Windows x64 / Visual Studio (uses QueryPerformanceCounter).
 *=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

#include "kernel_c.h"

/* the x86-64 assembly kernel (see kernel_asm.asm) */
extern void distance_asm(int n,
                          const double* X1, const double* X2,
                          const double* Y1, const double* Y2,
                          double* Z);

#define EPSILON    1e-9

/* ---------------------------------------------------------------------
 * Step 1: worked example from the assignment (n = 4)
 * ------------------------------------------------------------------- */
static void run_example(void) {
    double X1[4] = { 1.5, 4.0, 3.5, 2.0 };
    double X2[4] = { 3.0, 2.5, 2.5, 1.0 };
    double Y1[4] = { 4.0, 3.0, 3.5, 3.0 };
    double Y2[4] = { 2.0, 2.5, 1.0, 1.5 };
    double expected[4] = { 2.5, 1.58113883, 2.692582404, 1.802775638 };
    double Zc[4], Za[4];
    int i;
    int ok_c = 1, ok_a = 1;

    printf("=========================================================\n");
    printf(" STEP 1: Worked example (n = 4) - sanity check vs answer key\n");
    printf("=========================================================\n");

    distance_c(4, X1, X2, Y1, Y2, Zc);
    distance_asm(4, X1, X2, Y1, Y2, Za);

    printf("%-4s %-14s %-14s %-14s\n", "i", "expected", "C kernel", "ASM kernel");
    for (i = 0; i < 4; i++) {
        printf("%-4d %-14.9f %-14.9f %-14.9f\n", i, expected[i], Zc[i], Za[i]);
        if (fabs(Zc[i] - expected[i]) > EPSILON) ok_c = 0;
        if (fabs(Za[i] - expected[i]) > EPSILON) ok_a = 0;
    }
    printf("C kernel   vs answer key : %s\n", ok_c ? "PASS" : "FAIL");
    printf("ASM kernel vs answer key : %s\n\n", ok_a ? "PASS" : "FAIL");
}

int main(void) {
    run_example();
    return 0;
}
