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

/* ---------------------------------------------------------------------
 * fills a buffer with random doubles in [lo, hi)
 * ------------------------------------------------------------------- */
static void fill_random(double* a, int n, double lo, double hi) {
    int i;
    for (i = 0; i < n; i++) {
        double r = (double)rand() / (double)RAND_MAX;
        a[i] = lo + r * (hi - lo);
    }
}

/* ---------------------------------------------------------------------
 * Step 2: correctness check between C and ASM on random data
 * ------------------------------------------------------------------- */
static void run_correctness_check(int n) {
    double *X1, *X2, *Y1, *Y2, *Zc, *Za;
    int i;
    double max_diff = 0.0;
    int mismatches = 0;

    printf("=========================================================\n");
    printf(" STEP 2: Correctness check, C vs ASM (n = %d, random data)\n", n);
    printf("=========================================================\n");

    X1 = (double*)malloc(n * sizeof(double));
    X2 = (double*)malloc(n * sizeof(double));
    Y1 = (double*)malloc(n * sizeof(double));
    Y2 = (double*)malloc(n * sizeof(double));
    Zc = (double*)malloc(n * sizeof(double));
    Za = (double*)malloc(n * sizeof(double));
    if (!X1 || !X2 || !Y1 || !Y2 || !Zc || !Za) {
        fprintf(stderr, "allocation failure in correctness check\n");
        exit(1);
    }

    fill_random(X1, n, -100.0, 100.0);
    fill_random(X2, n, -100.0, 100.0);
    fill_random(Y1, n, -100.0, 100.0);
    fill_random(Y2, n, -100.0, 100.0);

    distance_c(n, X1, X2, Y1, Y2, Zc);
    distance_asm(n, X1, X2, Y1, Y2, Za);

    for (i = 0; i < n; i++) {
        double d = fabs(Zc[i] - Za[i]);
        if (d > max_diff) max_diff = d;
        if (d > EPSILON) mismatches++;
    }

    printf("max |Zc - Za| = %.3e\n", max_diff);
    printf("mismatches (> %.1e) = %d out of %d\n", EPSILON, mismatches, n);
    printf("Correctness check: %s\n\n", (mismatches == 0) ? "PASS" : "FAIL");

    free(X1); free(X2); free(Y1); free(Y2); free(Zc); free(Za);
}

int main(void) {
    srand((unsigned int)time(NULL));

    run_example();
    run_correctness_check(1 << 16); /* 65536 random elements */

    return 0;
}
