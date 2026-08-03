//S25I - Miguel Ignacio

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

#include "kernel_c.h"

// x86-64 assembly kernel declaration
extern void distance_asm(int n,
                         const double* X1, const double* X2,
                         const double* Y1, const double* Y2,
                         double* Z);

#define NUM_RUNS 30
#define EPSILON 1e-6

// Helper for timing
static double get_time_ms(LARGE_INTEGER start, LARGE_INTEGER end, LARGE_INTEGER freq) {
    return ((double)(end.QuadPart - start.QuadPart) * 1000.0) / (double)freq.QuadPart;
}

// 1. Sanity check with the worked example from the assignment prompt (n = 4)
void test_example() {
    double X1[4] = { 1.5, 4.0, 3.5, 2.0 };
    double X2[4] = { 3.0, 2.5, 2.5, 1.0 };
    double Y1[4] = { 4.0, 3.0, 3.5, 3.0 };
    double Y2[4] = { 2.0, 2.5, 1.0, 1.5 };
    double expected[4] = { 2.5, 1.58113883, 2.692582404, 1.802775638 };
    
    double Z_c[4], Z_asm[4];
    int pass_c = 1, pass_asm = 1;

    printf("===========================================\n");
    printf("   Sanity Check: Worked Example (n = 4)   \n");
    printf("===========================================\n");

    distance_c(4, X1, X2, Y1, Y2, Z_c);
    distance_asm(4, X1, X2, Y1, Y2, Z_asm);

    printf("i\tExpected\tC Kernel\tASM Kernel\n");
    for (int i = 0; i < 4; i++) {
        printf("%d\t%.8f\t%.8f\t%.8f\n", i, expected[i], Z_c[i], Z_asm[i]);
        if (fabs(Z_c[i] - expected[i]) > EPSILON) pass_c = 0;
        if (fabs(Z_asm[i] - expected[i]) > EPSILON) pass_asm = 0;
    }

    printf("C Kernel Status:   %s\n", pass_c ? "CORRECT" : "INCORRECT");
    printf("ASM Kernel Status: %s\n\n", pass_asm ? "CORRECT" : "INCORRECT");
}

// Fills vector with random double values
void fill_random(double* vec, int n) {
    for (int i = 0; i < n; i++) {
        vec[i] = ((double)rand() / RAND_MAX) * 200.0 - 100.0;
    }
}

// 2. Correctness check on larger random dataset
void check_correctness(int n) {
    double *X1 = (double*)malloc(n * sizeof(double));
    double *X2 = (double*)malloc(n * sizeof(double));
    double *Y1 = (double*)malloc(n * sizeof(double));
    double *Y2 = (double*)malloc(n * sizeof(double));
    double *Z_c = (double*)malloc(n * sizeof(double));
    double *Z_asm = (double*)malloc(n * sizeof(double));

    if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
        printf("Memory allocation failed!\n");
        return;
    }

    fill_random(X1, n);
    fill_random(X2, n);
    fill_random(Y1, n);
    fill_random(Y2, n);

    distance_c(n, X1, X2, Y1, Y2, Z_c);
    distance_asm(n, X1, X2, Y1, Y2, Z_asm);

    int correct = 1;
    for (int i = 0; i < n; i++) {
        if (fabs(Z_c[i] - Z_asm[i]) > EPSILON) {
            correct = 0;
            break;
        }
    }

    printf("===========================================\n");
    printf("  Correctness Check: C vs ASM (n = %d)  \n", n);
    printf("===========================================\n");
    printf("Output Match: %s\n\n", correct ? "PASS" : "FAIL");

    free(X1); free(X2); free(Y1); free(Y2); free(Z_c); free(Z_asm);
}

// 3. Performance benchmark
void run_benchmark(int exponent, LARGE_INTEGER freq) {
    int n = 1 << exponent;
    printf("===========================================\n");
    printf(" Performance Benchmark: n = 2^%d (%d)\n", exponent, n);
    printf("===========================================\n");

    double *X1 = (double*)malloc(n * sizeof(double));
    double *X2 = (double*)malloc(n * sizeof(double));
    double *Y1 = (double*)malloc(n * sizeof(double));
    double *Y2 = (double*)malloc(n * sizeof(double));
    double *Z_c = (double*)malloc(n * sizeof(double));
    double *Z_asm = (double*)malloc(n * sizeof(double));

    if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
        printf("Memory allocation failed for n = 2^%d! Skipping...\n\n", exponent);
        free(X1); free(X2); free(Y1); free(Y2); free(Z_c); free(Z_asm);
        return;
    }

    fill_random(X1, n);
    fill_random(X2, n);
    fill_random(Y1, n);
    fill_random(Y2, n);

    LARGE_INTEGER start, end;
    double total_time_c = 0.0;
    double total_time_asm = 0.0;

    // Benchmark C Kernel
    for (int r = 0; r < NUM_RUNS; r++) {
        QueryPerformanceCounter(&start);
        distance_c(n, X1, X2, Y1, Y2, Z_c);
        QueryPerformanceCounter(&end);
        total_time_c += get_time_ms(start, end, freq);
    }

    // Benchmark ASM Kernel
    for (int r = 0; r < NUM_RUNS; r++) {
        QueryPerformanceCounter(&start);
        distance_asm(n, X1, X2, Y1, Y2, Z_asm);
        QueryPerformanceCounter(&end);
        total_time_asm += get_time_ms(start, end, freq);
    }

    double avg_c = total_time_c / NUM_RUNS;
    double avg_asm = total_time_asm / NUM_RUNS;

    printf("Average C Kernel Execution Time:   %.4f ms\n", avg_c);
    printf("Average ASM Kernel Execution Time: %.4f ms\n", avg_asm);
    printf("Speedup (C / ASM): %.2fx\n\n", avg_c / avg_asm);

    // Display first 10 elements of vector Z
    printf("First 10 elements of vector Z:\n");
    printf("i\tZ (C)\t\tZ (ASM)\n");
    for (int i = 0; i < 10 && i < n; i++) {
        printf("%d\t%.8f\t%.8f\n", i, Z_c[i], Z_asm[i]);
    }
    printf("\n");

    free(X1); free(X2); free(Y1); free(Y2); free(Z_c); free(Z_asm);
}

int main() {
    srand((unsigned int)time(NULL));

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    // Run worked example sanity check
    test_example();

    // Run correctness check
    check_correctness(65536);

    // Run timing benchmark for n = 2^20, 2^24, 2^28
    int exponents[] = { 20, 24, 28 };
    int num_tests = sizeof(exponents) / sizeof(exponents[0]);

    for (int i = 0; i < num_tests; i++) {
        run_benchmark(exponents[i], freq);
    }

    return 0;
}

